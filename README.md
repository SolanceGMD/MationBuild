# MationBuild

**MationBuild by Solance Good** is an early-stage Geometry Dash auto-decoration mod project with a GitHub Pages access portal, Geode build workflow, and starter licensing/template API.

## Status — read before using

This repository contains a **starter Geode mod**, build workflow, static access portal, and a deployed Supabase starter backend. The initial in-game popup is only a scaffold. It does **not yet perform automatic decoration, validate keys inside the mod, import Antiprime-native templates, or produce a verified release build**. Do not treat a build artifact as complete until the CI run succeeds and the resulting `.geode` is tested in-game.

## Project files

- `src/main.cpp`, `mod.json`, `CMakeLists.txt` — Geode mod scaffold.
- `site/` — responsive access portal; uses the existing `MationBuild.ico`.
- `.github/workflows/geode-build.yml` — Windows .geode build and artifact upload.
- `.github/workflows/pages.yml` — GitHub Pages deployment.
- `supabase/migrations/`, `supabase/functions/` — starter licensing and community-template API.
- `docs/community-templates.md` — template compatibility and moderation plan.

## Build

The GitHub Actions workflow uses the Geode SDK build action. See the [Actions tab](https://github.com/SolanceGMD/MationBuild/actions). A successful run uploads a Windows `.geode` artifact. Build success is not a substitute for an in-game compatibility test.

Geode's official mod workflow and metadata requirements are documented in the [Geode SDK documentation](https://docs.geode-sdk.org/).

## Enable GitHub Pages

1. Open **Settings → Pages**.
2. Set the source to **GitHub Actions**.
3. The workflow publishes the `site/` directory when the relevant files change.
4. The site URL appears under Settings → Pages.

## Licensing backend

A dedicated Supabase project is deployed in the TLAL organization on the free plan, in London (`eu-west-2`). The database migration is applied and the Edge Function is active. The access portal is configured to call the deployed verification endpoint. Project creation was quoted and approved at $0/month; future usage, add-ons, or plan changes could still incur costs, so monitor billing.

Follow [supabase/README.md](supabase/README.md) to configure the private admin token. Key issuance is not ready until that secret is set. Never place a Supabase service-role key or admin token in browser code or source control. The current API is a starter implementation; production release needs abuse protection, a secure admin workflow, revocation controls, and tests.

## Community templates and Antiprime

The project intends to support community-authored templates and, where permitted, compatibility with Antiprime AutoBuild templates. The current mod does **not** yet implement that importer. See [docs/community-templates.md](docs/community-templates.md) for the compatibility boundary and safe submission policy. Please provide an authorized sample export of the template format before claiming native compatibility.

MationBuild is an independent community project and is not affiliated with RobTop Games or Antiprime. Third-party assets remain their creators' property and must be used according to their terms.