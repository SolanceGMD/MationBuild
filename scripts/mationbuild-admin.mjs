#!/usr/bin/env node
/**
 * MationBuild private admin CLI.
 * Set MATIONBUILD_ADMIN_TOKEN in your shell; never commit it or pass it as a command-line argument.
 * Usage:
 *   node scripts/mationbuild-admin.mjs pending
 *   node scripts/mationbuild-admin.mjs issue "Creator access" 365
 *   node scripts/mationbuild-admin.mjs revoke MB-AAAAAA-BBBBBB-CCCCCC
 *   node scripts/mationbuild-admin.mjs moderate <slug> publish|reject|remove
 */
const endpoint = process.env.MATIONBUILD_API_URL ?? "https://gfgmknlxzdfnjdpxetdz.supabase.co/functions/v1/mationbuild-api";
const token = process.env.MATIONBUILD_ADMIN_TOKEN;
const publishableKey = "sb_publishable_w0puiuo49MRiVpf4sQWj9Q_67R-YnCO";
if (!token) {
  console.error("Missing MATIONBUILD_ADMIN_TOKEN. Set it privately in your shell; do not put it in source code.");
  process.exit(2);
}
const [command, ...args] = process.argv.slice(2);
const request = async (body) => {
  const response = await fetch(endpoint, {
    method: "POST",
    headers: { "content-type": "application/json", "apikey": publishableKey, "x-admin-token": token },
    body: JSON.stringify(body),
  });
  const data = await response.json().catch(() => ({}));
  if (!response.ok) {
    console.error(`API error (${response.status}):`, data.error ?? data.message ?? "Request failed");
    process.exit(1);
  }
  return data;
};
let result;
switch (command) {
  case "pending":
    result = await request({ action: "list-pending-templates" });
    console.log(JSON.stringify(result.templates, null, 2));
    break;
  case "issue": {
    const [label = "Community access", daysText = "365"] = args;
    const days = Number(daysText);
    if (!Number.isInteger(days) || days < 1 || days > 3650) throw new Error("days must be an integer from 1 to 3650");
    result = await request({ action: "issue", label, days });
    // This is the only time the plaintext key is returned. Save it securely.
    console.log(JSON.stringify(result, null, 2));
    break;
  }
  case "revoke":
    if (!args[0]) throw new Error("Usage: revoke MB-XXXXXX-XXXXXX-XXXXXX");
    result = await request({ action: "revoke-key", key: args[0] });
    console.log(JSON.stringify(result, null, 2));
    break;
  case "moderate": {
    const [slug, decision] = args;
    const statusMap = { publish: "published", reject: "rejected", remove: "removed" };
    if (!slug || !statusMap[decision]) throw new Error("Usage: moderate <slug> publish|reject|remove");
    result = await request({ action: "moderate-template", slug, status: statusMap[decision] });
    console.log(JSON.stringify(result, null, 2));
    break;
  }
  default:
    console.error("Commands: pending | issue [label] [days] | revoke <key> | moderate <slug> publish|reject|remove");
    process.exit(2);
}
