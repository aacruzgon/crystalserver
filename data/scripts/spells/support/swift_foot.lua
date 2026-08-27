local spellDuration = 10000
-- Own subid so toggling another CONDITION_ATTRIBUTES effect off cannot clear this debuff.
local SUBID_SWIFT_FOOT = 91

local combat = Combat()
combat:setParameter(COMBAT_PARAM_EFFECT, CONST_ME_MAGIC_GREEN)
combat:setParameter(COMBAT_PARAM_AGGRESSIVE, 0)

local condition = Condition(CONDITION_HASTE)
condition:setParameter(CONDITION_PARAM_TICKS, spellDuration)
condition:setFormula(1.8, 40, 1.8, 40)
combat:addCondition(condition)

local spell = Spell("instant")

function spell.onCastSpell(creature, var)
	local summons = creature:getSummons()
	if summons and type(summons) == "table" and #summons > 0 then
		for i = 1, #summons do
			local summon = summons[i]
			local summon_t = summon:getType()
			if summon_t and summon_t:familiar() then
				local deltaSpeed = math.max(creature:getBaseSpeed() - summon:getBaseSpeed(), 0)
				local FamiliarSpeed = ((summon:getBaseSpeed() + deltaSpeed) * 0.8) - 72
				local FamiliarHaste = Condition(CONDITION_HASTE)
				FamiliarHaste:setParameter(CONDITION_PARAM_TICKS, spellDuration)
				FamiliarHaste:setParameter(CONDITION_PARAM_SPEED, FamiliarSpeed)
				summon:addCondition(FamiliarHaste)
			end
		end
	end

	if combat:execute(creature, var) then
		-- 15.25 Vocation Adjustments: "Swift Foot now allows attacking and casting while being active,
		-- but damage dealt is reduced by 30%." Before, attacks were disabled outright and only the
		-- wheel augment lifted that; the augment is gone (the slot now carries Divine Barrage), so this
		-- is unconditional base behaviour.
		local damageDebuff = Condition(CONDITION_ATTRIBUTES)
		damageDebuff:setParameter(CONDITION_PARAM_SUBID, SUBID_SWIFT_FOOT)
		damageDebuff:setParameter(CONDITION_PARAM_TICKS, spellDuration)
		damageDebuff:setParameter(CONDITION_PARAM_BUFF_DAMAGEDEALT, 70) -- deals 70% of normal damage
		creature:addCondition(damageDebuff)
		return true
	end

	return false
end

spell:name("Swift Foot")
spell:words("utamo tempo san")
spell:group("support", "focus")
spell:vocation("paladin;true", "royal paladin;true")
spell:castSound(SOUND_EFFECT_TYPE_SPELL_SWIFT_FOOT)
spell:id(134)
spell:cooldown(4 * 1000) -- Vocation Adjustment: 10s -> 4s
spell:groupCooldown(2 * 1000, 2 * 1000) -- Vocation Adjustment: secondary 10s -> 2s
spell:level(55)
spell:mana(400)
spell:isSelfTarget(true)
spell:isAggressive(false)
spell:isPremium(true)
spell:needLearn(false)

spell:register()
