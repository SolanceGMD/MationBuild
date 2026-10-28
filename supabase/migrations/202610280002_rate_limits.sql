-- MationBuild request throttling and private moderation support.
create table if not exists public.mationbuild_rate_limits (
  identifier_hash text not null check (identifier_hash ~ '^[a-f0-9]{64}$'),
  action text not null check (action in ('verify','submit-template','issue','moderate','catalog')),
  window_start timestamptz not null,
  request_count integer not null default 0 check (request_count >= 0),
  primary key (identifier_hash, action, window_start)
);
alter table public.mationbuild_rate_limits enable row level security;
revoke all on public.mationbuild_rate_limits from anon, authenticated;
revoke all on public.mationbuild_rate_limits from public;

create or replace function public.mationbuild_rate_limit(
  p_identifier_hash text,
  p_action text,
  p_limit integer,
  p_window_seconds integer
) returns boolean
language plpgsql
security definer
set search_path = ''
as $$
declare
  v_window timestamptz;
  v_count integer;
begin
  if p_identifier_hash !~ '^[a-f0-9]{64}$'
     or p_action not in ('verify','submit-template','issue','moderate','catalog')
     or p_limit < 1 or p_limit > 1000
     or p_window_seconds < 1 or p_window_seconds > 86400 then
    raise exception 'invalid rate limit arguments';
  end if;
  v_window := to_timestamp(floor(extract(epoch from clock_timestamp()) / p_window_seconds) * p_window_seconds);
  insert into public.mationbuild_rate_limits(identifier_hash, action, window_start, request_count)
  values (p_identifier_hash, p_action, v_window, 1)
  on conflict (identifier_hash, action, window_start)
  do update set request_count = public.mationbuild_rate_limits.request_count + 1
  returning request_count into v_count;
  return v_count <= p_limit;
end;
$$;
revoke all on function public.mationbuild_rate_limit(text,text,integer,integer) from public, anon, authenticated;
grant execute on function public.mationbuild_rate_limit(text,text,integer,integer) to service_role;

create index if not exists mationbuild_keys_active_idx on public.mationbuild_keys (key_hash) where revoked_at is null;
create index if not exists mationbuild_rate_limits_cleanup_idx on public.mationbuild_rate_limits (window_start);