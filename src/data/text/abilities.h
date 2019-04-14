const u8 gAbilityNoneDescription[] = _("No special ability.");
const u8 gAbilityStenchDescription[] = _("Helps repel wild POKéMON.");
const u8 gAbilityDrizzleDescription[] = _("Summons rain in battle.");
const u8 gAbilitySpeedBoostDescription[] = _("Gradually boosts SPEED.");
const u8 gAbilityBattleArmorDescription[] = _("Blocks critical hits.");
const u8 gAbilitySturdyDescription[] = _("Negates 1-hit KO attacks.");
const u8 gAbilityDampDescription[] = _("Prevents self-destruction.");
const u8 gAbilityLimberDescription[] = _("Prevents paralysis.");
const u8 gAbilitySandVeilDescription[] = _("Ups evasion in a sandstorm.");
const u8 gAbilityStaticDescription[] = _("Paralyzes on contact.");
const u8 gAbilityVoltAbsorbDescription[] = _("Turns electricity into HP.");
const u8 gAbilityWaterAbsorbDescription[] = _("Changes water into HP.");
const u8 gAbilityObliviousDescription[] = _("Prevents attraction.");
const u8 gAbilityCloudNineDescription[] = _("Negates weather effects.");
const u8 gAbilityCompoundEyesDescription[] = _("Raises accuracy.");
const u8 gAbilityInsomniaDescription[] = _("Prevents sleep.");
const u8 gAbilityColorChangeDescription[] = _("Changes type to foe's move.");
const u8 gAbilityImmunityDescription[] = _("Prevents poisoning.");
const u8 gAbilityFlashFireDescription[] = _("Powers up if hit by fire.");
const u8 gAbilityShieldDustDescription[] = _("Prevents added effects.");
const u8 gAbilityOwnTempoDescription[] = _("Prevents confusion.");
const u8 gAbilitySuctionCupsDescription[] = _("Firmly anchors the body.");
const u8 gAbilityIntimidateDescription[] = _("Lowers the foe's ATTACK.");
const u8 gAbilityShadowTagDescription[] = _("Prevents the foe's escape.");
const u8 gAbilityRoughSkinDescription[] = _("Hurts to touch.");
const u8 gAbilityWonderGuardDescription[] = _("“Super effective” hits.");
const u8 gAbilityLevitateDescription[] = _("Not hit by GROUND attacks.");
const u8 gAbilityEffectSporeDescription[] = _("Leaves spores on contact.");
const u8 gAbilitySynchronizeDescription[] = _("Passes on status problems.");
const u8 gAbilityClearBodyDescription[] = _("Prevents ability reduction.");
const u8 gAbilityNaturalCureDescription[] = _("Heals upon switching out.");
const u8 gAbilityLightningRodDescription[] = _("Draws electrical moves.");
const u8 gAbilitySereneGraceDescription[] = _("Promotes added effects.");
const u8 gAbilitySwiftSwimDescription[] = _("Raises SPEED in rain.");
const u8 gAbilityChlorophyllDescription[] = _("Raises SPEED in sunshine.");
const u8 gAbilityIlluminateDescription[] = _("Encounter rate increases.");
const u8 gAbilityTraceDescription[] = _("Copies special ability.");
const u8 gAbilityHugePowerDescription[] = _("Raises ATTACK.");
const u8 gAbilityPoisonPointDescription[] = _("Poisons foe on contact.");
const u8 gAbilityInnerFocusDescription[] = _("Prevents flinching.");
const u8 gAbilityMagmaArmorDescription[] = _("Prevents freezing.");
const u8 gAbilityWaterVeilDescription[] = _("Prevents burns.");
const u8 gAbilityMagnetPullDescription[] = _("Traps STEEL-type POKéMON.");
const u8 gAbilitySoundproofDescription[] = _("Avoids sound-based moves.");
const u8 gAbilityRainDishDescription[] = _("Slight HP recovery in rain.");
const u8 gAbilitySandStreamDescription[] = _("Summons a sandstorm.");
const u8 gAbilityPressureDescription[] = _("Raises foe's PP usage.");
const u8 gAbilityThickFatDescription[] = _("Heat-and-cold protection.");
const u8 gAbilityEarlyBirdDescription[] = _("Awakens quickly from sleep.");
const u8 gAbilityFlameBodyDescription[] = _("Burns the foe on contact.");
const u8 gAbilityRunAwayDescription[] = _("Makes escaping easier.");
const u8 gAbilityKeenEyeDescription[] = _("Prevents loss of accuracy.");
const u8 gAbilityHyperCutterDescription[] = _("Prevents ATTACK reduction.");
const u8 gAbilityPickupDescription[] = _("May pick up items.");
const u8 gAbilityTruantDescription[] = _("Moves only every two turns.");
const u8 gAbilityHustleDescription[] = _("Trades accuracy for power.");
const u8 gAbilityCuteCharmDescription[] = _("Infatuates on contact.");
const u8 gAbilityPlusDescription[] = _("Powers up with MINUS.");
const u8 gAbilityMinusDescription[] = _("Powers up with PLUS.");
const u8 gAbilityForecastDescription[] = _("Changes with the weather.");
const u8 gAbilityStickyHoldDescription[] = _("Prevents item theft.");
const u8 gAbilityShedSkinDescription[] = _("Heals the body by shedding.");
const u8 gAbilityGutsDescription[] = _("Ups ATTACK if suffering.");
const u8 gAbilityMarvelScaleDescription[] = _("Ups DEFENSE if suffering.");
const u8 gAbilityLiquidOozeDescription[] = _("Draining causes injury.");
const u8 gAbilityOvergrowDescription[] = _("Ups GRASS moves in a pinch.");
const u8 gAbilityBlazeDescription[] = _("Ups FIRE moves in a pinch.");
const u8 gAbilityTorrentDescription[] = _("Ups WATER moves in a pinch.");
const u8 gAbilitySwarmDescription[] = _("Ups BUG moves in a pinch.");
const u8 gAbilityRockHeadDescription[] = _("Prevents recoil damage.");
const u8 gAbilityDroughtDescription[] = _("Summons sunlight in battle.");
const u8 gAbilityArenaTrapDescription[] = _("Prevents fleeing.");
const u8 gAbilityVitalSpiritDescription[] = _("Prevents sleep.");
const u8 gAbilityWhiteSmokeDescription[] = _("Prevents ability reduction.");
const u8 gAbilityPurePowerDescription[] = _("Raises ATTACK.");
const u8 gAbilityShellArmorDescription[] = _("Blocks critical hits.");
const u8 gAbilityCacophonyDescription[] = _("Avoids sound-based moves.");
const u8 gAbilityAirLockDescription[] = _("Negates weather effects.");

