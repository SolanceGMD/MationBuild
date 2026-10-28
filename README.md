# MationBuild

**MationBuild by Solance** is a Geometry Dash editor companion for repeatable block-decoration frames and portable community template packs.

## Current release: v0.2.0

### Features
- **Layered decoration presets** — Tech, Minimal, and Geometric panel-frame patterns built from native decorative objects rather than tiny symbol clusters.
- **Access-key verification** — editor tools verify keys through the MationBuild API.
- **Portable template import** — validated `mationbuild-json` v1 files for Geometry Dash 2.2081.
- **Undo-friendly placement** — generated objects use the editor paste path; use Undo to revert a pass.
- **Automated Windows builds** — GitHub Actions builds the Geode package and uploads it as an artifact.

### Compatibility status
MationBuild's portable JSON format is supported. **Native Antiprime AutoBuild template import is not implemented** because a verified, authorized export and its exact format are not available yet. Do not treat the portable JSON importer as an Antiprime importer.

## Install the Windows build

1. Open [GitHub Actions](https://github.com/SolanceGMD/MationBuild/actions).
2. Open the latest successful **Build MationBuild Geode mod** run and download **MationBuild-Windows**.
3. Extract the ZIP and install the contained `.geode` file using Geode.
4. Enter your MationBuild access key in Geode's MationBuild settings.
5. Open a level editor, select the block/anchor objects you want to decorate, and choose **Auto**. Set the style in mod settings.
6. To apply a portable pack, select anchor objects, choose **Import**, and select a MationBuild JSON v1 file.

**Important:** build success confirms compilation and packaging, not full in-game compatibility. Back up important levels and test in a disposable level before using the mod in a production project.

## Repository layout

- `src/main.cpp`, `mod.json`, `CMakeLists.txt` — Geode editor tools and portable importer.
- `site/` — access portal and community catalog.
- `.github/workflows/geode-build.yml` — Windows build and artifact workflow.
- `.github/workflows/ci.yml` — site and template validation.
- `supabase/` — database and Edge Function source.
- `docs/community-templates.md` — format, limits, and compatibility notes.
- `templates/` — sample portable packs.

## Website and backend setup

- Website: [MationBuild portal](https://solancegmd.github.io/MationBuild/)
- Build history: [GitHub Actions](https://github.com/SolanceGMD/MationBuild/actions)
- Enable GitHub Pages in **Settings → Pages → Source: GitHub Actions** if it is not already enabled.
- The Supabase function requires the project secrets `MATIONBUILD_ADMIN_TOKEN` and `ALLOWED_ORIGIN=https://solancegmd.github.io` for admin key issuance and moderation. See [supabase/README.md](supabase/README.md). Never commit the admin token or service-role key.

## Release checklist

Before distributing a public release:
- Confirm the latest Windows build and CI workflows pass.
- Install the artifact in Geometry Dash 2.2081 with the target Geode version.
- Verify Auto, each style, key rejection/acceptance, import validation, and Undo in-game.
- Verify the website and backend admin secrets are configured.
- Publish release notes and only claim compatibility that has been tested.

MationBuild is an independent community project and is not affiliated with RobTop Games or Antiprime. Third-party assets and templates remain their creators' property.
