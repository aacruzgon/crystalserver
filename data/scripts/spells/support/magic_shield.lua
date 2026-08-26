local combat = Combat()
combat:setParameter(COMBAT_PARAM_EFFECT, CONST_ME_MAGIC_BLUE)
combat:setParameter(COMBAT_PARAM_AGGRESSIVE, 0)

local spell = Spell("instant")

function spell.onCastSpell(creature, var)
	local player = creature:getPlayer()
	if not player then
		return combat:execute(creature, var)
	end

	local condition = Condition(CONDITION_MANASHIELD)

	if not isRetroPVP() then
		local grade = player:upgradeSpellsWOD("Magic Shield")
		local shield = player:getMagicShieldCapacity(grade >= WHEEL_GRADE_REGULAR)
		condition:setParameter(CONDITION_PARAM_TICKS, 180000)
		condition:setParameter(CONDITION_PARAM_MANASHIELD, shield)
	else
		local level = player:getLevel()
		if level < 275 then
			condition:setParameter(CONDITION_PARAM_TICKS, 200000)
		else
			local grade = player:upgradeSpellsWOD("Magic Shield")
			local shield = player:getMagicShieldCapacity(grade >= WHEEL_GRADE_REGULAR)
			condition:setParameter(CONDITION_PARAM_TICKS, 180000)
			condition:setParameter(CONDITION_PARAM_MANASHIELD, shield)
		end
	end

	creature:addCondition(condition)
	return combat:execute(creature, var)
end

spell:name("Magic Shield")
spell:words("utamo vita")
spell:group("support")
spell:vocation("druid;true", "elder druid;true", "sorcerer;true", "master sorcerer;true")
spell:castSound(SOUND_EFFECT_TYPE_SPELL_MAGIC_SHIELD)
spell:id(44)
spell:cooldown(14 * 1000)
spell:groupCooldown(2 * 1000)
spell:level(14)
spell:mana(50)
spell:isSelfTarget(true)
spell:isAggressive(false)
spell:register()
