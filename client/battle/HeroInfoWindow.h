/*
 * HeroInfoWindow.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "../windows/CWindowObject.h"

#include "BattleSidePanel.h"

class CLabel;
class CAnimImage;
class CGHeroInstance;

struct InfoAboutHero;

class HeroInfoBasicPanel : public BattleSidePanel //extracted from InfoWindow to fit better as non-popup embed element
{
private:
	std::shared_ptr<CPicture> background;
	std::vector<std::shared_ptr<CLabel>> labels;
	std::vector<std::shared_ptr<CAnimImage>> icons;
	const CGHeroInstance * revealedHero = nullptr;

public:
	HeroInfoBasicPanel(const InfoAboutHero & hero, const Point * position, bool initializeBackground = true);

	void initializeData(const InfoAboutHero & hero);
	void update(const InfoAboutHero & updatedInfo);
	/// right click on the panel opens the full hero screen of this hero
	void revealHero(const CGHeroInstance * hero);
	void showPopupWindow(const Point & cursorPosition) override;
};

class HeroInfoWindow : public CWindowObject
{
private:
	std::shared_ptr<HeroInfoBasicPanel> content;

public:
	HeroInfoWindow(const InfoAboutHero & hero, const Point * position);
};
