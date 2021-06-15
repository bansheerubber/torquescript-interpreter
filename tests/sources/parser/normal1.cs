datablock TSShapeConstructor(MiniDungeonsDTS) {
	baseShape = "base/data/shapes/player/minidungeons.dts";
	sequence0 = "base/data/shapes/player/default.dsq";
	sequence1 = "base/data/shapes/player/minidungeons.dsq";
};

datablock PlayerData(MiniDungeonsArmor : PlayerStandardArmor)  {
	shapeFile = MiniDungeonsDTS.baseShape;
	uiName = "Mini Dungeons Player";

	minImpactSpeed = 500;

	canJet = false;
	isMiniDungeons = true;
};

function Player::dungeonsFixAppearance(%obj, %client) {	
	if(%obj.getClassName() $= "AiPlayer") {
		return;
	}
	
	%obj.hideNode("ALL");
	%obj.unHideNode((%client.chest ? "femChest" : "chest"));
	
	%obj.unHideNode((%client.rhand ? "rhook" : "rhand"));
	%obj.unHideNode((%client.lhand ? "lhook" : "lhand"));
	%obj.unHideNode((%client.rarm ? "rarmSlim" : "rarm"));
	%obj.unHideNode((%client.larm ? "larmSlim" : "larm"));
	%obj.unHideNode("headskin");
	if($pack[%client.pack] !$= "none") {
		%obj.unHideNode($pack[%client.pack]);
		%obj.setNodeColor($pack[%client.pack], %client.packColor);
	}

	if($secondPack[%client.secondPack] !$= "none") {
		%obj.unHideNode($secondPack[%client.secondPack]);
		%obj.setNodeColor($secondPack[%client.secondPack], %client.secondPackColor);
	}

	if($hat[%client.hat] !$= "none" && %hatmodHat $= "") {
		%obj.unHideNode($hat[%client.hat]);
		%obj.setNodeColor($hat[%client.hat], %client.hatColor);
	}

	if(%client.hip) {
		%obj.unHideNode("skirthip");
		%obj.unHideNode("skirttrimleft");
		%obj.unHideNode("skirttrimright");
	}
	else {
		%obj.unHideNode("pants");
		%obj.unHideNode((%client.rleg ? "rpeg" : "rshoe"));
		%obj.unHideNode((%client.lleg ? "lpeg" : "lshoe"));
	}
	%obj.setHeadUp(0);

	if(%client.pack + %client.secondPack > 0) {
		%obj.setHeadUp(1);
	}

	if($hat[%client.hat] $= "Helmet") {
		if(%client.accent == 1 && $accent[4] !$= "none") {
			%obj.unHideNode($accent[4]);
			%obj.setNodeColor($accent[4], %client.accentColor);
		}
	}
	else if($accent[%client.accent] !$= "none" && strPos($accentsAllowed[$hat[%client.hat]], strLwr($accent[%client.accent])) != -1) {
		%obj.unHideNode($accent[%client.accent]);
		%obj.setNodeColor($accent[%client.accent], %client.accentColor);
	}

	%obj.setFaceName(%client.faceName);
	%obj.setDecalName(%client.decalName);
	
	%obj.setNodeColor("headskin", %client.headColor);
	
	%obj.setNodeColor("chest", %client.chestColor);
	%obj.setNodeColor("femChest", %client.chestColor);
	%obj.setNodeColor("pants", %client.hipColor);
	%obj.setNodeColor("skirthip", %client.hipColor);
	
	%obj.setNodeColor("rarm", %client.rarmColor);
	%obj.setNodeColor("larm", %client.larmColor);
	%obj.setNodeColor("rarmSlim", %client.rarmColor);
	%obj.setNodeColor("larmSlim", %client.larmColor);
	
	%obj.setNodeColor("rhand", %client.rhandColor);
	%obj.setNodeColor("lhand", %client.lhandColor);
	%obj.setNodeColor("lhand2", %client.lhandColor);
	%obj.setNodeColor("rhook", %client.rhandColor);
	%obj.setNodeColor("lhook", %client.lhandColor);
	%obj.setNodeColor("rhandVielle", %client.lhandColor);
	%obj.setNodeColor("lhandVielle", %client.lhandColor);

	%obj.setNodeColor("whirl", "1 1 1 1");
	
	%obj.setNodeColor("rshoe", %client.rlegColor);
	%obj.setNodeColor("lshoe", %client.llegColor);
	%obj.setNodeColor("rpeg", %client.rlegColor);
	%obj.setNodeColor("lpeg", %client.llegColor);
	%obj.setNodeColor("skirttrimright", %client.rlegColor);
	%obj.setNodeColor("skirttrimleft", %client.llegColor);

	if(%obj.hasVielle) {
		%obj.unHideNode("vielle");
		%obj.unHideNode("rHandVielle");
		%obj.unHideNode("lHandVielle");
		%obj.unHideNode("crank");
		%obj.unHideNode("whirl");

		%obj.hideNode("rhand");
		%obj.hideNode("lhand");
	}

	if(%obj.isAlligatorSlayer) {
		%obj.unHideNode("demonSlayer");
		%obj.unHideNode("demonSlayerDoor");
		%obj.unHideNode("demonSlayerStraps");
	}

	if(isObject(%obj.swordHands)) {
		%obj.swordHands.hideNode("ALL");

		%obj.swordHands.setNodeColor("rhand", %client.rhandColor);
		%obj.swordHands.setNodeColor("lhand", %client.lhandColor);
	}

	if(%obj.hideRightHand || isObject(%sword = %obj.sword[0])) {
		if(%obj.forceNormalHands) {
			if(%sword.getDatablock().swordDoubleHanded) {
				%obj.hideNode("lhand");
				%obj.unHideNode("lhand2");
			}
		}
		else {
			if(!%obj.hideRightHand && %sword.getDatablock().swordDoubleHanded) {
				%obj.hideNode("rhand");
				%obj.hideNode("lhand");

				%obj.swordHands.unHideNode("rhand");
				%obj.swordHands.unHideNode("lhand");
			}
			else {
				%obj.hideNode("rhand");
				%obj.swordHands.unHideNode("rhand");
			}
		}
	}
}

function Player::unHideVielle(%this) {
	%this.hasVielle = true;
	%this.dungeonsFixAppearance(%this.client);
}

deActivatePackage(MiniDungeonsPlayer);
package MiniDungeonsPlayer {
	function GameConnection::applyBodyColors(%client, %obj)  {
		Parent::applyBodyColors(%client, %obj);

		if(isObject(%player = %client.player)) {
			if(%player.getDatablock().isMiniDungeons) {
				%player.dungeonsFixAppearance(%client);
			}
		}
	}

	function GameConnection::applyBodyParts(%client, %obj)  {
		Parent::applyBodyParts(%client, %obj);

		if(isObject(%player = %client.player)) {
			if(%player.getDatablock().isMiniDungeons) {
				%player.dungeonsFixAppearance(%client);
			}
		}
	}

	function GameConnection::spawnPlayer(%this) {
		Parent::spawnPlayer(%this);
		// %this.player.setDatablock(MiniDungeonsArmor);
	}
};
activatePackage(MiniDungeonsPlayer);