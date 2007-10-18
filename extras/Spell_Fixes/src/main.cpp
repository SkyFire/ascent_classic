/*

Purpuse : make modifications in dbc file for ascent project to set values that are missing for spells in able to work
Warning : Output result is changed in order to obtain ascent specific data. Do not try to use or interpret data for anything else
Warning : SpellEntry + sql_translation_table + SPELL_ENTRY must have the same structure !!!

project status : not finished yet..not tested

*/

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <assert.h>
#include <conio.h>
#include <windows.h>

#include "../../../src/shared/Database/dbcfile.h"
#include "../../../src/shared/crc32.cpp"
#include "../../../dep/include/mysql/mysql.h"

#include "defines.h"

DBCFile dbc;

//make this after the main
void do_fixes(TCHAR *inf);
void dump_as_sql(TCHAR *inf);
void import_from_sql();

void print_usage()
{
	printf("Usage: -dofix spell_fix.exe inf=input_spell.dbc\n");
	printf("parameters: -h output this help message\n");
	printf("parameters: -dofix start making custom fixes\n");
	printf("parameters: -conv dump DBC as sql\n");
	printf("parameters: -imp import data from an sql table(experimental)\n");
	printf("parameters: inf= specify the input filename(no spaces)\n");
}

//-dofix inf=spell.dbc
//-conv inf=spell.dbc
void main(int argc, TCHAR* argv[])
{
	int dotask=0;
	TCHAR file_name[300];//store the input filename
	file_name[0]=0;

	//make sure it is morron proof. Ever met a guy clicking all the next buttons in an install kit without reading 1 line ? :P
	if(argc<=1)
	{
		printf("For safety mesures this program cannot be run without parameters. Type -h for help");
		print_usage();
		exit(1);
	}
	//maybe right now we do not have a lot of options but what the hack, let's make it fancy :)
	for (int i=1; i<argc; i++)
	{
		int arglen=(int)strlen(argv[i]);
		if (strnicmp(argv[i],"-h",2)==0) print_usage();
		if (strnicmp(argv[i],"-dofix",6)==0) dotask=1;
		if (strnicmp(argv[i],"-conv",5)==0) dotask=2;
		if (strnicmp(argv[i],"-imp",4)==0) dotask=3;
		if (strnicmp(argv[i],"inf=",4)==0) strcpy(file_name,argv[i]+4);
		
	}

	//we will work with this anyway. This whole tool is about this var :P
	if(strlen(file_name)<2)
	{
		printf("This tool is designed for some special purpuse and it needs an imput file \n");
		print_usage();
		exit(1);
	}

	dbc.open(file_name);

	if(!dbc.getFieldCount())
	{
		printf("error, could not open dbc file\n");
		exit(1);
	}
	else printf("Opened DBC with %u fields and %u rows\n",(int)dbc.getFieldCount(),(int)dbc.getRecordCount());

	switch(dotask)
	{
		case 1:		do_fixes(file_name);		break;
		case 2:		dump_as_sql(file_name);		break;
		case 3:		import_from_sql();			break;
		default:								break;
	}
//	getch();
}

__int64 getfilesize(TCHAR *infname)
{
	FILE *inf=fopen(infname,"r");
	if(!inf)
	{
		printf("error ocured while opening file\n");
		exit(1);
	}
	fpos_t len;
	fseek(inf,0,SEEK_END);
	fgetpos(inf,&len);
	fclose(inf);
	return len;
}

unsigned int get_spell_row(uint32 id)
{
	//replace this later with at least a binary search
	for(unsigned int j=0;j<dbc.getRecordCount();j++)
		if(dbc.getRecord(j).getUInt(SPELL_ENTRY_Id)==id)
			return j;
	return 0xFFFFFFFF;
}

/*
void change_spell_value(int spell_id,unsigned int field, unsigned int value)
{
	uint32 row=get_spell_row(spell_id);
	if(!row)return;
	dbc.getRecord(row).SetUInt(field,value);
}

void change_spell_value(int spell_id,unsigned int field, signed int value)
{
	uint32 row=get_spell_row(spell_id);
	if(!row)return;
	dbc.getRecord(row).SetInt(field,value);
}

void change_spell_value(int spell_id,unsigned int field, float value)
{
	uint32 row=get_spell_row(spell_id);
	if(!row)return;
	dbc.getRecord(row).SetFloat(field,value);
}*/

void assign_row_to_SpellEntry(SpellEntry **spe,uint32 row)
{
	*spe = (SpellEntry *)dbc.getRecord(row).getRowStart();
}

void assign_Spell_to_SpellEntry(SpellEntry **spe,uint32 spell_id)
{
	uint32 row=get_spell_row(spell_id);
	if(row==0xFFFFFFFF)
		*spe = NULL;
	else assign_row_to_SpellEntry(spe,row);
}

