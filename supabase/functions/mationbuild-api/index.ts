import { createClient } from "npm:@supabase/supabase-js@2";
const corsHeaders = {
  "Access-Control-Allow-Origin": Deno.env.get("ALLOWED_ORIGIN") ?? "https://solancegmd.github.io",
  "Access-Control-Allow-Headers": "authorization, x-client-info, apikey, content-type, x-admin-token",
  "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
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
const dbClient = () => createClient(Deno.env.get("SUPABASE_URL")!, Deno.env.get("SUPABASE_SERVICE_ROLE_KEY")!, { auth: { persistSession: false } });
Deno.serve(async req => {
  if (req.method === "OPTIONS") return new Response("ok", { headers: corsHeaders });
  const url = new URL(req.url);
  const db = dbClient();
  if (req.method === "GET" && url.pathname.endsWith("/templates")) {
    const { data, error } = await db.from("mationbuild_templates").select("slug,title,description,author_name,source_url,download_url,game_version,schema_version,tags,created_at").eq("status", "published").order("created_at", { ascending: false }).limit(100);
    if (error) return reply({ error: "catalog_unavailable" }, 503);
    return reply({ templates: data ?? [] });
  }
  if (req.method !== "POST") return reply({ error: "method_not_allowed" }, 405);
  let body: Record<string, unknown>;
  try { body = await req.json(); } catch { return reply({ error: "invalid_json" }, 400); }
  if (body.action === "verify") {
    const key = typeof body.key === "string" ? body.key.trim().toUpperCase() : "";
    if (!/^MB-(?:[A-Z2-9]{6}-){2}[A-Z2-9]{6}$/.test(key)) return reply({ valid: false, message: "Invalid key format." }, 400);
    const { data, error } = await db.from("mationbuild_keys").select("id,expires_at,revoked_at").eq("key_hash", await sha256(key)).maybeSingle();
    if (error) return reply({ valid: false, message: "Verification is temporarily unavailable." }, 503);
    if (!data || data.revoked_at || (data.expires_at && Date.parse(data.expires_at) <= Date.now())) return reply({ valid: false, message: "This key is invalid, expired, or revoked." }, 401);
    await db.from("mationbuild_keys").update({ last_used_at: new Date().toISOString() }).eq("id", data.id);
    return reply({ valid: true, message: "Key verified." });
  }
  if (body.action === "issue") {
    const adminToken = Deno.env.get("MATIONBUILD_ADMIN_TOKEN");
    if (!adminToken || req.headers.get("x-admin-token") !== adminToken) return reply({ error: "forbidden" }, 403);
    const label = typeof body.label === "string" ? body.label.slice(0, 80) : "Community access";
    const days = Number.isInteger(body.days) ? Math.max(1, Math.min(3650, Number(body.days))) : 365;
    const key = makeKey();
    const expiresAt = new Date(Date.now() + days * 86400000).toISOString();
    const { error } = await db.from("mationbuild_keys").insert({ key_hash: await sha256(key), label, expires_at: expiresAt });
    if (error) return reply({ error: "key_issue_failed" }, 500);
    return reply({ key, expires_at: expiresAt }, 201);
  }
  if (body.action === "submit-template") {
    const title = typeof body.title === "string" ? body.title.trim().slice(0, 100) : "";
    const author = typeof body.author_name === "string" ? body.author_name.trim().slice(0, 80) : "";
    const source = typeof body.source_url === "string" ? body.source_url : "";
    const download = typeof body.download_url === "string" ? body.download_url : "";
    const slug = typeof body.slug === "string" ? body.slug.toLowerCase().replace(/[^a-z0-9-]/g, "-").slice(0, 80) : "";
    if (title.length < 3 || !author || !slug || !source.startsWith("https://") || !download.startsWith("https://")) return reply({ error: "invalid_template_metadata" }, 400);
    const { error } = await db.from("mationbuild_templates").insert({
      slug, title, author_name: author, source_url: source, download_url: download,
      description: typeof body.description === "string" ? body.description.slice(0, 1000) : "",
      game_version: typeof body.game_version === "string" ? body.game_version.slice(0, 32) : null,
      tags: Array.isArray(body.tags) ? body.tags.filter(x => typeof x === "string").slice(0, 12).map(x => x.slice(0, 32)) : [],
      status: "pending"
    });
    if (error) return reply({ error: "template_submission_failed" }, 409);
    return reply({ submitted: true, message: "Submitted for review." }, 201);
  }
  return reply({ error: "unknown_action" }, 400);
});