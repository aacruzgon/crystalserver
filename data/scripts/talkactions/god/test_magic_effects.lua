-- Drives Game.sendMagicEffects, which packs several effects into one 0x83 using the
-- client's delta and delay entries. Nothing else in the server emits those two, so this
-- is what exercises them.
--
--   /testeffects            three effects on a diagonal, the last two staggered
--   /testeffects 3,4,250    a rows x cols block, each tile <delay> ms after the previous
local testEffects = TalkAction("/testeffects")

local DEFAULT_EFFECT = CONST_ME_FIREAREA

function testEffects.onSay(player, words, param)
	logCommand(player, words, param)

	local origin = player:getPosition()
	local effects = {}

	local split = param:split(",")
	local rows = tonumber(split[1])

	if rows then
		-- The deltas only ever add, and the client reads them as (total % 18, total / 18)
		-- tiles from the packet's own position, so the block has to grow south-east and stay
		-- inside 18 columns.
		local cols = tonumber(split[2]) or rows
		local step = tonumber(split[3]) or 0

		cols = math.min(cols, 18)

		for row = 0, rows - 1 do
			for col = 0, cols - 1 do
				effects[#effects + 1] = {
					position = Position(origin.x + col, origin.y + row, origin.z),
					type = DEFAULT_EFFECT,
					delay = (row * cols + col) * step
				}
			end
		end
	else
		effects = {
			{ position = origin, type = DEFAULT_EFFECT, delay = 0 },
			{ position = Position(origin.x + 1, origin.y + 1, origin.z), type = DEFAULT_EFFECT, delay = 300 },
			{ position = Position(origin.x + 2, origin.y + 2, origin.z), type = DEFAULT_EFFECT, delay = 600 }
		}
	end

	if not Game.sendMagicEffects(effects, player) then
		player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "No effects were sent - check the entries.")
		return true
	end

	player:sendTextMessage(MESSAGE_EVENT_ADVANCE, string.format("Sent %d effects in one packet.", #effects))
	return true
end

testEffects:separator(" ")
testEffects:groupType("god")
testEffects:register()
