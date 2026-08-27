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
		-- Official ladder, straight from the client's own augment strings:
		--   no augment : attacks and spells are disabled while active
		--   bonus I    : "Attacks and spells are enabled but dealt damage is reduced by 50%."
		--   bonus II   : "and the damage dealt is no longer reduced."
		local grade = creature:upgradeSpellsWOD("Swift Foot")
		if grade == WHEEL_GRADE_NONE then
			local pacify = Condition(CONDITION_PACIFIED)
			pacify:setParameter(CONDITION_PARAM_TICKS, spellDuration)
			creature:addCondition(pacify)
		elseif grade == WHEEL_GRADE_REGULAR then
			local damageDebuff = Condition(CONDITION_ATTRIBUTES)
			damageDebuff:setParameter(CONDITION_PARAM_SUBID, SUBID_SWIFT_FOOT)
			damageDebuff:setParameter(CONDITION_PARAM_TICKS, spellDuration)
			damageDebuff:setParameter(CONDITION_PARAM_BUFF_DAMAGEDEALT, 50) -- deals 50% of normal damage
			creature:addCondition(damageDebuff)
		end
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
spell:cooldown(10 * 1000)
spell:groupCooldown(2 * 1000, 10 * 1000)
spell:level(55)
spell:mana(400)
spell:isSelfTarget(true)
spell:isAggressive(false)
spell:isPremium(true)
spell:needLearn(false)

spell:register()