void do_fixes(TCHAR *inf)
{
	if(!dbc.getFieldCount())
	{
		printf("error, could not open dbc file\n");
		exit(1);
	}
	else printf("Opened DBC with %u fields and %u rows\n",(int)dbc.getFieldCount(),(int)dbc.getRecordCount());

	uint32 cnt = (uint32)dbc.getRecordCount();
	uint32 effect;
	uint32 All_Seal_Groups_Combined=0;
	SpellEntry * sp;

	for(uint32 x=0; x < cnt; x++)
	{
		uint32 result = 0;
		// SpellID
//		uint32 spellid = dbc.getRecord(x).getUInt(SPELL_ENTRY_Id);
		// get spellentry
//		assign_Spell_to_SpellEntry(&sp,spellid);
		assign_row_to_SpellEntry(&sp,x);
		// Description field
		char* desc = (char*)dbc.getRecord(x).getString(SPELL_ENTRY_Description); 
		const char* ranktext = dbc.getRecord(x).getString(SPELL_ENTRY_Rank);
		const char* nametext = dbc.getRecord(x).getString(SPELL_ENTRY_Name);

		uint32 rank = 0;
		uint32 type = 0;
		uint32 namehash = 0;

		// hash the name
		//!!!!!!! representing all strings on 32 bits is dangerous. There is a chance to get same hash for a lot of strings ;)
        namehash = (uint32) crc32((const unsigned char*)nametext, (uint32)strlen(nametext));
		sp->NameHash   = namehash; //need these set before we start processing spells

		sp->proc_interval = 0;//trigger at each event
		sp->c_is_flags = 0;

		// parse rank text
		if(!sscanf(ranktext, "Rank %d", (unsigned int*)&rank))
			rank = 0;

		if(namehash == 0x56392512)			/* seal of light */
			sp->procChance=45;	/* this will do */

		//these mostly do not mix so we can use else 
        // look for seal, etc in name
        if(strstr(nametext, "Seal"))
		{
            type |= SPELL_TYPE_SEAL;
			All_Seal_Groups_Combined |= sp->SpellGroupType;
		}
        else if(strstr(nametext, "Blessing"))
            type |= SPELL_TYPE_BLESSING;
        else if(strstr(nametext, "Curse"))
            type |= SPELL_TYPE_CURSE;
        else if(strstr(nametext, "Aspect"))
            type |= SPELL_TYPE_ASPECT;
        else if(strstr(nametext, "Sting") || strstr(nametext, "sting"))
            type |= SPELL_TYPE_STING;
        // don't break armor items!
        else if(strcmp(nametext, "Armor") && strstr(nametext, "Armor") || strstr(nametext, "Demon Skin"))
            type |= SPELL_TYPE_ARMOR;
        else if(strstr(nametext, "Aura"))
            type |= SPELL_TYPE_AURA;
		else if(strstr(nametext, "Track")==nametext)
            type |= SPELL_TYPE_TRACK;
//		else if(strstr(nametext, "Gift of the Wild") || strstr(nametext, "Mark of the Wild"))
		else if(namehash==0xF77CAB47 || namehash==0x202F1D74)
            type |= SPELL_TYPE_MARK_GIFT;
//		else if(strstr(nametext, "Immolation Trap") || strstr(nametext, "Freezing Trap") || strstr(nametext, "Frost Trap") || strstr(nametext, "Explosive Trap") || strstr(nametext, "Snake Trap"))
		else if(namehash==0x238FEAF2 || namehash==0x593D7EC7 || namehash==0xF7318BD7 || namehash==0x54B6F3EE || namehash==0xF57361BB)
            type |= SPELL_TYPE_HUNTER_TRAP;
//		else if(strstr(nametext, "Arcane Intellect") || strstr(nametext, "Arcane Brilliance"))
		else if(namehash==0x2F7018BA || namehash==0x9CA89B04)
            type |= SPELL_TYPE_MAGE_INTEL;
//		else if(strstr(nametext, "Amplify Magic") || strstr(nametext, "Dampen Magic"))
		else if(namehash==0x75E2ACFF || namehash==0x3C4E2404)
            type |= SPELL_TYPE_MAGE_MAGI;
//		else if(strstr(nametext, "Fire Ward") || strstr(nametext, "Frost Ward"))
		else if(namehash==0xC83A99CB || namehash==0x80D1ACFF)
            type |= SPELL_TYPE_MAGE_WARDS;
//		else if(strstr(nametext, "Shadow Protection") || strstr(nametext, "Prayer of Shadow Protection"))
		else if(namehash==0xD9948728 || namehash==0xBDA6C77C)
            type |= SPELL_TYPE_PRIEST_SH_PPROT;
//		else if(strstr(nametext, "Water Shield") || strstr(nametext, "Earth Shield") || strstr(nametext, "Lightning Shield"))
		else if(namehash==0xB9DAC27E || namehash==0x0227BA8B || namehash==0x2123CF1E)
            type |= SPELL_TYPE_SHIELD;
//		else if(strstr(nametext, "Power Word: Fortitude") || strstr(nametext, "Prayer of Fortitude"))
		else if(namehash==0x9056D252 || namehash==0x519F97B3)
            type |= SPELL_TYPE_FORTITUDE;
//		else if(strstr(nametext, "Divine Spirit") || strstr(nametext, "Prayer of Spirit"))
		else if(namehash==0xB1792C31 || namehash==0xC8F56DAF)
            type |= SPELL_TYPE_SPIRIT;
//		else if(strstr(nametext, "Curse of Weakness") || strstr(nametext, "Curse of Agony") || strstr(nametext, "Curse of Recklessness") || strstr(nametext, "Curse of Tongues") || strstr(nametext, "Curse of the Elements") || strstr(nametext, "Curse of Idiocy") || strstr(nametext, "Curse of Shadow") || strstr(nametext, "Curse of Doom"))
//		else if(namehash==4129426293 || namehash==885131426 || namehash==626036062 || namehash==3551228837 || namehash==2784647472 || namehash==776142553 || namehash==3407058720 || namehash==202747424)
//		else if(strstr(nametext, "Curse of "))
//            type |= SPELL_TYPE_WARLOCK_CURSES;
		else if(strstr(nametext, "Immolate") || strstr(nametext, "Conflagrate"))
			type |= SPELL_TYPE_WARLOCK_IMMOLATE;
		else if(strstr(nametext, "Amplify Magic") || strstr(nametext, "Dampen Magic"))
			type |= SPELL_TYPE_MAGE_AMPL_DUMP;
		else if(strstr(desc, "Finishing move")==desc)
			type |= SPELL_TYPE_FINISHING_MOVE;
        else if(strstr(desc, "Battle Elixir"))
            type |= SPELL_TYPE_ELIXIR_BATTLE;
        else if(strstr(desc, "Guardian Elixir"))
            type |= SPELL_TYPE_ELIXIR_GUARDIAN;
        else if(strstr(desc, "Battle and Guardian elixir"))
            type |= SPELL_TYPE_ELIXIR_FLASK;
		else if(namehash==0xFF89ABD2)		// hunter's mark
			type |= SPELL_TYPE_HUNTER_MARK;
		if(IsDamagingSpell(sp))
			sp->c_is_flags |= SPELL_FLAG_IS_DAMAGING;
		if(IsHealingSpell(sp))
			sp->c_is_flags |= SPELL_FLAG_IS_HEALING;

		//stupid spell ranking problem
		if(sp->spellLevel==0)
		{
			uint32 new_level=0;
			if(strstr(nametext, "Apprentice "))
				new_level = 1;
			else if(strstr(nametext, "Journeyman "))
				new_level = 2;
			else if(strstr(nametext, "Expert "))
				new_level = 3;
			else if(strstr(nametext, "Artisan "))
				new_level = 4;
			else if(strstr(nametext, "Master "))
				new_level = 5;
			if(new_level!=0)
			{
				uint32 teachspell=0;
				if(sp->Effect[0]==SPELL_EFFECT_LEARN_SPELL)
					teachspell = sp->EffectTriggerSpell[0];
				else if(sp->Effect[1]==SPELL_EFFECT_LEARN_SPELL)
					teachspell = sp->EffectTriggerSpell[1];
				else if(sp->Effect[2]==SPELL_EFFECT_LEARN_SPELL)
					teachspell = sp->EffectTriggerSpell[2];
				if(teachspell)
				{
					SpellEntry *spellInfo;
					assign_Spell_to_SpellEntry(&spellInfo,teachspell);
					spellInfo->spellLevel = new_level;
					sp->spellLevel = new_level;
				}
			}
		}

		/*FILE * f = fopen("C:\\spells.txt", "a");
		fprintf(f, "case 0x%08X:		// %s\n", namehash, nametext);
		fclose(f);*/

		// find diminishing status
		sp->DiminishStatus = GetDiminishingGroup(namehash);
		sp->buffIndexType=0;
		switch(namehash)
		{
		case 0xFF89ABD2:		// Hunter's mark
			sp->buffIndexType = SPELL_TYPE_INDEX_MARK;
			break;

		case 0x2266F1F2:		// Polymorph
		case 0xB15D524E:		// Polymorph: Chicken
		case 0xA73086F1:		// Polymorph: Pig
		case 0xBD6B76DC:		// Polymorph: Sheep
		case 0x0128F8E9:		// Polymorph: Turtle
			sp->buffIndexType = SPELL_TYPE_INDEX_POLYMORPH;
			break;

		case 0x328E44DC:		// Fear
			sp->buffIndexType = SPELL_TYPE_INDEX_FEAR;
			break;

		case 0x4219BB33:		// Sap
			sp->buffIndexType = SPELL_TYPE_INDEX_SAP;
			break;

		case 0x94675337:		// Scare Beast
			sp->buffIndexType = SPELL_TYPE_INDEX_SCARE_BEAST;
			break;

		case 0x898B6207:		// Hibernate
			sp->buffIndexType = SPELL_TYPE_INDEX_HIBERNATE;
			break;

		case 0x0227BA8B:		// Earth Shield
			sp->buffIndexType = SPELL_TYPE_INDEX_EARTH_SHIELD;
			break;

		case 0x73812928:		// Cyclone
			sp->buffIndexType = SPELL_TYPE_INDEX_CYCLONE;
			break;

		case 0x3D46465A:		// Banish
			sp->buffIndexType = SPELL_TYPE_INDEX_BANISH;
			break;
		}

		// HACK FIX: Break roots/fear on damage.. this needs to be fixed properly!
		if(!(sp->AuraInterruptFlags & AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN))
		{
			for(uint32 z = 0; z < 3; ++z) {
				if(sp->EffectApplyAuraName[z] == SPELL_AURA_MOD_FEAR ||
					sp->EffectApplyAuraName[z] == SPELL_AURA_MOD_ROOT)
				{
					sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_UNUSED2;
					break;
				}
			}
		}

		// set extra properties
		sp->buffType   = type;
		sp->RankNumber = rank;

		uint32 pr=sp->procFlags;
		for(uint32 y=0;y < 3; y++)
		{
			// get the effect number from the spell
			effect = dbc.getRecord(x).getUInt(64 + y); // spelleffect[0] = 64 // 2.0.1

			//spell group
			/*if(effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT1||effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT2||
				effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT3||effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT4)
			{
			
					const char *p=desc;
					while(p=strstr(p,"$"))
					{
						p++;
						//got $  -> check if spell
						if(*p>='0' && *p <='9')
						{//woot this is spell id
							uint32 tmp=atoi(p);
							SpellEntry*s=sSpellStore.LookupEntry(tmp);
							bool ch=false;
							for(uint32 i=0;i<3;i++)
								if(s->EffectTriggerSpell[i])
								{
									ch=true;
									result=tmp;
									break;
								}
							if(ch)break;
							result=tmp;
							
						}
					
					}
				
			}else*/
			/*if(effect==SPELL_EFFECT_ENCHANT_ITEM)//add inventory type check
			{
				result=0;
				//136--desc field
				//dirty code
				if(strstr(desc,"head"))
					result|=(1<<INVTYPE_HEAD);
				if(strstr(desc,"leg"))
					result|=(1<<INVTYPE_LEGS);
				if(strstr(desc,"neck"))
					result|=(1<<INVTYPE_NECK);
				if(strstr(desc,"shoulder"))
					result|=(1<<INVTYPE_SHOULDERS);
				if(strstr(desc,"body"))
					result|=(1<<INVTYPE_BODY);
				if(strstr(desc,"chest"))
					result|=((1<<INVTYPE_CHEST)|(1<<INVTYPE_ROBE));
				if(strstr(desc,"waist"))
					result|=(1<<INVTYPE_WAIST);
				if(strstr(desc,"foot")||strstr(desc,"feet")||strstr(desc,"boot"))
					result|=(1<<INVTYPE_FEET);
				if(strstr(desc,"wrist")||strstr(desc,"bracer"))
					result|=(1<<INVTYPE_WRISTS);
				if(strstr(desc,"hand")||strstr(desc,"glove"))
					result|=(1<<INVTYPE_HANDS);
				if(strstr(desc,"finger")||strstr(desc,"ring"))
					result|=(1<<INVTYPE_FINGER);
				if(strstr(desc,"trinket"))
					result|=(1<<INVTYPE_TRINKET);
				if(strstr(desc,"shield"))
					result|=(1<<INVTYPE_SHIELD);
				if(strstr(desc,"cloak"))
					result|=(1<<INVTYPE_CLOAK);
				if(strstr(desc,"robe"))
					result|=(1<<INVTYPE_ROBE);
				//if(strstr(desc,"two")||strstr(desc,"Two"))
				//	result|=(1<<INVTYPE_2HWEAPON);<-handled in subclass
			}
			else*/
			if(effect==SPELL_EFFECT_APPLY_AURA)
			{
				uint32 aura = dbc.getRecord(x).getUInt(94+y); // 58+30+3 = 91
				if( aura == SPELL_AURA_PROC_TRIGGER_SPELL ||
					aura == SPELL_AURA_PROC_TRIGGER_DAMAGE
					)//search for spellid in description
				{
					const char *p=desc;
					while((p=strstr(p,"$")))
					{
						p++;
						//got $  -> check if spell
						if(*p>='0' && *p <='9')
						{//woot this is spell id
						
							result=atoi(p);
						}					
					}
					pr=0;

					int len = (int)strlen(desc);
					for(int i = 0; i < len; ++i)
						desc[i] = tolower(desc[i]);
					//dirty code for procs, if any1 got any better idea-> u are welcome
					//139944 --- some magic number, it will trigger on all hits etc
						//for seems to be smth like custom check
					if(strstr(desc,"takes damage"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc,"attackers when hit"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"character strikes an enemy"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"strike you with a melee attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"target casts a spell"))
						pr|=PROC_ON_CAST_SPELL;
					if(strstr(desc,"any damage caused"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc,"the next melee attack against the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"when successfully hit"))
						pr|=PROC_ON_MELEE_ATTACK ;
					if(strstr(desc,"an enemy on hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"when it hits"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"when successfully hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"on a successful hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"damage to attacker on hit"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"on a hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"strikes you with a melee attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"when caster takes damage"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc,"when the caster is using melee attacks"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"when struck in combat"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"successful melee attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"chance per attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"chance per hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"that strikes a party member"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"when hit by a melee attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"your critical strikes"))
						pr|=PROC_ON_CRIT_ATTACK;
					if(strstr(desc,"whenever you deal ranged damage"))
						pr|=PROC_ON_RANGED_ATTACK;
//					if(strstr(desc,"whenever you deal melee damage"))
					if(strstr(desc,"you deal melee damage"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"your melee attacks"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"damage with your Sword"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"when struck in melee combat"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"any successful spell cast against the priest"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if(strstr(desc,"the next melee attack on the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"striking melee or ranged attackers"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM|PROC_ON_RANGED_ATTACK_VICTIM;
					if(strstr(desc,"when damaging an enemy in melee"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"victim of a critical strike"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
					if(strstr(desc,"on successful melee or ranged attack"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
					if(strstr(desc,"enemy that strikes you in melee"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"after getting a critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if(strstr(desc,"whenever damage is dealt to you"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc,"when ranged or melee damage is dealt"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
					if(strstr(desc,"damaging melee attacks"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"on melee or ranged attack"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
					if(strstr(desc,"on a melee swing"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"Chance on melee"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"spell criticals against you"))
						pr|=PROC_ON_SPELL_CRIT_HIT_VICTIM;
					if(strstr(desc,"after being struck by a melee or ranged critical hit"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
//					if(strstr(desc,"on a critical hit"))
					if(strstr(desc,"critical hit"))
						pr|=PROC_ON_CRIT_ATTACK;
					if(strstr(desc,"strikes the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"a spell, melee or ranged attack hits the caster"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc,"after dealing a critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if(strstr(desc,"each melee or ranged damage hit against the priest"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM|PROC_ON_RANGED_ATTACK_VICTIM;				
					if(strstr(desc, "a chance to deal additional"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc, "chance to get an extra attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc, "melee attacks has"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc, "any damage spell hits a target"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc, "giving each melee attack a chance"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc, "damage when hit"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM; //myabe melee damage ?
					if(strstr(desc, "gives your"))
					{
						if(strstr(desc, "melee"))
							pr|=PROC_ON_MELEE_ATTACK;
						else if(strstr(desc,"sinister strike, backstab, gouge and shiv"))
							pr|=PROC_ON_CAST_SPELL;
						else if(strstr(desc,"chance to daze the target"))
							pr|=PROC_ON_CAST_SPELL;
						else if(strstr(desc,"finishing moves"))
							pr|=PROC_ON_CAST_SPELL;
//						else if(strstr(desc,"shadow bolt, shadowburn, soul fire, incinerate, searing pain and conflagrate"))
//							pr|=PROC_ON_CAST_SPELL|PROC_TAGRGET_SELF;
						//we should find that specific spell (or group) on what we will trigger
						else pr|=PROC_ON_CAST_SPECIFIC_SPELL;
					}
					if(strstr(desc, "chance to add an additional combo") && strstr(desc, "critical") )
						pr|=PROC_ON_CRIT_ATTACK;
					else if(strstr(desc, "chance to add an additional combo"))
						pr|=PROC_ON_CAST_SPELL;
					if(strstr(desc, "victim of a melee or ranged critical strike"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
					if(strstr(desc, "getting a critical effect from"))
						pr|=PROC_ON_SPELL_CRIT_HIT_VICTIM;
					if(strstr(desc, "damaging attack is taken"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc, "struck by a Stun or Immobilize"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if(strstr(desc, "melee critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if(strstr(nametext, "Bloodthirst"))
						pr|=PROC_ON_MELEE_ATTACK | PROC_TAGRGET_SELF;
					if(strstr(desc, "experience or honor"))
						pr|=PROC_ON_GAIN_EXPIERIENCE;
					if(strstr(desc,"your next offensive ability"))
						pr|=PROC_ON_CAST_SPELL;
					if(strstr(desc,"hit by a melee or ranged attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if(strstr(desc,"enemy strikes the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"melee and ranged attacks against you"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if(strstr(desc,"when a block occurs"))
						pr|=PROC_ON_BLOCK_VICTIM;
					if(strstr(desc,"dealing a critical strike from a weapon swing, spell, or ability"))
						pr|=PROC_ON_CRIT_ATTACK|PROC_ON_SPELL_CRIT_HIT;
					if(strstr(desc,"dealing a critical strike from a weapon swing, spell, or ability"))
						pr|=PROC_ON_CRIT_ATTACK|PROC_ON_SPELL_CRIT_HIT;
					if(strstr(desc,"shadow bolt critical strikes increase shadow damage"))
						pr|=PROC_ON_SPELL_CRIT_HIT;
					if(strstr(desc,"next offensive ability"))
						pr|=PROC_ON_CAST_SPELL;
					if(strstr(desc,"after being hit with a shadow or fire spell"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if(strstr(desc,"giving each melee attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"each strike has"))
						pr|=PROC_ON_MELEE_ATTACK;		
					if(strstr(desc,"your Fire damage spell hits"))
						pr|=PROC_ON_CAST_SPELL;		//this happens only on hit ;)
					if(strstr(desc,"corruption, curse of agony, siphon life and seed of corruption spells also cause"))
						pr|=PROC_ON_CAST_SPELL;
					if(strstr(desc,"pain, mind flay and vampiric touch spells also cause"))
						pr|=PROC_ON_CAST_SPELL;
					if(strstr(desc,"shadow damage spells have"))
						pr|=PROC_ON_CAST_SPELL;
					if(strstr(desc,"your spell criticals have"))
						pr|=PROC_ON_SPELL_CRIT_HIT | PROC_ON_SPELL_CRIT_HIT_VICTIM;
					if(strstr(desc,"after dodging their attack"))
					{
						pr|=PROC_ON_DODGE_VICTIM;
						if(strstr(desc,"add a combo point"))
							pr|=PROC_TAGRGET_SELF;
					}
					if(strstr(desc,"fully resisting"))
						pr|=PROC_ON_RESIST_VICTIM;
					if(strstr(desc,"Your Shadow Word: Pain, Mind Flay and Vampiric Touch spells also cause the target"))
						pr|=PROC_ON_CAST_SPELL;
//					if(strstr(desc,"chill effect to your Blizzard"))
//						pr|=PROC_ON_CAST_SPELL;	
					//////////////////////////////////////////////////
					//proc dmg flags
					//////////////////////////////////////////////////
					if(strstr(desc,"each attack blocked"))
						pr|=PROC_ON_BLOCK_VICTIM;
					if(strstr(desc,"into flame, causing an additional"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"victim of a critical melee strike"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
					if(strstr(desc,"damage to melee attackers"))
						pr|=PROC_ON_MELEE_ATTACK;
					if(strstr(desc,"target blocks a melee attack"))
						pr|=PROC_ON_BLOCK_VICTIM;
					if(strstr(desc,"ranged and melee attacks to deal"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if(strstr(desc,"damage on hit"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc,"after being hit by any damaging attack"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc,"striking melee or ranged attackers"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if(strstr(desc,"damage to attackers when hit"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"striking melee attackers"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if(strstr(desc,"whenever the caster takes damage"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if(strstr(desc,"damage on every attack"))
						pr|=PROC_ON_MELEE_ATTACK | PROC_ON_RANGED_ATTACK;
					if(strstr(desc,"chance to reflect Fire spells"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if(strstr(desc,"hunter takes on the aspects of a hawk"))
						pr|=PROC_TAGRGET_SELF | PROC_ON_RANGED_ATTACK;
					if(strstr(desc,"successful auto shot attacks"))
						pr|=PROC_ON_AUTO_SHOT_HIT;
					if(strstr(desc,"after getting a critical effect from your"))
						pr=PROC_ON_SPELL_CRIT_HIT;
//					if(strstr(desc,"Your critical strikes from Fire damage"))
//						pr|=PROC_ON_SPELL_CRIT_HIT;
				}//end "if procspellaura"
				//dirty fix to remove auras that should expire on event and they are not
//				else if(sp->procCharges>0)
//				{
					//there are at least 185 spells that should loose charge uppon some event.Be prepared to add more here !
					// ! watch it cause this might conflict with our custom modified spells like : lighning shield !

					//spells like : Presence of Mind,Nature's Swiftness, Inner Focus,Amplify Curse,Coup de Grace
					//SELECT * FROM dbc_spell where proc_charges!=0 and (effect_aura_1=108 or effect_aura_2=108 and effect_aura_3=108) and description!=""
//					if(aura == SPELL_AURA_ADD_PCT_MODIFIER)
//						sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_CAST_SPELL;
					//most of them probably already have these flags...not sure if we should add to all of them without checking
/*					if(strstr(desc, "melee"))
						sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_START_ATTACK;
					if(strstr(desc, "ranged"))
						sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_START_ATTACK;*/
//				}
			}//end "if aura"
		}//end "for each effect"
		sp->procFlags=pr;

		if (strstr(desc, "Must remain seated"))
		{
			sp->RecoveryTime = 1000;
			sp->CategoryRecoveryTime = 1000;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// procintervals
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//omg lighning shield trigger spell id's are all wrong ?
		//if you are bored you could make thiese by hand but i guess we might find other spells with this problem..and this way it's safe
		if(strstr(nametext, "Lightning Shield") && sp->EffectTriggerSpell[0])
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "for $");
			if(startofid)
			{
				startofid += strlen("for $");
				sp->EffectTriggerSpell[0]=atoi(startofid); //get new lightning shield trigger id
			}
			sp->proc_interval = 3000; //few seconds
		}
		//mage ignite talent should proc only on some chances
		else if(strstr(nametext, "Ignite") && sp->Id>=11119 && sp->Id<=12848 && sp->EffectApplyAuraName[0]==4)
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "an additional ");
			if(startofid)
			{
				startofid += strlen("an additional ");
				sp->EffectBasePoints[0]=atoi(startofid); //get new value. This is actually level*8 ;)
			}
			sp->Effect[0] = 6; //aura
			sp->EffectApplyAuraName[0] = 42; //force him to use procspell effect
			sp->EffectTriggerSpell[0] = 12654; //evil , but this is good for us :D
			sp->procFlags = PROC_ON_SPELL_CRIT_HIT; //add procflag here since this was not processed with the others !
		}
		// Winter's Chill handled by frost school
		else if(strstr(nametext, "Winter's Chill"))
		{
			sp->School = 4;
		}
		// Blackout handled by Shadow school
		else if(strstr(nametext, "Blackout"))
		{
			sp->School = 5;
		}
		// Shadow Weaving
		else if(strstr(nametext, "Shadow Weaving"))
		{
			sp->School = 5;
			sp->EffectApplyAuraName[0] = 42;
			sp->procChance = sp->EffectBasePoints[0] + 1;
			sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
		}
		//Improved Aspect of the Hawk
		else if(strstr(nametext, "Improved Aspect of the Hawk"))
			sp->EffectSpellGroupRelation[1] = 0x100000;
		//more triggered spell ids are wrong. I think blizz is trying to outsmart us :S
		else if( strstr(nametext, "Nature's Guardian"))
		{
			sp->EffectTriggerSpell[0]=31616;
			sp->proc_interval = 5000;
		}
		//this starts to be an issue for trigger spell id : Deep Wounds
		else if(strstr(nametext, "Deep Wounds") && sp->EffectTriggerSpell[0])
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "over $");
			if(startofid)
			{
				startofid += strlen("over $");
				sp->EffectTriggerSpell[0]=atoi(startofid);
			}
		}
		else if(strstr(nametext, "Shred"))
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "damage plus ");
			if(startofid)
			{
				startofid += strlen("damage plus ");
				sp->EffectBasePoints[1]=atoi(startofid);
			}
		}
		else if(strstr(nametext, "Ravage"))
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "damage plus ");
			if(startofid)
			{
				startofid += strlen("damage plus ");
				sp->EffectBasePoints[1]=atoi(startofid);
			}
		}
		else if(strstr(nametext, "Mangle"))
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "damage plus ");
			if(startofid)
				sp->EffectBasePoints[1]=(sp->EffectBasePoints[0]+1)*(sp->EffectBasePoints[2]+1)/100;//kinda rough estimation no ? :P
		}
		else if(strstr(nametext, "Holy Shock"))
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "causing $");
			if(startofid)
			{
				startofid += strlen("causing $");
				sp->EffectTriggerSpell[0]=atoi(startofid);
			}
			//check if we can find in the desription
			startofid=strstr(desc, " or $");
			if(startofid)
			{
				startofid += strlen(" or $");
				sp->EffectTriggerSpell[1]=atoi(startofid);
			}
		}
		else if(strstr(nametext, "Touch of Weakness"))
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "cause $");
			if(startofid)
			{
				startofid += strlen("cause $");
				sp->EffectTriggerSpell[0]=atoi(startofid);
				sp->EffectTriggerSpell[1]=sp->EffectTriggerSpell[0]; //later versions of this spell changed to eff[1] the aura
				sp->procFlags = uint32(PROC_ON_MELEE_ATTACK_VICTIM | PROC_TAGRGET_SELF);
			}
		}
		else if(strstr(nametext, "Firestone Passive"))
		{
			//Enchants the main hand weapon with fire, granting each attack a chance to deal $17809s1 additional fire damage.
			//check if we can find in the desription
			char * startofid=strstr(desc, "to deal $");
			if(startofid)
			{
				startofid += strlen("to deal $");
				sp->EffectTriggerSpell[0]=atoi(startofid);
				sp->EffectApplyAuraName[0]=42;
				sp->procFlags = PROC_ON_MELEE_ATTACK;
				sp->procChance = 50;
			}
		}
		//some procs trigger at intervals
		else if(strstr(nametext, "Water Shield"))
		{
			sp->proc_interval = 3000; //few seconds
			sp->procFlags |= PROC_TAGRGET_SELF;
		}
		else if(strstr(nametext, "Earth Shield"))
			sp->proc_interval = 3000; //few seconds
		else if(strstr(nametext, "Shadowguard"))
			sp->proc_interval = 3000; //few seconds
		else if(strstr(nametext, "Poison Shield"))
			sp->proc_interval = 3000; //few seconds
		else if(strstr(nametext, "Infused Mushroom"))
			sp->proc_interval = 10000; //10 seconds
		else if(strstr(nametext, "Aviana's Purpose"))
			sp->proc_interval = 10000; //10 seconds
//		else if(strstr(nametext, "Illumination"))
//			sp->EffectTriggerSpell[0]=20272;
		//sp->dummy=result;
/*		//if there is a proc spell and has 0 as charges then it's probably going to triger infinite times. Better not save these
		if(sp->procCharges==0)
			sp->procCharges=-1;*/
		if(sp->proc_interval!=0)
			sp->procFlags |= PROC_REMOVEONUSE;

		/* Seal of Command - Proc Chance */
		if(sp->NameHash == 0xC5C30B39)
			sp->procChance = 25;
		
		/* Seal of Jusice - Proc Chance */
		if(sp->NameHash == 0xCC6D4182)
			sp->procChance = 25;

		/* Decapitate */
		if(sp->NameHash == 0xB6C3243C)
			sp->procChance = 30;

		/* hackfix for this - FIX ME LATER - Burlex */
		if(namehash==3238263755UL)
			sp->procFlags=0;

//junk code to get me has :P 
//if(sp->Id==11267 || sp->Id==11289 || sp->Id==6409)
//	printf("!!!!!!! name %s , id %u , hash %u \n",nametext,sp->Id, namehash);
	}
	//this is so lame : shamanistic rage triggers a new spell which borrows it's stats from parent spell :S
	SpellEntry * tsp;
	SpellEntry * parentsp;
	SpellEntry * triggersp;
	assign_Spell_to_SpellEntry(&parentsp,30823);
	assign_Spell_to_SpellEntry(&triggersp,30824);
	if(parentsp && triggersp) 
		triggersp->EffectBasePoints[0] = parentsp->EffectBasePoints[0];

	assign_Spell_to_SpellEntry(&sp,16164);
	if(sp && sp->Id==16164)
		sp->procFlags=PROC_ON_SPELL_CRIT_HIT_VICTIM;

	assign_Spell_to_SpellEntry(&sp,17364);
	//remove stormstrike effect 0
	//if(sp && sp->Id==17364)
		//sp->Effect[0]=0;

	//paladin - Blessing of Light
		//first let us get the 2 spellgrouprelations
	assign_Spell_to_SpellEntry(&sp,635);//holy light
	uint32 HL_grouprelation;
	if(sp)
		HL_grouprelation = sp->SpellGroupType;
	else HL_grouprelation=0;
	assign_Spell_to_SpellEntry(&sp,19750);//Flash of light
	uint32 FL_grouprelation;
	if(sp)
		FL_grouprelation = sp->SpellGroupType;
	else FL_grouprelation=0;

	assign_Spell_to_SpellEntry(&sp,19977);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 107;
		sp->EffectApplyAuraName[1] = 107;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectMiscValue[1] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0]=HL_grouprelation;
		sp->EffectSpellGroupRelation[1]=FL_grouprelation;
	}
	assign_Spell_to_SpellEntry(&sp,19978);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 107;
		sp->EffectApplyAuraName[1] = 107;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectMiscValue[1] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0]=HL_grouprelation;
		sp->EffectSpellGroupRelation[1]=FL_grouprelation;
	}
	assign_Spell_to_SpellEntry(&sp,19979);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 107;
		sp->EffectApplyAuraName[1] = 107;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectMiscValue[1] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0]=HL_grouprelation;
		sp->EffectSpellGroupRelation[1]=FL_grouprelation;
	}
	assign_Spell_to_SpellEntry(&sp,27144);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 107;
		sp->EffectApplyAuraName[1] = 107;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectMiscValue[1] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0]=HL_grouprelation;
		sp->EffectSpellGroupRelation[1]=FL_grouprelation;
	}
	assign_Spell_to_SpellEntry(&sp,32770);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 107;
		sp->EffectApplyAuraName[1] = 107;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectMiscValue[1] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0]=HL_grouprelation;
		sp->EffectSpellGroupRelation[1]=FL_grouprelation;
	}

	//paladin - seal of blood
	assign_Spell_to_SpellEntry(&sp,31892);
	if(sp)
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 31893;
	}
	assign_Spell_to_SpellEntry(&sp,38008);
	if(sp)
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 31893;
	}
	//paladin - Spiritual Attunement 
	assign_Spell_to_SpellEntry(&sp,31785);
	if(sp)
	{
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TAGRGET_SELF;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 31786;
	}
	assign_Spell_to_SpellEntry(&sp,33776);
	if(sp)
	{
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TAGRGET_SELF;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 31786;
	}
	//fix for the right Enchant ID for Enchant Cloak - Major Resistance
	assign_Spell_to_SpellEntry(&sp,27962);
	if(sp)
		sp->EffectMiscValue[0] = 2998;
	assign_Spell_to_SpellEntry(&sp,36285);
	if(sp)
		sp->EffectMiscValue[0] = 2998;

	//muhaha, rewriting Retaliation spell as old one :D
	assign_Spell_to_SpellEntry(&sp,20230);
	if(sp)
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = 42; //force him to use procspell effect
		sp->EffectTriggerSpell[0] = 22858; //evil , but this is good for us :D
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM; //add procflag here since this was not processed with the others !
	}

	//"bloodthirst" new version is ok but old version is wrong from now on :(
	assign_Spell_to_SpellEntry(&sp,23881);
	if(sp)
	{
		sp->Effect[1] = 64; //cast on us, it is good
		sp->EffectTriggerSpell[1] = 23885; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry(&sp,23892);
	if(sp)
	{
		sp->Effect[1] = 64;
		sp->EffectTriggerSpell[1] = 23886; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry(&sp,23893);
	if(sp)
	{
		sp->Effect[1] = 64; //
		sp->EffectTriggerSpell[1] = 23887; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry(&sp,23894);
	if(sp)
	{
		sp->Effect[1] = 64; //
		sp->EffectTriggerSpell[1] = 23888; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry(&sp,25251);
	if(sp)
	{
		sp->Effect[1] = 64; //aura
		sp->EffectTriggerSpell[1] = 25252; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry(&sp,30335);
	if(sp)
	{
		sp->Effect[1] = 64; //aura
		sp->EffectTriggerSpell[1] = 30339; //evil , but this is good for us :D
	}
	//Druid:Intensity
	assign_Spell_to_SpellEntry(&sp,17106);
	if(sp)
	{
	   sp->EffectApplyAuraName[1] = 42;
	   sp->procFlags=PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry(&sp,17107);
	if(sp)
	{
		sp->EffectApplyAuraName[1] = 42;
		 sp->procFlags=PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry(&sp,17108);
	if(sp)
	{
		sp->EffectApplyAuraName[1] = 42;
		sp->procFlags=PROC_ON_CAST_SPELL;
	}
    //Improved Sprint
	assign_Spell_to_SpellEntry(&sp,13743);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->procFlags=PROC_ON_CAST_SPELL;
		sp->procChance = 50;
	}
	assign_Spell_to_SpellEntry(&sp,13875);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->procFlags=PROC_ON_CAST_SPELL;
	}
	//warlock: Shadow Mastery
	for (uint32 i=0;i<5;i++)
	{
		assign_Spell_to_SpellEntry(&sp,18271+i);
		if (sp)
		{
			sp->EffectSpellGroupRelation[0]=33562624;
			sp->EffectSpellGroupRelation[1]=8421376;
		}
	}
	//mage: Arcane Power
	assign_Spell_to_SpellEntry(&sp,12042);
	if (sp)
	{
		sp->EffectSpellGroupRelation[0]=5775504;
		sp->EffectSpellGroupRelation[1]=10518528;
	}
	//mage: Fire Power
	assign_Spell_to_SpellEntry(&sp,11124);
	if (sp)
	{
		sp->EffectSpellGroupRelation[0]=868;
		sp->EffectSpellGroupRelation[1]=868;
	}
	assign_Spell_to_SpellEntry(&sp,12398);
	if (sp)
	{
		sp->EffectSpellGroupRelation[0]=868;
		sp->EffectSpellGroupRelation[1]=868;
	}
	assign_Spell_to_SpellEntry(&sp,12399);
	if (sp)
	{
		sp->EffectSpellGroupRelation[0]=868;
		sp->EffectSpellGroupRelation[1]=868;
	}
	assign_Spell_to_SpellEntry(&sp,12400);
	if (sp)
	{
		sp->EffectSpellGroupRelation[0]=868;
		sp->EffectSpellGroupRelation[1]=868;
	}
	assign_Spell_to_SpellEntry(&sp,12378);
	if (sp)
	{
		sp->EffectSpellGroupRelation[0]=868;
		sp->EffectSpellGroupRelation[1]=868;
	}
	////mage: Spell Power
	//sp = sSpellStore.LookupEntry(35581);
	//if(sp)	
	//{
	//	sp->EffectSpellGroupRelation[0]=5775504;
	//}
	//sp = sSpellStore.LookupEntry(35578);
	//if(sp)	
	//{
	//	sp->EffectSpellGroupRelation[0]=5775504;
	//}
	//mage: Blazing Speed
	assign_Spell_to_SpellEntry(&sp,31641); 
	if(sp)	sp->EffectTriggerSpell[0]=31643;
	assign_Spell_to_SpellEntry(&sp,31642);
	if(sp)	sp->EffectTriggerSpell[0]=31643;

	//mage talent "frostbyte". we make it to be dummy
	assign_Spell_to_SpellEntry(&sp,11071);
	if(sp)	sp->EffectApplyAuraName[0]=4;
	assign_Spell_to_SpellEntry(&sp,12496);
	if(sp)	sp->EffectApplyAuraName[0]=4;
	assign_Spell_to_SpellEntry(&sp,12497);
	if(sp)	sp->EffectApplyAuraName[0]=4;
	//rogue-shiv -> add 1 combo point
	assign_Spell_to_SpellEntry(&sp,5938);
	if(sp)	sp->Effect[1]=80;

	//warlock - soul leech
	assign_Spell_to_SpellEntry(&sp,30293);
	if(sp)
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 30294;
		sp->procFlags=uint32(PROC_ON_CAST_SPELL|PROC_TAGRGET_SELF);
	}
	assign_Spell_to_SpellEntry(&sp,30295);
	if(sp)
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 30294;
		sp->procFlags=uint32(PROC_ON_CAST_SPELL|PROC_TAGRGET_SELF);
	}
	assign_Spell_to_SpellEntry(&sp,30296);
	if(sp)
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 30294;
		sp->procFlags=uint32(PROC_ON_CAST_SPELL|PROC_TAGRGET_SELF);
	}

	//warlock - Pyroclasm
	assign_Spell_to_SpellEntry(&sp,18073);
	if(sp)
	{
		sp->Effect[0] = 0; //delete this owerride effect :P
		sp->EffectTriggerSpell[1] = 18093; //trigger spell was wrong :P
		sp->procFlags=PROC_ON_CAST_SPELL;
		sp->procChance = 13; //god, save us from fixed values !
	}
	assign_Spell_to_SpellEntry(&sp,18096);
	if(sp)
	{
		sp->Effect[0] = 0; //delete this owerride effect :P
		sp->EffectTriggerSpell[1] = 18093; //trigger spell was wrong :P
		sp->procFlags=PROC_ON_CAST_SPELL;
		sp->procChance = 26; //god, save us from fixed values !
	}

	//improved scorch
	assign_Spell_to_SpellEntry(&sp,11095);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 42; 
		sp->procFlags=PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry(&sp,12872);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 42; 
		sp->procFlags=PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry(&sp,12873);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 42; 
		sp->procFlags=PROC_ON_CAST_SPELL;
	}

	//Nature's Grasp
	assign_Spell_to_SpellEntry(&sp,16689);
	if(sp)
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 339; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry(&sp,16810);
	if(sp)
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 1062; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry(&sp,16811);
	if(sp)
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 5195; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry(&sp,16812);
	if(sp)
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 5196; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry(&sp,16813);
	if(sp)
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 9852; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry(&sp,17329);
	if(sp)
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 9853; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry(&sp,27009);
	if(sp)
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 26989; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	//wrath of air totem targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry(&sp,2895);
	if(sp)
	{
		sp->EffectImplicitTargetA[0]=1;
		sp->EffectImplicitTargetA[1]=1;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}
	//Relentless Strikes
	assign_Spell_to_SpellEntry(&sp,14179);
	if(sp)
	{
		sp->EffectApplyAuraName[0]=42;//proc spell
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectBasePoints[1] = 20; //client showes 20% chance but whe do not have it ? :O
	}
	//priest - surge of light
	assign_Spell_to_SpellEntry(&sp,33150);
	if(sp)
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT_VICTIM | PROC_TAGRGET_SELF);
	assign_Spell_to_SpellEntry(&sp,33154);
	if(sp)
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT_VICTIM | PROC_TAGRGET_SELF);
	assign_Spell_to_SpellEntry(&sp,33151);
	if(sp)
	{
		sp->EffectSpellGroupRelation[0]=128;
		sp->EffectSpellGroupRelation[1]=128;
		sp->EffectSpellGroupRelation[2]=128;
		sp->AuraInterruptFlags = AURA_INTERRUPT_ON_CAST_SPELL;
	}
	//Seal of Justice -lowered proc chance (experimental values !)
	assign_Spell_to_SpellEntry(&sp,20164);
	if(sp)
		sp->procChance = 20;
	assign_Spell_to_SpellEntry(&sp,31895);
	if(sp)
		sp->procChance = 20;
	//make Berserking a simple spell 
	assign_Spell_to_SpellEntry(&sp,20554);
	if(sp)
	{
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26635;
	}
	assign_Spell_to_SpellEntry(&sp,26296);
	if(sp)
	{
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26635;
	}
	assign_Spell_to_SpellEntry(&sp,26297);
	if(sp)
	{
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26635;
	}
	//rogue - intiative
	assign_Spell_to_SpellEntry(&sp,13976);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->procFlags=uint32(PROC_ON_CAST_SPELL|PROC_TAGRGET_SELF);
	}
	assign_Spell_to_SpellEntry(&sp,13979);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->procFlags=uint32(PROC_ON_CAST_SPELL|PROC_TAGRGET_SELF);
	}
	assign_Spell_to_SpellEntry(&sp,13980);
	if(sp)
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->procFlags=uint32(PROC_ON_CAST_SPELL|PROC_TAGRGET_SELF);
	}
	//this an on equip item spell(2824) :  ice arrow(29501)
	assign_Spell_to_SpellEntry(&sp,29501);
	if(sp)
	{
		sp->procChance = 30;//some say it is triggered every now and then
		sp->procFlags = PROC_ON_RANGED_ATTACK;
	}
	//warrior - berserker rage is missing 1 effect = regenerate rage
	assign_Spell_to_SpellEntry(&sp,18499);
	if(sp)
	{
		sp->Effect[2] = 6;
		sp->EffectApplyAuraName[2] = 42;
		sp->EffectTriggerSpell[2] = 9174; //not sure if this is the one. In my time this adds 30 rage
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TAGRGET_SELF;
	}
	//warrior - improved berserker rage
	assign_Spell_to_SpellEntry(&sp,20500);
	if(sp)
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TAGRGET_SELF;
	assign_Spell_to_SpellEntry(&sp,20501);
	if(sp)
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TAGRGET_SELF;
	//warrior - Rampage
	assign_Spell_to_SpellEntry(&sp,29801);
	if(sp)
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TAGRGET_SELF;
		sp->EffectTriggerSpell[0] = sp->EffectTriggerSpell[1];
	}
	assign_Spell_to_SpellEntry(&sp,30030);
	if(sp)
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TAGRGET_SELF;
		sp->EffectTriggerSpell[0] = sp->EffectTriggerSpell[1];
	}
	assign_Spell_to_SpellEntry(&sp,30033);
	if(sp)
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TAGRGET_SELF;
		sp->EffectTriggerSpell[0] = sp->EffectTriggerSpell[1];
	}
	// priest - Reflective Shield
	assign_Spell_to_SpellEntry(&sp,33201);
	if(sp)
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	assign_Spell_to_SpellEntry(&sp,33202);
	if(sp)
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	assign_Spell_to_SpellEntry(&sp,33203);
	if(sp)
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	assign_Spell_to_SpellEntry(&sp,33204);
	if(sp)
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	assign_Spell_to_SpellEntry(&sp,33205);
	if(sp)
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	// mage - Frost Warding
	assign_Spell_to_SpellEntry(&sp,11189);
	if(sp)
	{
		assign_Spell_to_SpellEntry(&tsp,168); //frost armor
		if(tsp)
			sp->EffectSpellGroupRelation[0] = tsp->SpellGroupType;
		assign_Spell_to_SpellEntry(&tsp,7302); //ice armor
		if(tsp)
			sp->EffectSpellGroupRelation[0] |= tsp->SpellGroupType;
	}
	assign_Spell_to_SpellEntry(&sp,28332);
	if(sp)
	{
		assign_Spell_to_SpellEntry(&tsp,168); //frost armor
		if(tsp)
			sp->EffectSpellGroupRelation[0] = tsp->SpellGroupType;
		assign_Spell_to_SpellEntry(&tsp,7302); //ice armor
		if(tsp)
			sp->EffectSpellGroupRelation[0] |= tsp->SpellGroupType;
//		sp->procChance = 100;
	}
	//Gnomish Poultryizer trinket - Poultryizer
	assign_Spell_to_SpellEntry(&sp,30507);
	if(sp)
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = 56;
	}
	// paladin - benediction
	uint32 judgement_group=0;
	assign_Spell_to_SpellEntry(&tsp,20271); //judgement
	if(tsp)
		judgement_group = tsp->SpellGroupType;
	assign_Spell_to_SpellEntry(&sp,20101);
	if(sp)
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | judgement_group;
	assign_Spell_to_SpellEntry(&sp,20102);
	if(sp)
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | judgement_group;
	assign_Spell_to_SpellEntry(&sp,20103);
	if(sp)
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | judgement_group;
	assign_Spell_to_SpellEntry(&sp,20104);
	if(sp)
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | judgement_group;
	assign_Spell_to_SpellEntry(&sp,20105);
	if(sp)
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | judgement_group;
	// paladin - Improved Hammer of Justice
	uint32 Hammer_of_Justice_group=0;
	assign_Spell_to_SpellEntry(&tsp,853); //Hammer of Justice
	if(tsp)
		Hammer_of_Justice_group = tsp->SpellGroupType;
	assign_Spell_to_SpellEntry(&sp,20487);
	if(sp)
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	assign_Spell_to_SpellEntry(&sp,20488);
	if(sp)
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	assign_Spell_to_SpellEntry(&sp,20489);
	if(sp)
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	assign_Spell_to_SpellEntry(&sp,24188);
	if(sp)
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	// paladin - Improved Judgement
	assign_Spell_to_SpellEntry(&sp,25956);
	if(sp)
		sp->EffectSpellGroupRelation[0] = judgement_group;
	assign_Spell_to_SpellEntry(&sp,25957);
	if(sp)
		sp->EffectSpellGroupRelation[0] = judgement_group;
	// paladin - Improved Sanctity Aura
	uint32 Sanctity_group=0;
	assign_Spell_to_SpellEntry(&tsp,20218); //Sanctity Aura
	if(tsp)
		Sanctity_group = tsp->SpellGroupType;
	assign_Spell_to_SpellEntry(&sp,31869);
	if(sp)
	{
		sp->EffectSpellGroupRelation[0] = Sanctity_group;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
	assign_Spell_to_SpellEntry(&sp,31870);
	if(sp)
	{
		sp->EffectSpellGroupRelation[0] = Sanctity_group;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}

	// druid - Improved Mark of the Wild
	uint32 imarkofthv_group=0;
	assign_Spell_to_SpellEntry(&tsp,1126); //Mark of the wild
	if(tsp)
		imarkofthv_group = tsp->SpellGroupType;
	assign_Spell_to_SpellEntry(&tsp,21849); //Gift of the Wild
	if(tsp)
		imarkofthv_group |= tsp->SpellGroupType;
	assign_Spell_to_SpellEntry(&sp,17050);
	if(sp)
		sp->EffectSpellGroupRelation[0] = imarkofthv_group;
	assign_Spell_to_SpellEntry(&sp,17051);
	if(sp)
		sp->EffectSpellGroupRelation[0] = imarkofthv_group;
	assign_Spell_to_SpellEntry(&sp,17053);
	if(sp)
		sp->EffectSpellGroupRelation[0] = imarkofthv_group;
	assign_Spell_to_SpellEntry(&sp,17054);
	if(sp)
		sp->EffectSpellGroupRelation[0] = imarkofthv_group;
	assign_Spell_to_SpellEntry(&sp,17055);
	if(sp)
		sp->EffectSpellGroupRelation[0] = imarkofthv_group;
	//rogue - initiative
	assign_Spell_to_SpellEntry(&sp,13976);
	if(sp)
		sp->procChance = 25;
	assign_Spell_to_SpellEntry(&sp,13979);
	if(sp)
		sp->procChance = 50;
	assign_Spell_to_SpellEntry(&sp,13980);
	if(sp)
		sp->procChance = 75;

	//for test only
/*	sp = dbcSpell.LookupEntry(32796);
	if(sp)
	{
//		printf("!!!!!!hash %u \n",sp->NameHash);
//		sp->procChance=100;
//		SpellDuration *sd=sSpellDuration.LookupEntry(sp->DurationIndex);
//printf("iterruptflag %u, duration %u",sp->AuraInterruptFlags,GetDuration(sd));
	}
	*/
	//improoved berserker stance should be triggered on berserker stance use
//	sp = sSpellStore.LookupEntry(12704);
//	if(sp)	sp->procFlags=PROC_ON_CAST_SPECIFIC_SPELL;

//	sp = sSpellStore.LookupEntry(16280);
//	if(sp)	printf("!!Interrupt flags %u interval %u charges %u\n",sp->AuraInterruptFlags,sp->proc_interval,sp->procCharges);
//	sp = sSpellStore.LookupEntry(16284);
//	if(sp)	printf("Interrupt flags %u\n",sp->AuraInterruptFlags);

	//fix for Predatory Strikes
	uint32 mm=(1<<(FORM_BEAR-1))|(1<<(FORM_DIREBEAR-1))|(1<<(FORM_MOONKIN-1))|(1<<(FORM_CAT-1));
	assign_Spell_to_SpellEntry(&sp,16972);
	if(sp)
		sp->RequiredShapeShift = mm;
	assign_Spell_to_SpellEntry(&sp,16974);
	if(sp)
		sp->RequiredShapeShift = mm;
	assign_Spell_to_SpellEntry(&sp,16972);
	if(sp)
		sp->RequiredShapeShift = mm;
	assign_Spell_to_SpellEntry(&sp,16975);
	if(sp)
		sp->RequiredShapeShift = mm;
	assign_Spell_to_SpellEntry(&sp,20134);
	if(sp)
		sp->procChance = 50;

	/* aspect of the pack - change to AA */
	assign_Spell_to_SpellEntry(&sp,13159);
	if(sp)
		sp->Effect[0] = SPELL_EFFECT_APPLY_AREA_AURA;
	assign_Spell_to_SpellEntry(&sp,13159);
	if(sp)
		sp->Effect[1] = SPELL_EFFECT_APPLY_AREA_AURA;
	
	/* shadowstep - change proc flags */
	assign_Spell_to_SpellEntry(&sp,36563);
	if(sp)
		sp->procChance = 50;

	dbc.DumpBufferToFile(inf);
}

void dump_as_sql(TCHAR *inf)
{
	if(SPELL_COLUMN_COUNT!=dbc.getFieldCount())
	{
		printf("error,column counts do not match update code for latest dbc format\n");
		exit(1);
	}

	printf("will start dumping data into sql file (we will drop not required fields!)\n");

	FILE *fsql=fopen("dbc_spell.sql","w");

	//drop table if already exist
	fprintf(fsql,"%s","DROP TABLE IF EXISTS `dbc_spell`;\n");

	//create the table
	fprintf(fsql,"%s","CREATE TABLE dbc_spell (\n");

	for(int i=0;i<SPELL_COLUMN_COUNT;i++)
		if(sql_translation_table[i][2][0]=='0')
		{
			if(sql_translation_table[i][0][0]=='u')
				fprintf(fsql,"\t `%s` INT (11) UNSIGNED DEFAULT '0' NOT NULL,\n",sql_translation_table[i][1]);
			else if(sql_translation_table[i][0][0]=='i')
				fprintf(fsql,"\t `%s` INT (11) DEFAULT '0' NOT NULL,\n",sql_translation_table[i][1]);
			else if(sql_translation_table[i][0][0]=='f')
				fprintf(fsql,"\t `%s` FLOAT DEFAULT '0' NOT NULL,\n",sql_translation_table[i][1]);
			else if(sql_translation_table[i][0][0]=='s')
				fprintf(fsql,"\t `%s` VARCHAR(2000),\n",sql_translation_table[i][1]);
		}

	fprintf(fsql,"%s","PRIMARY KEY(id), UNIQUE(id), INDEX(id));\n");

	fprintf(fsql,"\n\n");
	//start dumping the data from the DBC

	char tstr[2000];
	for(unsigned int j=0;j<dbc.getRecordCount();j++)
	{
		//we start a new insert block
		if((j % SQL_INSERTS_PER_QUERY) == 0)
		{
			fprintf(fsql,"%s","INSERT INTO dbc_spell (");
			for(int i=0;i<SPELL_COLUMN_COUNT-1;i++)
				if(sql_translation_table[i][2][0]=='0')
					fprintf(fsql,"`%s`,",sql_translation_table[i][1]);
			fprintf(fsql,"`%s`) values \n",sql_translation_table[SPELL_COLUMN_COUNT-1][1]);
			fprintf(fsql," (");
		}
		else
			fprintf(fsql,",(");
		for(int i=0;i<SPELL_COLUMN_COUNT;i++)
			if(sql_translation_table[i][2][0]=='0')
			{
				if(i!=0)
					fprintf(fsql,",");
				if(sql_translation_table[i][0][0]=='u')
					fprintf(fsql,"%u",dbc.getRecord(j).getUInt(i));
				else if(sql_translation_table[i][0][0]=='i')
					fprintf(fsql,"%d",dbc.getRecord(j).getInt(i));
				else if(sql_translation_table[i][0][0]=='f')
					fprintf(fsql,"%f",dbc.getRecord(j).getFloat(i));
				else if(sql_translation_table[i][0][0]=='s')
				{
					const char *dstr=dbc.getRecord(j).getString(i);
					int otherindex=0;
					for(unsigned int k=0;k<=strlen(dstr);k++)
						if(dstr[k]=='\'' || dstr[k]=='"')
						{
							tstr[otherindex++] = '\\';
							tstr[otherindex++] = dstr[k];
						}
						else
							tstr[otherindex++] = dstr[k];
					fprintf(fsql,"\"%s\"",tstr);
				}
			}
		//we need to end an insert block
		if(((j+1) % SQL_INSERTS_PER_QUERY)==0)
			fprintf(fsql,");\n");
		else fprintf(fsql,")\n");
	}
	fprintf(fsql,";");

	fprintf(fsql,"\n\n");

	fclose(fsql);
}


//this is not tested yet !!!
void import_from_sql()
{
	TCHAR mHostname[50];strcpy(mHostname,"");
	TCHAR mUsername[50];strcpy(mUsername,"");
	TCHAR mPassword[50];strcpy(mPassword,"");
	TCHAR mDatabaseName[50];strcpy(mDatabaseName,"");
	TCHAR mTableName[50];strcpy(mTableName,"");
	uint32 mPort=3306;
	TCHAR strbuffer[500];
	//read sql connection data
	FILE *inif=fopen("mysql_con.ini","r");
	if(!inif)
	{
		printf("error,Could not open mysql ini file\n");
		exit(1);
	}
	while (!feof(inif))
	{
		fgets (strbuffer , 100 , inif);
		if (strnicmp(strbuffer,"Hostname=",strlen("Hostname="))==0) strcpy(mHostname,strbuffer+strlen("Hostname="));
		else if (strnicmp(strbuffer,"Username=",strlen("Username="))==0) strcpy(mUsername,strbuffer+strlen("Username="));
		else if (strnicmp(strbuffer,"Password=",strlen("Password="))==0) strcpy(mPassword,strbuffer+strlen("Password="));
		else if (strnicmp(strbuffer,"DatabaseName=",strlen("DatabaseName="))==0) strcpy(mDatabaseName,strbuffer+strlen("DatabaseName="));
		else if (strnicmp(strbuffer,"TableName=",strlen("TableName="))==0) strcpy(mTableName,strbuffer+strlen("TableName="));
		else if (strnicmp(strbuffer,"Port=",strlen("Port="))==0) mPort=atoi(strbuffer+strlen("Port="));
	}
	fclose(inif);

	//establish mysql connection
	MYSQL* Descriptor = mysql_init(NULL);
	if(mysql_options(Descriptor, MYSQL_SET_CHARSET_NAME, "utf8"))
		printf("sql: Could not set utf8 character set [!!]");
	MYSQL *con = mysql_real_connect(Descriptor, mHostname,
		mUsername, mPassword, "", mPort, NULL, 0);
	if(con == NULL)
	{
		printf("sql: Connection failed. Reason was `%s`", mysql_error(Descriptor));
		return;
	}
	
	if(mysql_select_db(con, mDatabaseName))
	{
		printf("sql: Select of Database %s failed due to `%s`", mDatabaseName,mysql_error(con));
		return;
	}

	//start to get the lines we have in our dbc from the mysql table
	for(unsigned int j=0;j<dbc.getRecordCount();j++)
	{
		uint32 spell_id=dbc.getRecord(j).getUInt(SPELL_ENTRY_Id);

		//get the eqivalent of this dbc spell from mysql
		sprintf(strbuffer,"SELECT * FROM `%s` where id='%d'",mTableName,spell_id);
		int result = mysql_query(con, strbuffer);
		if(result > 0)
		{
			printf("Sql query failed due to [%s]", mysql_error(con));
			exit(1);
		}
        MYSQL_RES * Result = mysql_store_result(con);
        uint32 RowCount = (uint32)mysql_affected_rows(con);
        uint32 FieldCount = mysql_field_count(con);
        if(!RowCount || !FieldCount)
		{
			//empty result means we did not have this spell in sql
            mysql_free_result(Result); 
			continue;
		}
	    MYSQL_ROW row = mysql_fetch_row(Result);
		//now to set the values
		int *dbc_row; //dbc row does contain diferent data types but we don;t interpret values just simply set data
		int col_shifter=0;
		dbc_row=(int *)dbc.getRecord(j).getRowStart();
		for(int i=0;i<SPELL_COLUMN_COUNT;i++)
			if(sql_translation_table[i][2][0]=='0' && sql_translation_table[i][0][0]!='s')
				dbc_row[i] = (int)atoi(row[col_shifter++]); 
		mysql_free_result(Result); 
	}
}