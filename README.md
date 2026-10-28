# MationBuild

**MationBuild by Solance Good** is an early-stage Geometry Dash auto-decoration mod project with an access portal, Geode build workflow, and a deployed Supabase licensing/template API.

## Status — read before using

The repository now contains a first in-editor auto-decoration pass, a static access portal, and a deployed licensing/template API. In the editor, select up to 100 objects and use the MationBuild button; after server-side key verification it adds tiny-circle and tiny-cross accents near each selected object using the editor's paste/undo path. This is an intentionally small first pass, not feature parity with Antiprime AutoBuild. It still requires a successful build and in-game validation; native Antiprime template import is not implemented.

## Project files

- `src/main.cpp`, `mod.json`, `CMakeLists.txt` — Geode mod scaffold.
- `site/` — responsive access portal; uses the existing `MationBuild.ico`.
- `.github/workflows/geode-build.yml` — Windows .geode build and artifact upload.
- `.github/workflows/pages.yml` — GitHub Pages deployment.
- `supabase/migrations/`, `supabase/functions/` — database and API source.
- `scripts/mationbuild-admin.mjs` — private CLI for key issuance and template moderation; requires a private admin token.
- `docs/community-templates.md` — template compatibility and moderation policy.

## Build and website

- [GitHub Actions](https://github.com/SolanceGMD/MationBuild/actions) shows current builds and site checks.
- Enable Pages in **Settings → Pages → Source: GitHub Actions** if not already enabled.
- A successful build artifact is not a substitute for in-game compatibility testing.

## Backend

A dedicated Supabase project is deployed in TLAL, London (`eu-west-2`). The database schema, database-backed rate limiting and Edge Function are deployed; a basic rate-limit test passed. The website source is configured to call the verification endpoint. Project creation was approved at $0/month; future usage, add-ons, or plan changes could still incur costs, so monitor billing.

**Manual security step still required:** set `MATIONBUILD_ADMIN_TOKEN` and `ALLOWED_ORIGIN=https://solancegmd.github.io` in [Supabase Function secrets](https://supabase.com/dashboard/project/gfgmknlxzdfnjdpxetdz/functions/secrets). Key issuance and template moderation remain disabled until the admin token is configured. Follow [supabase/README.md](supabase/README.md). No service-role key or admin token is committed to this repository. The mod's access-key setting is stored in the local Geode settings file; use a private key and do not share it.

## Community templates and Antiprime

The project intends to support community-authored templates and, where permitted, compatibility with Antiprime AutoBuild templates. The native importer is not implemented yet. See [docs/community-templates.md](docs/community-templates.md). A permitted sample export and verified format are required before claiming compatibility.

MationBuild is an independent community project and is not affiliated with RobTop Games or Antiprime. Third-party assets remain their creators' property and must be used according to their terms.
