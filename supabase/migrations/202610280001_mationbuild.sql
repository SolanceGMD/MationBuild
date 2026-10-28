create extension if not exists pgcrypto;
create table if not exists public.mationbuild_keys (
 id uuid primary key default gen_random_uuid(),
 key_hash text not null unique,
 label text not null default 'Community access',
 created_at timestamptz not null default now(),
 expires_at timestamptz,
 revoked_at timestamptz,
 max_activations integer not null default 1 check (max_activations between 1 and 100),
 activation_count integer not null default 0 check (activation_count >= 0),
 last_used_at timestamptz
);
create table if not exists public.mationbuild_templates (
 id uuid primary key default gen_random_uuid(),
 slug text not null unique,
 title text not null check (length(title) between 3 and 100),
 description text not null default '' check (length(description) <= 1000),
 author_name text not null check (length(author_name) between 1 and 80),
 source_url text not null check (source_url like 'https://%'),
 download_url text not null check (download_url like 'https://%'),
 game_version text,
 schema_version integer not null default 1,
 tags text[] not null default '{}',
 status text not null default 'pending' check (status in ('pending','published','rejected','removed')),
 created_at timestamptz not null default now(),
 updated_at timestamptz not null default now()
);
create index if not exists mationbuild_templates_public_idx on public.mationbuild_templates (status, created_at desc);
alter table public.mationbuild_keys enable row level security;
alter table public.mationbuild_templates enable row level security;
revoke all on public.mationbuild_keys from anon, authenticated;
revoke all on public.mationbuild_templates from anon, authenticated;
grant select on public.mationbuild_templates to anon, authenticated;
drop policy if exists "Published templates are readable" on public.mationbuild_templates;
create policy "Published templates are readable" on public.mationbuild_templates for select to anon, authenticated using (status = 'published');
comment on table public.mationbuild_keys is 'Hashed license keys only. Never store raw license keys.';
comment on table public.mationbuild_templates is 'Community template catalog. Publish only templates reviewed for provenance and safety.'