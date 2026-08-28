---
name: crystalserver-db
description: "Query and edit the crystalserver MariaDB database. Use when the user asks what is stored for a character or account — level, vocation, coins, group, town, depot, storage, bans, houses, guilds — or wants a value read or changed directly in SQL, or wants a database backup."
when_to_use: "Trigger for: check the database, query the DB, what level is <character>, look up an account, how many coins does X have, is <character> online, run this SQL, edit the players table, dump/backup the database."
---

# Crystalserver Database

## Connecting

There is **no mysql/mariadb client installed on the host**. The database runs in a
Docker container; go through `docker exec`:

```bash
docker exec crystalserver-db mariadb -uroot -proot crystalserver -e "SELECT ..."
```

Credentials come from `config.lua:509-514` (`127.0.0.1:3307`, `root`/`root`,
database `crystalserver`). Re-read those lines rather than trusting this file if a
connection is refused. `docker ps` also shows a `crystalserver-login` container
(the login server) — it holds no game data.

Schema of record: `schema.sql`. Migrations live in `data/migrations/`.

## The one rule that matters

**Never write to the rows of a player who is online.** The server holds the whole
character in memory and writes it back wholesale on autosave (hourly),
`/save`, logout and shutdown — silently overwriting anything SQL changed in the
meantime. `player_items`, `player_depotitems`, `player_storage` and friends are
`DELETE` + re-`INSERT` on every save (`src/io/functions/iologindata_save_player.cpp`),
so a hand-inserted row does not survive.

Check first, every time:

```bash
docker exec crystalserver-db mariadb -uroot -proot crystalserver -e \
  "SELECT p.name FROM players_online o JOIN players p ON p.id=o.player_id;"
```

If the target is online, either have them log out first, or — better for anything
the game models — do it through Lua instead. See `crystalserver-grants`.

Reads are always safe; prefer them for answering questions.

Item rows (`player_items`, `player_depotitems`, `player_inboxitems`) store an
`attributes` column that is a **serialized binary blob** (imbuements, tier, charges,
custom attributes). It *can* be hand-authored in SQL — the format is short and the
table already holds server-written blobs to validate an encoder against — but only
if you actually run that validation. `crystalserver-grants` has the byte layout and
the check. Never write one you have not byte-diffed against a real row.

## Table map

- **Identity** — `accounts`, `players`, `players_online`, `player_namelocks`, `player_oldnames`
- **Items** — `player_items` (inventory + backpacks), `player_depotitems`, `player_inboxitems`, `player_stash`, `player_rewards`
- **Progress** — `player_storage`, `player_spells`, `player_outfits`, `player_mounts`, `player_charms`, `player_bosstiary`, `player_prey`, `player_taskhunt`, `player_wheeldata`, `player_weekly_tasks`, `player_bounty_tasks`
- **Social** — `guilds`, `guild_membership`, `guild_ranks`, `guild_invites`, `guild_wars`, `guildwar_kills`, `account_viplist`, `account_vipgroups`, `account_vipgrouplist`
- **World** — `houses`, `house_lists`, `towns`, `worlds`, `tile_store`, `global_storage`, `kv_store`, `server_config`
- **Economy/logs** — `coins_transactions`, `store_history`, `market_offers`, `market_history`, `forge_history`, `daily_reward_history`, `player_deaths`, `player_kills`, `player_statements`
- **Moderation** — `account_bans`, `account_ban_history`, `ip_bans`, `account_sessions`

## Known facts about this install

Verified against the live DB — re-check rather than assume if something looks off.

- Account `1` is `god` / descriptor `@god`, password `god`, and its
  `accounts.type` is **5** (`ACCOUNT_TYPE_COMMUNITYMANAGER`). `ACCOUNT_TYPE_GOD`
  is **6** (`src/enums/account_type.hpp`), so a Lua or SQL check written as
  `= ACCOUNT_TYPE_GOD` will not match this account. Gate on `>= ACCOUNT_TYPE_GAMEMASTER`.
- The `* Sample` characters (`Rook`, `Sorcerer`, `Druid`, `Paladin`, `Knight`,
  `Monk Sample`) all live on account 1 with `group_id = 1`, i.e. ordinary players.
  They cannot run any `groupType("god")` talkaction.
- The group-6 characters on account 1 are `GOD` (players.id 7) and `ADM2`–`ADM9`.
  Log in as one of those to use `/reload`, `/i`, `/addtc`, `/addskill`, etc.
- Accounts `101`+ (`test1`…) are load/test accounts holding `Druid N` /
  `Druid Noob N` characters.
- **Account descriptors are `name = 'foo'`, `email = '@foo'`** — every account on
  this install follows it. A modern client logs in with `accounts.email`, an old
  protocol (version 1100) client with `accounts.name`
  (`src/account/account_repository_db.cpp:40`). The server itself does not care:
  `ProtocolLogin` only rejects an empty string (`protocollogin.cpp:179`) and the
  lookup is plain equality, so a descriptor with no `@` authenticates fine
  server-side but the client's email field will not accept it. Set both columns
  when creating an account, and hand back the `@` form.
- `accounts.password` is **sha1** of the plaintext (`config.lua` `passwordType`).
- Premium is driven by `accounts.lastday` being a **future unix timestamp**, not
  by `premdays` — `Account::updatePremiumTime` recomputes `premdays` from it and
  zeroes it when `lastday < now`. The column is `int unsigned`, so anything past
  ~2106 silently overflows; 18250 days (50 years) is safe.

## Useful queries

```sql
-- character overview
SELECT p.id, p.name, p.group_id, p.level, p.vocation, p.town_id,
       a.id AS account, a.name, a.type, a.coins, a.coins_transferable
FROM players p JOIN accounts a ON a.id = p.account_id
WHERE p.name = 'Druid Sample';

-- who is online right now
SELECT p.name FROM players_online o JOIN players p ON p.id = o.player_id;

-- what is in a character's depot boxes (pid = depot box number)
SELECT pid, itemtype, count FROM player_depotitems
WHERE player_id = 3 ORDER BY pid, sid;

-- a character's storage keys
SELECT key, value FROM player_storage WHERE player_id = 3;
```

`vocation` ids: 0 none, 1 sorcerer, 2 druid, 3 paladin, 4 knight, 5 master
sorcerer, 6 elder druid, 7 royal paladin, 8 elite knight, 9 monk.

## Backups

```bash
docker exec crystalserver-db mariadb-dump -uroot -proot crystalserver > backup.sql
```

Take one before any multi-row write. `database_backup/` in the repo holds prior
snapshots and pre-migration copies.

## Related

- `crystalserver-ops` — starting, stopping and reloading the server.
- `crystalserver-grants` — giving a character items or stats, in SQL or Lua.
