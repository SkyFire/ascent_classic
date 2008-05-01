CREATE TABLE `spell_effects_override` (
  `spellId` int(30) NOT NULL default '0',
  `EffectID` int(30) NOT NULL,
  `Disable` int(30) NOT NULL,
  `Effect` int(30) NOT NULL default '0',
  `BasePoints` int(30) NOT NULL default '0',
  `ApplyAuraName` int(30) NOT NULL default '0',
  `SpellGroupRelation` int(30) NOT NULL default '0',
  `MiscValue` int(30) NOT NULL default '0',
  `TriggerSpell` int(30) NOT NULL default '0',
  `ImplicitTargetA` int(30) NOT NULL default '0',
  `ImplicitTargetB` int(30) NOT NULL default '0',
  PRIMARY KEY  (`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
