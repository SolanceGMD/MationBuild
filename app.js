/* Public portal client. The publishable key is public; never ship service-role or admin secrets. */
const CONFIG = Object.freeze({
  endpoint: "https://gfgmknlxzdfnjdpxetdz.supabase.co/functions/v1/mationbuild-api",
  publishableKey: "sb_publishable_w0puiuo49MRiVpf4sQWj9Q_67R-YnCO"
});
const $ = (selector) => document.querySelector(selector);
const headers = { "Content-Type": "application/json", "apikey": CONFIG.publishableKey };
$("#year").textContent = new Date().getFullYear();

$("#verify-form").addEventListener("submit", async (event) => {
  event.preventDefault();
  const key = $("#key").value.trim();
  const status = $("#status");
  const button = $("#verify-button");
  status.classList.remove("error");
  button.disabled = true;
  button.textContent = "Checking…";
  status.textContent = "";
  try {
    const response = await fetch(CONFIG.endpoint, { method: "POST", headers, body: JSON.stringify({ action: "verify", key }) });
    const result = await response.json().catch(() => ({}));
    if (!response.ok || result.valid !== true) throw new Error(result.message || result.error || "This key is invalid, expired, or revoked.");
    status.textContent = "✓ " + (result.message || "Key verified. Access is active.");
  } catch (error) {
    status.textContent = error.message || "Could not reach the verification service. Try again later.";
    status.classList.add("error");
  } finally { button.disabled = false; button.textContent = "Verify key"; }
});
function showTemplateStatus(message, isError = false) { const el = $("#templates-status"); el.textContent = message; el.classList.toggle("error", isError); }
function safeLink(label, href) { const a = document.createElement("a"); a.textContent = label; a.href = href; a.target = "_blank"; a.rel = "noopener noreferrer nofollow"; return a; }
function renderTemplates(items) {
  const root = $("#template-list"); root.replaceChildren();
  if (!items.length) { const empty = document.createElement("div"); empty.className = "template-empty"; empty.textContent = "No approved templates yet. If you have a pack you’re allowed to share, submit it for review below."; root.append(empty); return; }
  for (const item of items) {
    const card = document.createElement("article"); card.className = "template-card";
    const title = document.createElement("h3"); title.textContent = item.title || item.slug || "Community template";
    const description = document.createElement("p"); description.textContent = item.description || "No description provided.";
    const byline = document.createElement("p"); byline.textContent = "By " + (item.author_name || "Unknown creator") + (item.game_version ? " · GD " + item.game_version : "");
    const tags = document.createElement("div"); tags.className = "template-meta";
    for (const value of (Array.isArray(item.tags) ? item.tags : []).slice(0, 12)) { const tag = document.createElement("span"); tag.className = "template-tag"; tag.textContent = String(value); tags.append(tag); }
    const links = document.createElement("div"); links.className = "template-links";
    if (isHttps(item.source_url)) links.append(safeLink("Source page ↗", item.source_url));
    if (isHttps(item.download_url)) links.append(safeLink("Creator download ↗", item.download_url));
    card.append(title, description, byline, tags, links); root.append(card);
  }
}
function isHttps(value) { try { return new URL(value).protocol === "https:"; } catch { return false; } }
async function loadTemplates() {
  showTemplateStatus("Loading community templates…"); $("#refresh-templates").disabled = true;
  try {
    const response = await fetch(CONFIG.endpoint, { headers: { "apikey": CONFIG.publishableKey } });
    const result = await response.json().catch(() => ({}));
    if (!response.ok || !Array.isArray(result.templates)) throw new Error(result.error || "Template library is temporarily unavailable.");
    renderTemplates(result.templates); showTemplateStatus(result.templates.length ? result.templates.length + " approved template(s)." : "Library loaded.");
  } catch (error) { renderTemplates([]); showTemplateStatus((error.message || "Could not load templates.") + " Check back later.", true); }
  finally { $("#refresh-templates").disabled = false; }
}
$("#refresh-templates").addEventListener("click", loadTemplates);
$("#template-form").addEventListener("submit", async (event) => {
  event.preventDefault(); const form = event.currentTarget; const button = $("#template-submit"); const status = $("#template-submit-status");
  const values = Object.fromEntries(new FormData(form).entries()); values.tags = (values.tags || "").split(",").map(x => x.trim()).filter(Boolean).slice(0, 12);
  for (const field of ["source_url", "download_url"]) if (!isHttps(values[field])) { status.textContent = "Both links must use HTTPS."; status.classList.add("error"); return; }
  status.classList.remove("error"); status.textContent = ""; button.disabled = true; button.textContent = "Submitting…";
  try {
    const response = await fetch(CONFIG.endpoint, { method: "POST", headers, body: JSON.stringify({ action: "submit-template", ...values }) });
    const result = await response.json().catch(() => ({}));
    if (!response.ok || result.submitted !== true) throw new Error(result.error || "Submission failed. Check the fields and try again.");
    form.reset(); $("#template-version").value = "2.2081"; status.textContent = "✓ " + (result.message || "Submitted for review.");
  } catch (error) { status.textContent = error.message || "Could not submit this template. Try again later."; status.classList.add("error"); }
  finally { button.disabled = false; button.textContent = "Submit for review"; }
});
loadTemplates();
