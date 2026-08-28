---
name: crystalserver-grants
description: "Give a character items, equipment sets, imbuements, Tibia Coins, gold, levels, magic level, skills or storage values on the running crystalserver. Use when the user asks to hand a character gear or a BiS set, add coins or money, set a level or skill, apply imbuements at a given tier, or place items in a depot, inbox, stash or backpack."
when_to_use: "Trigger for: give <character> <items>, add tibia coins, add gold, make him level N, set magic level, give a BiS set, imbue this weapon, put the items in the depot, send it to the inbox, max out skills, set a storage value."
---

# Granting Items and Stats

## Pick the route first

Start with the cheapest route that works. Do not reach for a script when a column
will do.

| What is being granted | Route |
|---|---|
| Level, coins, bank, cap, vocation, town | **SQL** — plain columns in `players` / `accounts` |
| Plain items with no attributes | **SQL** — an item row with an empty `attributes` blob |
| Imbued items, tiered items, charges | **SQL** — hand-author the blob, then validate it (see below) |
| Anything at all, target online | **Lua** — SQL writes get overwritten |

**Only one condition actually pushes you off SQL: the character is online.** The
server holds them in memory and rewrites their rows wholesale on logout/autosave,
discarding anything SQL changed. Check `players_online` first — always. If they
are online, either wait for logout or use Lua.

A serialized `attributes` blob is *not* a second condition. An earlier version of
this skill called the blob "unverifiable until the server boots" and sent you to
Lua — which nearly cost a pointless restart that would have kicked an online
player. That was wrong. The blob is small, fully specified below, and **the live
DB already holds server-written blobs you can validate your encoder against**, so
the SQL route is checkable before anything boots. See "Serialized attributes".

If SQL fits, use it — it needs no restart and no reload. Reach for a talkaction
only when the target is online, and say so when you do.

When you do write one, remember it costs a `/reload scripts` (god character
required) or a full restart to load. Raise that cost with the user **before**
starting, not after.

Before writing anything, check whether an existing GM command already covers it —
these are live and need no code (all `groupType("god")`, so a group-6 character
must type them; see `crystalserver-db`):

| Need | Command |
|---|---|
| Tibia Coins | `/addtc {PlayerName},{amount}` (`/addttc` for transferable) |
| Levels / magic / skills | `/addskill {PlayerName}, level\|magic\|<skill>, {amount}` — adds N, does not set N |
| Items into your own backpack | `/i {itemName or id},{count}` |
| Gold | `/addmoney` |
| Storage | `/setstorage`, `/manage_storage` |

Full list: `data/scripts/talkactions/god/`. Anything involving imbuements, a
specific target level, or delivery into another character's depot needs a script.

A complete, working example of that script lives at
`data/scripts/talkactions/god/druid_bis_kit.lua` — copy it as the starting point.

## Finding item ids

```bash
grep -in 'name="[^"]*sanguine' data/items/items.xml | head -20
```

Then read the item's full block to see what it actually supports:

```bash
awk '/<item id="43886" /{p=1} p{print} p&&/<\/item>/{exit}' data/items/items.xml
```

This data set is roughly Tibia 12.x–13.x. Many newer BiS items are absent from
`items.xml` (no Moonsilver anything, no Primal Armor/Legs, no Sanguine Tome).
**Always grep before promising an item**, and tell the user which requested items
are missing rather than substituting silently.

But say *"missing from the server"*, not *"does not exist"* — the client assets at
`~/Github/Tibia/CrystalOTC` are **15.30** and carry the newer items. Item ids are
one shared space (server `items.xml` id == client appearance id, verified), so the
client is the authority on whether an item is real and what its id is:

```bash
# data/things/1530/appearances-*.dat is protobuf; Appearance.id = field 1, .name = field 4
```

