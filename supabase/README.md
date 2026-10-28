# MationBuild licensing and template API

The dedicated Supabase project has been created in the TLAL organization on the free plan.

- Project ref: `gfgmknlxzdfnjdpxetdz`
- Region: London (`eu-west-2`)
- Function URL: `https://gfgmknlxzdfnjdpxetdz.supabase.co/functions/v1/mationbuild-api`
- Project creation quote approved: $0/month. This does not guarantee future usage/add-on costs remain zero.

The initial database migration has been applied, and the `mationbuild-api` Edge Function is deployed. The public access portal points to the verification endpoint. No service-role key or admin token is committed to this repository.

## Required secret setup before issuing keys

The admin-only `issue` action remains disabled until `MATIONBUILD_ADMIN_TOKEN` is set.

1. Open the [Supabase Dashboard](https://supabase.com/dashboard/project/gfgmknlxzdfnjdpxetdz/functions/secrets).
2. Add `MATIONBUILD_ADMIN_TOKEN` with a long, randomly generated secret value.
3. Add `ALLOWED_ORIGIN` with `https://solancegmd.github.io` (the origin, not a repository path).
4. Do not paste these values into GitHub files, browser code, chat, or public issues. Store the admin token in a password manager and send key-issuance requests only from a trusted admin environment.
5. Confirm the function can read the secrets before issuing any real keys.

Supabase provides its project URL and service-role credentials to Edge Functions as server-side environment variables. Never expose the service-role key in a public client.

## API actions

- `POST { "action": "verify", "key": "MB-..." }`
- `POST { "action": "issue", "label": "Creator", "days": 365 }` with the `x-admin-token` header
- `GET /templates` lists reviewed/published templates
- `POST { "action": "submit-template", ...metadata }` creates a pending review entry

Template metadata is not the template payload itself. Only publish files whose provenance, license, format and contents have been reviewed. Community submissions are untrusted; do not auto-load arbitrary files as executable code.

## Production hardening still required

This is a starter backend, not a production-ready paid licensing service. Add request throttling/bot protection, audit logs, activation/device binding if needed, admin authentication, and a moderation interface before issuing keys publicly. The public template-submission endpoint currently needs abuse protection. Review Supabase security advisors periodically and monitor usage/billing before public launch.