const u8 gAbilityNames[ABILITIES_COUNT][ABILITY_NAME_LENGTH + 1] =
{
    [ABILITY_NONE] = _("-------"),
    [ABILITY_STENCH] = _("STENCH"),
    [ABILITY_DRIZZLE] = _("DRIZZLE"),
    [ABILITY_SPEED_BOOST] = _("SPEED BOOST"),
    [ABILITY_BATTLE_ARMOR] = _("BATTLE ARMOR"),
    [ABILITY_STURDY] = _("STURDY"),
    [ABILITY_DAMP] = _("DAMP"),
    [ABILITY_LIMBER] = _("LIMBER"),
    [ABILITY_SAND_VEIL] = _("SAND VEIL"),
    [ABILITY_STATIC] = _("STATIC"),
    [ABILITY_VOLT_ABSORB] = _("VOLT ABSORB"),
    [ABILITY_WATER_ABSORB] = _("WATER ABSORB"),
    [ABILITY_OBLIVIOUS] = _("OBLIVIOUS"),
    [ABILITY_CLOUD_NINE] = _("CLOUD NINE"),
    [ABILITY_COMPOUND_EYES] = _("COMPOUNDEYES"),
    [ABILITY_INSOMNIA] = _("INSOMNIA"),
    [ABILITY_COLOR_CHANGE] = _("COLOR CHANGE"),
    [ABILITY_IMMUNITY] = _("IMMUNITY"),
    [ABILITY_FLASH_FIRE] = _("FLASH FIRE"),
    [ABILITY_SHIELD_DUST] = _("SHIELD DUST"),
    [ABILITY_OWN_TEMPO] = _("OWN TEMPO"),
    [ABILITY_SUCTION_CUPS] = _("SUCTION CUPS"),
    [ABILITY_INTIMIDATE] = _("INTIMIDATE"),
    [ABILITY_SHADOW_TAG] = _("SHADOW TAG"),
    [ABILITY_ROUGH_SKIN] = _("ROUGH SKIN"),
    [ABILITY_WONDER_GUARD] = _("WONDER GUARD"),
    [ABILITY_LEVITATE] = _("LEVITATE"),
    [ABILITY_EFFECT_SPORE] = _("EFFECT SPORE"),
    [ABILITY_SYNCHRONIZE] = _("SYNCHRONIZE"),
    [ABILITY_CLEAR_BODY] = _("CLEAR BODY"),
    [ABILITY_NATURAL_CURE] = _("NATURAL CURE"),
    [ABILITY_LIGHTNING_ROD] = _("LIGHTNINGROD"),
    [ABILITY_SERENE_GRACE] = _("SERENE GRACE"),
    [ABILITY_SWIFT_SWIM] = _("SWIFT SWIM"),
    [ABILITY_CHLOROPHYLL] = _("CHLOROPHYLL"),
    [ABILITY_ILLUMINATE] = _("ILLUMINATE"),
    [ABILITY_TRACE] = _("TRACE"),
    [ABILITY_HUGE_POWER] = _("HUGE POWER"),
    [ABILITY_POISON_POINT] = _("POISON POINT"),
    [ABILITY_INNER_FOCUS] = _("INNER FOCUS"),
    [ABILITY_MAGMA_ARMOR] = _("MAGMA ARMOR"),
    [ABILITY_WATER_VEIL] = _("WATER VEIL"),
    [ABILITY_MAGNET_PULL] = _("MAGNET PULL"),
    [ABILITY_SOUNDPROOF] = _("SOUNDPROOF"),
    [ABILITY_RAIN_DISH] = _("RAIN DISH"),
    [ABILITY_SAND_STREAM] = _("SAND STREAM"),
    [ABILITY_PRESSURE] = _("PRESSURE"),
    [ABILITY_THICK_FAT] = _("THICK FAT"),
    [ABILITY_EARLY_BIRD] = _("EARLY BIRD"),
    [ABILITY_FLAME_BODY] = _("FLAME BODY"),
    [ABILITY_RUN_AWAY] = _("RUN AWAY"),
    [ABILITY_KEEN_EYE] = _("KEEN EYE"),
    [ABILITY_HYPER_CUTTER] = _("HYPER CUTTER"),
    [ABILITY_PICKUP] = _("PICKUP"),
    [ABILITY_TRUANT] = _("TRUANT"),
    [ABILITY_HUSTLE] = _("HUSTLE"),
    [ABILITY_CUTE_CHARM] = _("CUTE CHARM"),
    [ABILITY_PLUS] = _("PLUS"),
    [ABILITY_MINUS] = _("MINUS"),
    [ABILITY_FORECAST] = _("FORECAST"),
    [ABILITY_STICKY_HOLD] = _("STICKY HOLD"),
    [ABILITY_SHED_SKIN] = _("SHED SKIN"),
    [ABILITY_GUTS] = _("GUTS"),
    [ABILITY_MARVEL_SCALE] = _("MARVEL SCALE"),
    [ABILITY_LIQUID_OOZE] = _("LIQUID OOZE"),
    [ABILITY_OVERGROW] = _("OVERGROW"),
    [ABILITY_BLAZE] = _("BLAZE"),
    [ABILITY_TORRENT] = _("TORRENT"),
    [ABILITY_SWARM] = _("SWARM"),
    [ABILITY_ROCK_HEAD] = _("ROCK HEAD"),
    [ABILITY_DROUGHT] = _("DROUGHT"),
    [ABILITY_ARENA_TRAP] = _("ARENA TRAP"),
    [ABILITY_VITAL_SPIRIT] = _("VITAL SPIRIT"),
    [ABILITY_WHITE_SMOKE] = _("WHITE SMOKE"),
    [ABILITY_PURE_POWER] = _("PURE POWER"),
    [ABILITY_SHELL_ARMOR] = _("SHELL ARMOR"),
    [ABILITY_CACOPHONY] = _("CACOPHONY"),
    [ABILITY_AIR_LOCK] = _("AIR LOCK"),
};

