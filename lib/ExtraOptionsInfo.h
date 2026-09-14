/*
 * ExtraOptionsInfo.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

struct DLL_LINKAGE ExtraOptionsInfo
{
	bool cheatsAllowed = true;
	bool unlimitedReplay = false;
	/// if set, client stores every received netpack on disk so the whole game can be replayed later
	bool recordGame = false;
	/// if set, town popup shows spells of every mage guild level, including levels that are not built yet
	bool revealMageGuildSpells = false;
	/// if set, rewards that are normally hidden (Pandora's Box, creature banks) are shown on right click
	bool revealHiddenRewards = false;
	/// if set, monster popup shows how the stack splits for battle and whether it joins, flees or fights the selected hero
	bool revealMonsterInfo = false;
	/// if set, invisible map events are marked on the adventure map and their contents are shown on right click
	bool revealHiddenEvents = false;
	/// if set, enemy hero popups show exact army and stats, and Alt + right click opens the full hero screen
	bool revealEnemyHeroes = false;

	bool operator == (const ExtraOptionsInfo & other) const = default;

	template <typename Handler>
	void serialize(Handler &h)
	{
		h & cheatsAllowed;
		h & unlimitedReplay;
		if(h.hasFeature(Handler::Version::GAME_REPLAY_RECORDING))
			h & recordGame;
		else
			recordGame = false;

		if(h.hasFeature(Handler::Version::REVEAL_EXTRA_OPTIONS))
		{
			h & revealMageGuildSpells;
			h & revealHiddenRewards;
		}
		else
		{
			revealMageGuildSpells = false;
			revealHiddenRewards = false;
		}

		if(h.hasFeature(Handler::Version::REVEAL_MONSTER_INFO))
			h & revealMonsterInfo;
		else
			revealMonsterInfo = false;

		if(h.hasFeature(Handler::Version::REVEAL_HIDDEN_EVENTS))
			h & revealHiddenEvents;
		else
			revealHiddenEvents = false;

		if(h.hasFeature(Handler::Version::REVEAL_ENEMY_HEROES))
			h & revealEnemyHeroes;
		else
			revealEnemyHeroes = false;
	}
};
