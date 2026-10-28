# MationBuild

MationBuild is a Geometry Dash auto-decoration project scaffold with a polished GitHub Pages access portal and a CI foundation.

> **Current status:** the repository initially contained only `MationBuild.ico`. This commit adds the website and CI scaffolding, not a working Geometry Dash mod or a production key service. Those require the target mod framework and a deployed backend.

## Repository layout

- `site/` — responsive access portal; reuses the existing `MationBuild.ico`.
- `.github/workflows/pages.yml` — deploys the static site to GitHub Pages.
- `.github/workflows/ci.yml` — validates the site files.
- `supabase/` — server-side key service and SQL schema (next setup step).

## Enable GitHub Pages

1. Open **Settings → Pages**.
2. Set the source to **GitHub Actions**.
3. Push to `main`; the Pages workflow publishes the `site/` directory.
4. The site URL will be shown under Settings → Pages.

## Key system security

A browser-only page cannot securely generate or validate license keys. Never put a service-role key or signing secret in GitHub Pages JavaScript. The portal intentionally fails closed until `verifyEndpoint` in `site/app.js` is set to a deployed HTTPS backend.

For a production service, use a server-side database with hashed key storage, cryptographically random key generation, expiration/revocation, rate limiting, and admin-only issuance. Store backend secrets in the backend provider's secret manager, not in the repository. The key service is not deployed or configured by this scaffold.

## Mod / build status

A reliable Geode mod build needs the intended Geode SDK/toolchain, supported Geometry Dash version, and a real mod source tree. No mod source was present when this scaffold was added, so the CI workflow does not pretend to produce a working mod binary. Add the implementation and SDK setup before publishing releases.

Antiprime's project can be used as a high-level inspiration for workflow quality, but MationBuild should have its own implementation and UI rather than copying proprietary code or branding.