const u8 *const gAbilityDescriptionPointers[ABILITIES_COUNT] =
{
    [ABILITY_NONE] = gAbilityNoneDescription,
    [ABILITY_STENCH] = gAbilityStenchDescription,
    [ABILITY_DRIZZLE] = gAbilityDrizzleDescription,
    [ABILITY_SPEED_BOOST] = gAbilitySpeedBoostDescription,
    [ABILITY_BATTLE_ARMOR] = gAbilityBattleArmorDescription,
    [ABILITY_STURDY] = gAbilitySturdyDescription,
    [ABILITY_DAMP] = gAbilityDampDescription,
    [ABILITY_LIMBER] = gAbilityLimberDescription,
    [ABILITY_SAND_VEIL] = gAbilitySandVeilDescription,
    [ABILITY_STATIC] = gAbilityStaticDescription,
    [ABILITY_VOLT_ABSORB] = gAbilityVoltAbsorbDescription,
    [ABILITY_WATER_ABSORB] = gAbilityWaterAbsorbDescription,
    [ABILITY_OBLIVIOUS] = gAbilityObliviousDescription,
    [ABILITY_CLOUD_NINE] = gAbilityCloudNineDescription,
    [ABILITY_COMPOUND_EYES] = gAbilityCompoundEyesDescription,
    [ABILITY_INSOMNIA] = gAbilityInsomniaDescription,
    [ABILITY_COLOR_CHANGE] = gAbilityColorChangeDescription,
    [ABILITY_IMMUNITY] = gAbilityImmunityDescription,
    [ABILITY_FLASH_FIRE] = gAbilityFlashFireDescription,
    [ABILITY_SHIELD_DUST] = gAbilityShieldDustDescription,
    [ABILITY_OWN_TEMPO] = gAbilityOwnTempoDescription,
    [ABILITY_SUCTION_CUPS] = gAbilitySuctionCupsDescription,
    [ABILITY_INTIMIDATE] = gAbilityIntimidateDescription,
    [ABILITY_SHADOW_TAG] = gAbilityShadowTagDescription,
    [ABILITY_ROUGH_SKIN] = gAbilityRoughSkinDescription,
    [ABILITY_WONDER_GUARD] = gAbilityWonderGuardDescription,
    [ABILITY_LEVITATE] = gAbilityLevitateDescription,
    [ABILITY_EFFECT_SPORE] = gAbilityEffectSporeDescription,
    [ABILITY_SYNCHRONIZE] = gAbilitySynchronizeDescription,
    [ABILITY_CLEAR_BODY] = gAbilityClearBodyDescription,
    [ABILITY_NATURAL_CURE] = gAbilityNaturalCureDescription,
    [ABILITY_LIGHTNING_ROD] = gAbilityLightningRodDescription,
    [ABILITY_SERENE_GRACE] = gAbilitySereneGraceDescription,
    [ABILITY_SWIFT_SWIM] = gAbilitySwiftSwimDescription,
    [ABILITY_CHLOROPHYLL] = gAbilityChlorophyllDescription,
    [ABILITY_ILLUMINATE] = gAbilityIlluminateDescription,
    [ABILITY_TRACE] = gAbilityTraceDescription,
    [ABILITY_HUGE_POWER] = gAbilityHugePowerDescription,
    [ABILITY_POISON_POINT] = gAbilityPoisonPointDescription,
    [ABILITY_INNER_FOCUS] = gAbilityInnerFocusDescription,
    [ABILITY_MAGMA_ARMOR] = gAbilityMagmaArmorDescription,
    [ABILITY_WATER_VEIL] = gAbilityWaterVeilDescription,
    [ABILITY_MAGNET_PULL] = gAbilityMagnetPullDescription,
    [ABILITY_SOUNDPROOF] = gAbilitySoundproofDescription,
    [ABILITY_RAIN_DISH] = gAbilityRainDishDescription,
    [ABILITY_SAND_STREAM] = gAbilitySandStreamDescription,
    [ABILITY_PRESSURE] = gAbilityPressureDescription,
    [ABILITY_THICK_FAT] = gAbilityThickFatDescription,
    [ABILITY_EARLY_BIRD] = gAbilityEarlyBirdDescription,
    [ABILITY_FLAME_BODY] = gAbilityFlameBodyDescription,
    [ABILITY_RUN_AWAY] = gAbilityRunAwayDescription,
    [ABILITY_KEEN_EYE] = gAbilityKeenEyeDescription,
    [ABILITY_HYPER_CUTTER] = gAbilityHyperCutterDescription,
    [ABILITY_PICKUP] = gAbilityPickupDescription,
    [ABILITY_TRUANT] = gAbilityTruantDescription,
    [ABILITY_HUSTLE] = gAbilityHustleDescription,
    [ABILITY_CUTE_CHARM] = gAbilityCuteCharmDescription,
    [ABILITY_PLUS] = gAbilityPlusDescription,
    [ABILITY_MINUS] = gAbilityMinusDescription,
    [ABILITY_FORECAST] = gAbilityForecastDescription,
    [ABILITY_STICKY_HOLD] = gAbilityStickyHoldDescription,
    [ABILITY_SHED_SKIN] = gAbilityShedSkinDescription,
    [ABILITY_GUTS] = gAbilityGutsDescription,
    [ABILITY_MARVEL_SCALE] = gAbilityMarvelScaleDescription,
    [ABILITY_LIQUID_OOZE] = gAbilityLiquidOozeDescription,
    [ABILITY_OVERGROW] = gAbilityOvergrowDescription,
    [ABILITY_BLAZE] = gAbilityBlazeDescription,
    [ABILITY_TORRENT] = gAbilityTorrentDescription,
    [ABILITY_SWARM] = gAbilitySwarmDescription,
    [ABILITY_ROCK_HEAD] = gAbilityRockHeadDescription,
    [ABILITY_DROUGHT] = gAbilityDroughtDescription,
    [ABILITY_ARENA_TRAP] = gAbilityArenaTrapDescription,
    [ABILITY_VITAL_SPIRIT] = gAbilityVitalSpiritDescription,
    [ABILITY_WHITE_SMOKE] = gAbilityWhiteSmokeDescription,
    [ABILITY_PURE_POWER] = gAbilityPurePowerDescription,
    [ABILITY_SHELL_ARMOR] = gAbilityShellArmorDescription,
    [ABILITY_CACOPHONY] = gAbilityCacophonyDescription,
    [ABILITY_AIR_LOCK] = gAbilityAirLockDescription,
};
