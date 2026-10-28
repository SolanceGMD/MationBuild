/* MationBuild access portal. Public endpoint only; no secret keys belong in this file. */
const CONFIG = Object.freeze({
  verifyEndpoint: "https://gfgmknlxzdfnjdpxetdz.supabase.co/functions/v1/mationbuild-api"
});
const form = document.querySelector("#verify-form");
const keyInput = document.querySelector("#key");
const statusEl = document.querySelector("#status");
const submitButton = document.querySelector("#verify-button");
document.querySelector("#year").textContent = new Date().getFullYear();

form.addEventListener("submit", async (event) => {
  event.preventDefault();
  const key = keyInput.value.trim();
  statusEl.classList.remove("error");
  submitButton.disabled = true;
  submitButton.textContent = "Checking…";
  statusEl.textContent = "";
  try {
    const response = await fetch(CONFIG.verifyEndpoint, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ action: "verify", key })
    });
    const result = await response.json().catch(() => ({}));
    if (!response.ok || result.valid !== true) {
      throw new Error(result.message || "This key is invalid, expired, or revoked.");
    }
    statusEl.textContent = result.message || "Key verified. Access is active.";
  } catch (error) {
    statusEl.textContent = error.message || "Could not reach the verification service. Try again later.";
    statusEl.classList.add("error");
  } finally {
    submitButton.disabled = false;
    submitButton.textContent = "Verify key";
  }
});