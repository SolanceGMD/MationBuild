import { createClient } from "npm:@supabase/supabase-js@2";

const allowedOrigin = Deno.env.get("ALLOWED_ORIGIN") ?? "https://solancegmd.github.io";
const corsHeaders = {
  "Access-Control-Allow-Origin": allowedOrigin,
  "Access-Control-Allow-Headers": "authorization, x-client-info, apikey, content-type, x-admin-token",
  "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
  "Access-Control-Max-Age": "86400",
  "Vary": "Origin",
  "Content-Type": "application/json; charset=utf-8",
};
function reply(body: unknown, status = 200) {
  return new Response(JSON.stringify(body), { status, headers: corsHeaders });
}
async function sha256(value: string) {
  const digest = await crypto.subtle.digest("SHA-256", new TextEncoder().encode(value));
  return Array.from(new Uint8Array(digest), b => b.toString(16).padStart(2, "0")).join("");
}
function makeKey() {
  const alphabet = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
  const bytes = crypto.getRandomValues(new Uint8Array(18));
  const token = Array.from(bytes, b => alphabet[b % alphabet.length]).join("");
  return "MB-" + token.match(/.{1,6}/g)!.join("-");
}
const dbClient = () => createClient(
  Deno.env.get("SUPABASE_URL")!,
  Deno.env.get("SUPABASE_SERVICE_ROLE_KEY")!,
  { auth: { persistSession: false } },
);
function clientAddress(req: Request) {
  // Hash before persistence; never store the raw address in the database.
  return req.headers.get("cf-connecting-ip")
    ?? req.headers.get("x-real-ip")
    ?? req.headers.get("x-forwarded-for")?.split(",").at(-1)?.trim()
    ?? "unknown";
}
async function rateLimit(db: ReturnType<typeof dbClient>, req: Request, action: string, limit: number, windowSeconds: number) {
  const identifierHash = await sha256(clientAddress(req) + "|mationbuild-rate-limit-v1");
  const { data, error } = await db.rpc("mationbuild_rate_limit", {
    p_identifier_hash: identifierHash,
    p_action: action,
    p_limit: limit,
    p_window_seconds: windowSeconds,
  });
  if (error) {
    console.error("Rate-limit service unavailable", error.message);
    return false; // Fail closed if the guard cannot be checked.
  }
  return data === true;
}
function isAdmin(req: Request) {
  const expected = Deno.env.get("MATIONBUILD_ADMIN_TOKEN");
  const supplied = req.headers.get("x-admin-token");
  return Boolean(expected && supplied && supplied === expected);
}
Deno.serve(async req => {
  if (req.method === "OPTIONS") return new Response("ok", { headers: corsHeaders });
  const url = new URL(req.url);
  const db = dbClient();

  if (req.method === "GET") {
    if (!await rateLimit(db, req, "catalog", 120, 60)) return reply({ error: "rate_limited" }, 429);
    const { data, error } = await db.from("mationbuild_templates")
      .select("slug,title,description,author_name,source_url,download_url,game_version,schema_version,tags,created_at")
      .eq("status", "published").order("created_at", { ascending: false }).limit(100);
    if (error) return reply({ error: "catalog_unavailable" }, 503);
    return reply({ templates: data ?? [] });
  }

  if (req.method !== "POST") return reply({ error: "method_not_allowed" }, 405);
  const declaredLength = Number(req.headers.get("content-length") ?? 0);
  if (declaredLength > 20_000) return reply({ error: "request_too_large" }, 413);
  let body: Record<string, unknown>;
  try {
    const raw = await req.text();
    if (raw.length > 20_000) return reply({ error: "request_too_large" }, 413);
    body = JSON.parse(raw);
    if (!body || typeof body !== "object" || Array.isArray(body)) throw new Error("invalid body");
  } catch {
    return reply({ error: "invalid_json" }, 400);
  }

  if (body.action === "verify") {
    if (!await rateLimit(db, req, "verify", 20, 60)) return reply({ valid: false, message: "Too many attempts. Try again in a minute." }, 429);
    const key = typeof body.key === "string" ? body.key.trim().toUpperCase() : "";
    if (!/^MB-(?:[A-HJ-NP-Z2-9]{6}-){2}[A-HJ-NP-Z2-9]{6}$/.test(key)) {
      return reply({ valid: false, message: "Invalid key format." }, 400);
    }
    const { data, error } = await db.from("mationbuild_keys")
      .select("id,expires_at,revoked_at").eq("key_hash", await sha256(key)).maybeSingle();
    if (error) return reply({ valid: false, message: "Verification is temporarily unavailable." }, 503);
    if (!data || data.revoked_at || (data.expires_at && Date.parse(data.expires_at) <= Date.now())) {
      return reply({ valid: false, message: "This key is invalid, expired, or revoked." }, 401);
    }
    const { error: updateError } = await db.from("mationbuild_keys")
      .update({ last_used_at: new Date().toISOString() }).eq("id", data.id);
    if (updateError) return reply({ valid: false, message: "Verification is temporarily unavailable." }, 503);
    return reply({ valid: true, message: "Key verified." });
  }

  if (body.action === "issue") {
    if (!isAdmin(req)) return reply({ error: "forbidden" }, 403);
    if (!await rateLimit(db, req, "issue", 10, 3600)) return reply({ error: "rate_limited" }, 429);
    const label = typeof body.label === "string" ? body.label.trim().slice(0, 80) : "Community access";
    const days = Number.isInteger(body.days) ? Math.max(1, Math.min(3650, Number(body.days))) : 365;
    const key = makeKey();
    const expiresAt = new Date(Date.now() + days * 86400000).toISOString();
    const { error } = await db.from("mationbuild_keys").insert({
      key_hash: await sha256(key), label: label || "Community access", expires_at: expiresAt,
    });
    if (error) return reply({ error: "key_issue_failed" }, 500);
    // The plaintext key is returned only once to the authenticated admin.
    return reply({ key, expires_at: expiresAt }, 201);
  }

  if (body.action === "revoke-key") {
    if (!isAdmin(req)) return reply({ error: "forbidden" }, 403);
    if (!await rateLimit(db, req, "moderate", 60, 60)) return reply({ error: "rate_limited" }, 429);
    const key = typeof body.key === "string" ? body.key.trim().toUpperCase() : "";
    if (!/^MB-(?:[A-HJ-NP-Z2-9]{6}-){2}[A-HJ-NP-Z2-9]{6}$/.test(key)) return reply({ error: "invalid_key_format" }, 400);
    const { data, error } = await db.from("mationbuild_keys")
      .update({ revoked_at: new Date().toISOString() })
      .eq("key_hash", await sha256(key)).is("revoked_at", null).select("id").maybeSingle();
    if (error) return reply({ error: "key_revoke_failed" }, 500);
    if (!data) return reply({ error: "key_not_found_or_already_revoked" }, 404);
    return reply({ revoked: true });
  }

  if (body.action === "list-pending-templates") {
    if (!isAdmin(req)) return reply({ error: "forbidden" }, 403);
    if (!await rateLimit(db, req, "moderate", 60, 60)) return reply({ error: "rate_limited" }, 429);
    const { data, error } = await db.from("mationbuild_templates")
      .select("slug,title,description,author_name,source_url,download_url,game_version,schema_version,tags,created_at")
      .eq("status", "pending").order("created_at", { ascending: true }).limit(100);
    if (error) return reply({ error: "pending_templates_unavailable" }, 503);
    return reply({ templates: data ?? [] });
  }

  if (body.action === "moderate-template") {
    if (!isAdmin(req)) return reply({ error: "forbidden" }, 403);
    if (!await rateLimit(db, req, "moderate", 60, 60)) return reply({ error: "rate_limited" }, 429);
    const slug = typeof body.slug === "string" ? body.slug.trim().slice(0, 80) : "";
    const status = body.status;
    if (!slug || !["published", "rejected", "removed"].includes(String(status))) {
      return reply({ error: "invalid_moderation_request" }, 400);
    }
    const { data, error } = await db.from("mationbuild_templates")
      .update({ status, updated_at: new Date().toISOString() })
      .eq("slug", slug).select("slug,status").maybeSingle();
    if (error) return reply({ error: "moderation_failed" }, 500);
    if (!data) return reply({ error: "template_not_found" }, 404);
    return reply({ template: data });
  }

  if (body.action === "submit-template") {
    if (!await rateLimit(db, req, "submit-template", 5, 3600)) return reply({ error: "rate_limited" }, 429);
    const title = typeof body.title === "string" ? body.title.trim().slice(0, 100) : "";
    const author = typeof body.author_name === "string" ? body.author_name.trim().slice(0, 80) : "";
    const source = typeof body.source_url === "string" ? body.source_url.trim() : "";
    const download = typeof body.download_url === "string" ? body.download_url.trim() : "";
    const slug = typeof body.slug === "string" ? body.slug.toLowerCase().replace(/[^a-z0-9-]/g, "-").replace(/-+/g, "-").replace(/^-|-$/g, "").slice(0, 80) : "";
    const isHttpsUrl = (value: string) => {
      try { const parsed = new URL(value); return parsed.protocol === "https:" && Boolean(parsed.hostname); }
      catch { return false; }
    };
    if (title.length < 3 || !author || !slug || !isHttpsUrl(source) || !isHttpsUrl(download)) {
      return reply({ error: "invalid_template_metadata" }, 400);
    }
    const { error } = await db.from("mationbuild_templates").insert({
      slug, title, author_name: author, source_url: source, download_url: download,
      description: typeof body.description === "string" ? body.description.slice(0, 1000) : "",
      game_version: typeof body.game_version === "string" ? body.game_version.slice(0, 32) : null,
      tags: Array.isArray(body.tags) ? body.tags.filter(x => typeof x === "string").slice(0, 12).map(x => x.slice(0, 32)) : [],
      status: "pending",
    });
    if (error) return reply({ error: "template_submission_failed" }, 409);
    return reply({ submitted: true, message: "Submitted for review." }, 201);
  }
  return reply({ error: "unknown_action" }, 400);
});