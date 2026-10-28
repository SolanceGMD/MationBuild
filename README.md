# MationBuild

**MationBuild by Solance Good** is an early-stage Geometry Dash auto-decoration mod project with an access portal, Geode build workflow, and a deployed Supabase licensing/template API.

## Status — read before using

The repository now contains a first in-editor auto-decoration pass, a static access portal, and a deployed licensing/template API. In the editor, select up to 100 objects and use the MationBuild button; after server-side key verification it adds small accents near each selected object using the editor's paste/undo path. Choose Tech, Minimal, or Geometric in the mod settings. This is an initial style-based generator, not feature parity with Antiprime AutoBuild. A Windows `.geode` artifact has been produced by GitHub Actions; it still needs an actual in-game compatibility test. The editor also supports the documented portable `mationbuild-json` v1 format; native Antiprime template import is not implemented.

## Project files

- `src/main.cpp`, `mod.json`, `CMakeLists.txt` — Geode editor tools, access-key check, and portable template importer.
- `site/` — responsive access portal; uses the existing `MationBuild.ico`.
- `.github/workflows/geode-build.yml` — Windows .geode build and artifact upload.
- `.github/workflows/pages.yml` — GitHub Pages deployment.
- `supabase/migrations/`, `supabase/functions/` — database and API source.
- `scripts/mationbuild-admin.mjs` — private CLI for key issuance and template moderation; requires a private admin token.
- `docs/community-templates.md` — template compatibility and moderation policy.

## Install and use the Windows build

1. Open [GitHub Actions](https://github.com/SolanceGMD/MationBuild/actions), open the latest successful **Build MationBuild Geode mod** run, and download the `MationBuild-Windows` artifact.
2. Extract the `.geode` file and install it using Geode's mod installer.
3. Open Geode's MationBuild settings and enter your access key.
4. In the level editor, select up to 100 anchor objects and press **Auto** to apply the selected Tech, Minimal, or Geometric accent style.
5. To import a community pack, download a `mationbuild-json` v1 JSON file, select anchor objects, press **Import**, and choose the file. Use Undo to revert an applied pass.

The portable importer only accepts the documented tiny decoration IDs 1764–1768. Native Antiprime files are not supported yet.

## Website

- [GitHub Actions](https://github.com/SolanceGMD/MationBuild/actions) shows builds and site checks.
- Enable Pages in **Settings → Pages → Source: GitHub Actions** if not already enabled. The deployment workflow cannot enable Pages because GitHub blocks that permission for its workflow token.
- A successful build artifact is not a substitute for in-game compatibility testing.

## Backend

A dedicated Supabase project is deployed in TLAL, London (`eu-west-2`). The database schema, database-backed rate limiting and Edge Function are deployed; a basic rate-limit test passed. The website source is configured to call the verification endpoint. Project creation was approved at $0/month; future usage, add-ons, or plan changes could still incur costs, so monitor billing.

**Manual security step still required:** set `MATIONBUILD_ADMIN_TOKEN` and `ALLOWED_ORIGIN=https://solancegmd.github.io` in [Supabase Function secrets](https://supabase.com/dashboard/project/gfgmknlxzdfnjdpxetdz/functions/secrets). Key issuance and template moderation remain disabled until the admin token is configured. Follow [supabase/README.md](supabase/README.md). No service-role key or admin token is committed to this repository. The mod's access-key setting is stored in the local Geode settings file; use a private key and do not share it.

## Community templates and Antiprime

The project intends to support community-authored templates and, where permitted, compatibility with Antiprime AutoBuild templates. The native importer is not implemented yet. See [docs/community-templates.md](docs/community-templates.md). A permitted sample export and verified format are required before claiming compatibility.

MationBuild is an independent community project and is not affiliated with RobTop Games or Antiprime. Third-party assets remain their creators' property and must be used according to their terms.