814 named client items are missing from `items.xml`; 127 of those are also absent
from the server's own `data/items/appearances.dat`, which is an older revision
(42108 objects vs the client's 43392) and would need refreshing first. The whole
Moonsilver/Flamingo set is ids 53197–53233. When a user asks for one of these,
the honest answer is that it can be added, not that it does not exist.

## Imbuements

### Which items can take which imbuement

An item can only carry a category its `<attribute key="imbuementslot">` block
declares, with a max tier per category:

```xml
<attribute key="imbuementslot" value="2">
    <attribute key="critical hit" value="3" />
    <attribute key="mana leech" value="3" />
</attribute>
```

`value="2"` is the number of slots; each child is a category and its max tier.
In practice **critical hit and mana/life leech are declared on weapons and helmets
only** — armors, legs, boots, shields, amulets and rings will not take them. Check
each item and report which ones could not be imbued instead of assuming.

Category ids (`data/XML/imbuements.xml` header, `src/items/items_definitions.hpp:263`):
`0` elemental damage, `1` life leech, `2` mana leech, `3` critical hit,
`4-9` elemental protection, `10` speed, `11-16,18` skillboosts, `17` capacity,
`19` paralysis removal.

Tiers: base `1` Basic, `2` Intricate, **`3` Powerful (highest)**.

### Imbuement ids

The id is the 1-based position of the `<imbuement>` node in
`data/XML/imbuements.xml` (`runningid` is pre-incremented,
`src/creatures/players/imbuements/imbuements.cpp:86`). Ids must stay ≤ 255 —
only the low byte is read back.

All three bases carry `duration="72000"`, so a fresh imbuement of any tier is
`72000 * 256 + id`.

Going the SQL route you have to read the ids straight out of the XML — count the
nodes, and sanity-check the pairing against a real blob already in the DB (see
"Serialized attributes"). In Lua, resolve at runtime by category and tier instead:

```lua
local function findImbuement(categoryId, baseId)
    for id = 1, 255 do
        local imbuement = Imbuement(id)
        if imbuement then
            local base, category = imbuement:getBase(), imbuement:getCategory()
            if base and category and base.id == baseId and category.id == categoryId then
                return { id = id, name = imbuement:getName(), duration = base.duration }
            end
        end
    end
end
```

`getBase()` returns `{id, name, price, removeCost, duration}`; `getCategory()`
returns `{id, name}`. Powerful duration is 72000 seconds.

### Applying one

The imbuement of slot *N* — 0-based, and must be `< item:getImbuementSlot()` — is
just a custom attribute keyed `500 + N` holding `(duration << 8) | imbuementId`
(`ITEM_IMBUEMENT_SLOT`, `src/items/item.cpp:141`). In SQL that is an entry in the
`ATTR_CUSTOM` blob with key `"500"`, `"501"`, … (see "Serialized attributes").

In Lua, `item:setImbuement()` is **not exposed**, so set the attribute directly —
this is byte-identical to what the C++ path writes:

```lua
-- slot is 0-based and must be < item:getImbuementSlot()
item:setCustomAttribute(500 + slot, imbuement.duration * 256 + imbuement.id)
```

Use `* 256` rather than `<< 8` so the script works regardless of Lua version.
One imbuement per category per item — the game rejects duplicates.

The alternative, `player:applyImbuementScrollToItem(scrollId, item)`, goes through
full validation but requires the player to be online with a client, the item in
their **inventory** (not a depot), and the matching scroll in their bag. The custom
attribute route has none of those constraints.

## Serialized attributes

The `attributes` column on `player_items`, `player_depotitems` and
`player_inboxitems` is a `PropStream` blob written by `Item::serializeAttr` and
read by `Item::unserializeAttr` (`src/items/item.cpp`). Custom attributes —
which is where imbuements live — encode as, all little-endian:

```
0x29                 ATTR_CUSTOM (41)
<uint64> count
  per entry:
    <uint16> keyLength
    key bytes (ASCII)
    0x02             value type (2 = int64; 1 string, 3 double, 4 bool)
    <int64> value
```

Nothing follows the last entry. Other attributes are their own tag bytes —
`ATTR_TIER` (40) is a single `uint8`, and a **stackable** item always carries
`0x0F` (`ATTR_COUNT`) plus a `uint8` count. Plain non-stackable items take `''`.

### Validating the blob before you write it

Never write a blob you have not checked against one the server produced. There are
~100 of them in `player_items` on this install:

```bash
docker exec crystalserver-db mariadb -uroot -proot crystalserver -e \
  "SELECT player_id, itemtype, HEX(attributes) FROM player_items WHERE LENGTH(attributes) > 0;"
```

Pick an imbued weapon, re-encode it with your own encoder, and diff the hex — it
must match byte for byte. Then decode your generated blobs back and assert the
cursor lands exactly on the end of the buffer. Two checks, no server boot needed.

Player 3's grand sanguine rod (43886) is a good reference row: two imbuements,
keys `"500"` and `"501"`.

### Verifying the result end-to-end

The login web service reads the account and player rows, so it confirms a
hand-built character loads without needing a game client:

```bash
curl -s -X POST http://127.0.0.1:8081/login -H 'Content-Type: application/json' \
  -d '{"type":"login","email":"@acc","password":"pw"}'
```

It returns the character list with level, vocation and premium status. It does
**not** touch `player_items`, so it validates the character, not the blobs — that
is what the byte-diff above is for.

## Where to deliver

### In SQL

`pid`/`sid` are all you need. `sid` is a unique per-character counter (the server
starts at 101); `pid` is either an **inventory slot** or the `sid` of the container
holding the item:

```
CONST_SLOT_  HEAD 1  NECKLACE 2  BACKPACK 3  ARMOR 4  RIGHT 5 (shield)
             LEFT 6 (weapon)  LEGS 7  FEET 8  RING 9  AMMO 10  STORE_INBOX 11
```

The store inbox (slot 11) is created automatically at load
(`iologindata_load_player.cpp:644`) — never insert it yourself. Load reads
`ORDER BY sid DESC` and resolves parents through a map, so children only need a
`pid` matching their container's `sid`.

**Charged items must not go in a worn slot.** `Player::onEquipInventory`
(`src/creatures/players/player.cpp:11760`) fires equip moveevents on *every*
login, so a `transformEquipTo` ring or amulet sitting in slot 9 transforms and
starts burning its timer the moment the character logs in, even idle. Put it in
the backpack instead and say so.

### In Lua

| Destination | Lua |
|---|---|
| Depot box (1–20, shared across towns) | `player:getDepotChest(1, true)` then `chest:addItem(id, 1)` |
| Backpack / inventory | `player:addItem(itemId, count, canDropOnMap, subType, slot, tier)` |
| Existing item object | `player:addItemEx(item)` |
| Inbox | `player:getInbox()` |
| Supply stash | `player:addItemStash(itemId, count)` |

**Depot gotcha:** `savePlayerDepotItems` is skipped entirely unless
`lastDepotId != -1` (`src/io/functions/iologindata_save_player.cpp:555`).
`player:getDepotChest(...)` sets it, so always fetch the chest through that call —
never construct one another way, or the items vanish on logout.

A depot box holds 32 slots per page, 2000 items total.

Items with `transformEquipTo` (charged amulets, rings) exist in two forms. Grant the
**de-equipped** id — the one carrying `stopduration="1"` — so the timer stays frozen
in storage.

## Stats

### In SQL

Offline, every stat is a plain column and is loaded verbatim — `maglevel` with
`manaspent = 0`, and `skill_<name>` with `skill_<name>_tries = 0`, both land
exactly on the value you write. Only the level needs derived values:

- `experience` = `(((L-6)*L + 17)*L - 12) / 6 * 100`. Level 1000 = 16566949800.
- Baselines at level 8 are health 185, mana 90, cap 470. Add the vocation's
  `gainhp`/`gainmana`/`gaincap` (`data/XML/vocations.xml`) for every level above 8.
  Elite Knight gains 15/5/25, so level 1000 = 15065 HP / 5050 mana / 25270 cap.
- `players.cap` is stored in **oz** — the loader multiplies it by 100
  (`iologindata_load_player.cpp:170`). Write 25270, not 2527000.
- `soul` caps at the vocation's `soulmax` (200 for a knight).

Cross-check any formula against an existing high-level row before trusting it;
`GOD` (level 1000, vocation 0) is the reference on this install.

### In Lua

```lua
-- Level: go through addExperience, not setLevel.
-- setLevel writes level + experience but leaves HP/mana/cap at the old values.
local required = Game.getExperienceForLevel(1000)
local current = target:getExperience()
if required > current then
    target:addExperience(required - current, false)
elseif required < current then
    target:removeExperience(current - required, false)
end
```

Passing `nil` as the source means `Player:onGainExperience`
(`data/events/scripts/player.lua:485`) returns early, so no stage or boost
multiplier is applied and the character lands exactly on the target level.

```lua
target:addTibiaCoins(1000000)        -- saves the account immediately
target:addTransferableCoins(n)
target:setBankBalance(n)
target:addManaSpent(n)               -- magic level, via vocation:getRequiredManaSpent
target:addSkillTries(SKILL_SHIELD, n, true)
target:setStorageValue(key, value)
target:save()
```

## Talkaction template

```lua
local grant = TalkAction("/mycommand")

function grant.onSay(player, words, param)
    logCommand(player, words, param)

    -- Gate here, not with groupType: the sample characters are group_id 1 and
    -- would be blocked by groupType("god"). The @god account is type 5, so
    -- comparing against ACCOUNT_TYPE_GOD (6) would reject it.
    if player:getAccountType() < ACCOUNT_TYPE_GAMEMASTER then
        player:sendCancelMessage("Only a staff account may use this command.")
        return true
    end

    local target = player
    local name = param:trim()
    if name ~= "" then
        -- Player() is online-only; Game.getOfflinePlayer loads from the DB.
        target = Player(name) or Game.getOfflinePlayer(name)
        if not target then
            player:sendCancelMessage(string.format("No such character: %s.", name))
            return true
        end
    end

    -- ... work here ...

    target:save()
    return true
end

grant:separator(" ")
grant:setDescription("[Usage]: /mycommand [playerName]")
grant:groupType("normal")
grant:register()
```

Talkaction gating is `player->getGroup()->id < talkAction->getGroupType()`
(`src/lua/creature/talkaction.cpp:54`). `groupType("normal")` plus an explicit
account-type check is what lets the character the user actually plays run it.

`Game.getOfflinePlayer(name or id)` returns a full `Player` loaded from the DB
with `isOffline()` set, and `player:save()` handles that case explicitly
(`player_functions.cpp`, `luaPlayerSave`). `Migration:forEachPlayer`
(`data/scripts/lib/register_migrations.lua:20`) is the proof it works. So Lua is
not restricted to online targets — but if the target is offline, ask why you are
writing Lua at all instead of SQL.

Put the file under `data/scripts/talkactions/god/` (folder name is organisational
only, it gates nothing). Syntax-check before loading:

```bash
luac -p data/scripts/talkactions/god/mycommand.lua
```

## Loading it

Only the Lua route needs this. SQL needs no reload and no restart — a new or
offline character is read fresh at login.

`/reload scripts` picks up new talkactions — but it is itself `groupType("god")`,
so it must be typed by `GOD` or an `ADM*` character. If the target character is a
sample character, that means either two clients or a restart. See
`crystalserver-ops` for the restart procedure, and always confirm with the user
before restarting a running server.

After the grant is done, offer to delete the one-off script.

## Reporting back

State plainly which items were created, which imbuements went on which item, and
— importantly — which requested items do not exist in this data set and which
could not take the requested imbuement. Do not quietly substitute.

## Related

- `crystalserver-ops` — restart/reload procedure.
- `crystalserver-db` — checking what a character currently has, and who is online.
