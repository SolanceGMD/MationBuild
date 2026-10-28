# MationBuild licensing and template API

This folder contains a Supabase Edge Function and SQL migration. It is not deployed until a Supabase project is linked and the migration/function are applied.

## Deploy
1. Create a Supabase project and save its project ref.
2. Apply `migrations/202610280001_mationbuild.sql` in the SQL editor or with Supabase CLI.
3. Set function secrets: `MATIONBUILD_ADMIN_TOKEN` to a long random secret and `ALLOWED_ORIGIN` to the actual GitHub Pages origin.
4. Deploy `functions/mationbuild-api`.
5. Set `verifyEndpoint` in `site/app.js` to `https://PROJECT_REF.supabase.co/functions/v1/mationbuild-api`.
6. Call the `issue` action from a trusted server/admin tool with the `x-admin-token` header. Never place this token in browser code or Git.
7. Add request throttling/bot protection before public launch.

## API actions
- `POST { "action": "verify", "key": "MB-..." }`
- `POST { "action": "issue", "label": "Creator", "days": 365 }` with the admin header
- `GET /templates` lists reviewed/published templates
- `POST { "action": "submit-template", ...metadata }` creates a pending review entry

Template metadata is not the template payload itself. Only publish files whose provenance, license, format and contents have been reviewed. Community submissions are untrusted; do not auto-load arbitrary files as executable code.

## Production hardening
This is a starter backend, not a production-ready paid licensing service: add IP-based rate limiting/bot protection, audit logs, activation/device binding if needed, admin authentication, and a moderation interface before issuing keys publicly. Never expose the service-role key. Rotate the admin token if exposed.