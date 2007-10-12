<?php
include("include/header.inc.php");
?>
Aura 107 and 108 uses spell groups to influence their values. <br>
Since 1.12 spell groups are no longer available to be used so we need them in order to have functional spells<br>
Showing info about spell groups:<br>
<b>list of spells for aura 107 </b><br>
<?php	list_aura_info(107); ?>
<b>list of spells for aura 108</b><br>
<?php 	list_aura_info(108); ?>





<?php
function list_aura_info($aura_name)
{
	global $dbi;
	$get_query="select distinct(name),description,EffectSpellGroupRelation_1,EffectSpellGroupRelation_2,EffectSpellGroupRelation_3,EffectApplyAuraName_1,EffectApplyAuraName_2,EffectApplyAuraName_3 from dbc_spell where (EffectApplyAuraName_1=$aura_name or EffectApplyAuraName_2=$aura_name or EffectApplyAuraName_3=$aura_name) order by name desc";
	$get_res=mysql_query($get_query,$dbi) or die(" Q2007082018031 ".mysql_error($dbi)."<br>".$get_query);
	$might_work=0;
	$line_counter=0; 
	$not_used=0;
	?>
	<table border='1'>
		<tr>
			<td>number</td>
			<td>name</td>
			<td>desc</td>
			<td>works</td>
		</tr>
<?php
	while(list($name,$desc,$gr1,$gr2,$gr3,$a1,$a2,$a3)=mysql_fetch_row($get_res))
	{
		$works=0;
		if(($a1==$aura_name && $gr1!=0) || ($a2==$aura_name && $gr2!=0) || ($a3==$aura_name && $gr3!=0))
			$works=1;
		$might_work += $works;
		if(strlen($desc)<5)
			$not_used++;
		$line_counter++;
		?>
		<tr>
			<td><?=$line_counter?></td>
			<td><?=$name?></td>
			<td><?=$desc?></td>
			<td><?=$works?></td>
		</tr>
		<?php
	}
		?>
	</table>
	<?php
	echo "from these $might_work might work and ".($line_counter-$might_work)." might not and $not_used might not be used<br>";
}
?>