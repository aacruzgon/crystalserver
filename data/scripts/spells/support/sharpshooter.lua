-- Base gives +40% distance skill. The wheel's bonus II reads
-- "-6s Cooldown; distance skill bonus increased by +5%", i.e. +45% in total.
local BASE_DISTANCE_PERCENT = 140

local function buildCombat(distancePercent)
	local condition = Condition(CONDITION_ATTRIBUTES)
	condition:setParameter(CONDITION_PARAM_SUBID, AttrSubId_Sharpshooter)
	condition:setParameter(CONDITION_PARAM_TICKS, -1)
	condition:setParameter(CONDITION_PARAM_SKILL_DISTANCEPERCENT, distancePercent)
	condition:setParameter(CONDITION_PARAM_BUFF_SPELL, true)

	local combat = Combat()
	combat:setParameter(COMBAT_PARAM_EFFECT, 5)
	combat:setParameter(COMBAT_PARAM_AGGRESSIVE, false)
	combat:addCondition(condition)
	return combat
end

local combat = buildCombat(BASE_DISTANCE_PERCENT)
local combatAugmented = buildCombat(BASE_DISTANCE_PERCENT + 5)

local spell = Spell("instant")

function spell.onCastSpell(creature, variant)
	local player = creature:getPlayer()
	if player and player:getStance() == STANCE_SHARPSHOOTER then
		player:removeCondition(CONDITION_ATTRIBUTES, CONDITIONID_COMBAT, AttrSubId_Sharpshooter)
		player:setStance(STANCE_NONE)
		player:getPosition():sendMagicEffect(CONST_ME_POFF)
		return true
	end
	if player then
		player:setStance(STANCE_SHARPSHOOTER)
		-- Wheel bonus II raises the distance skill bonus. Bonus I lifts the stance's block on
		-- support and healing spells, which is enforced in Spell::playerSpellCheck.
		if player:getWheelSpellSkillIncrease("Sharpshooter") > 0 then
			return combatAugmented:execute(creature, variant)
		end
	end
	return combat:execute(creature, variant)
end

spell:name("Sharpshooter")
spell:words("utori con")
spell:group("support", "stance")
spell:vocation("paladin;true", "royal paladin;true")
spell:castSound(SOUND_EFFECT_TYPE_SPELL_SHARPSHOOTER)
spell:id(313)
spell:cooldown(10 * 1000)
spell:groupCooldown(2 * 1000, 10 * 1000)
spell:level(60)
spell:mana(450)
spell:needLearn(false)
spell:isSelfTarget(true)
spell:isAggressive(false)
spell:isPremium(false)

spell:register()
