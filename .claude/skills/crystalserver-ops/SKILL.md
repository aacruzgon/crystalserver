---
name: crystalserver-ops
description: "Start, stop, restart, reload and rebuild the crystalserver OT server in this repo. Use when the user asks to start or restart the server, check whether it is running, read or tail the server log, hot-reload scripts/items/config/imbuements at runtime, or recompile after a C++ change."
when_to_use: "Trigger for: start the server, restart the server, is the server up, bring it back up, server log, tail the log, check for startup errors, /reload, reload scripts, reload items, recompile, rebuild the binary, the server won't boot."
---

# Crystalserver Ops

Operating the local crystalserver instance. All commands assume cwd is the repo
root — the binary reads `config.lua` from the working directory, so never `cd`
elsewhere to launch it.

## Layout

| Thing | Path |
|---|---|
| Binary (macOS) | `./build/macos-release/bin/crystalserver` |
| Config | `./config.lua` (template: `config.lua.dist`) |
| Core scripts/data | `data/` (`coreDirectory`) |
| Active data pack | `data-global/` (`dataPackDirectory`) — the only datapack in the repo |
| Item definitions | `data/items/items.xml` |
| Imbuement definitions | `data/XML/imbuements.xml` |
| Game port | 7182 · Login/status port 7181 |

`dataPackDirectory` and `coreDirectory` are set at `config.lua:5` and `config.lua:7`.
Confirm them before assuming which folder a script is loaded from.

## Is it running?

```bash
pgrep -fl "bin/crystalserver"
lsof -nP -iTCP -sTCP:LISTEN | grep -E "7181|7182"
```

The server is fully up when the log contains `World [1 - Crystal - Open PvP] on port [7182] is online!`.
Startup takes roughly 5 seconds; NPC spawn lines are the tail end of boot.

## Start

Launch in the background with `run_in_background: true` and redirect to a log file
in the scratchpad — the binary logs to stdout only:

```bash
./build/macos-release/bin/crystalserver > <scratchpad>/server.log 2>&1
```

Then poll the log until the "is online!" line appears, and grep it for
`[error]` / `[warn]`. A Lua syntax error in `data/scripts/**` shows up there as an
error line but does **not** stop the boot — zero error lines is the signal that
every script registered.

## Stop

`SIGTERM` (or `SIGINT`) sets `GAME_STATE_SHUTDOWN`, which kicks and saves every
online player, saves the world, then exits — typically under 5 seconds.

```bash
kill -TERM <pid>
while kill -0 <pid> 2>/dev/null; do sleep 1; done
```

Confirm `Server saved in ...` and `Done!` in the log before relaunching. Never
`kill -9` a running server: that skips the save and loses everything since the
last autosave (`saveIntervalTime = 1` hour, `config.lua:387-390`).

Other signals (`src/server/signals.cpp`):

- `SIGUSR1` — save the game state without stopping.
- `SIGHUP` — reload config.lua, raids, items, mounts, events, chat channels, core.lua.

## Hot reload

`allowReload = true` (`config.lua:375`), so `/reload <type>` works in-game. It is
gated `groupType("god")`, so it must be typed by a group-6 character — see
`crystalserver-db` for which characters those are.

`/reload scripts` clears all scripts and reloads `data-global/scripts` and
`data/scripts`, then monsters, NPCs and items (`src/game/functions/game_reload.cpp:193`).
That covers new or edited talkactions, actions, movements and creature events.

It does **not** cover everything. Use the specific type instead:

| Change | Reload type |
|---|---|
| `data/scripts/**`, `data-global/scripts/**` | `scripts` |
| `config.lua` | `config` |
| `data/XML/imbuements.xml` | `imbuements` |
| `data/items/items.xml` | `items` |
| `data/global.lua`, `data/stages.lua`, `data/libs/**` | `core` |
| `data/events/**` | `events` |
| everything | `all` |

Full type list: `data/scripts/talkactions/god/reload.lua:1-37`.

A restart is still required for anything compiled (C++), and for `config.lua`
values that are only read at boot.

## Rebuild

```bash
./recompile.sh "$HOME" macos-release
```

Presets in `CMakePresets.json`: `macos-release`, `macos-debug` (plus linux/windows
variants). Stop the server before overwriting the binary it is executing.

## Related

- `crystalserver-db` — reading and editing the MariaDB behind the server.
- `crystalserver-grants` — giving a character items, imbuements, coins or levels.
