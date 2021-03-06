/*
 * Copyright 2010-2014 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "NewManufactureListState.h"
#include <algorithm>
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleManufacture.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Production.h"
#include "../Savegame/Base.h"
#include "ManufactureStartState.h"
#include "../Menu/ErrorMessageState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the productions list screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
NewManufactureListState::NewManufactureListState(Base *base) : _base(base)
{
	_screen = false;

	_window = new Window(this, 320, 140, 0, 30, POPUP_BOTH);
	_btnOk = new TextButton(304, 16, 8, 146);
	_txtTitle = new Text(320, 17, 0, 38);
	_txtItem = new Text(156, 9, 10, 54);
	_txtCategory = new Text(130, 9, 166, 54);
	_lstManufacture = new TextList(288, 80, 8, 62);

	// Set palette
	setPalette("PAL_BASESCAPE", 6);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtItem);
	add(_txtCategory);
	add(_lstManufacture);

	centerAllSurfaces();

	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));
	_txtTitle->setColor(Palette::blockOffset(15)+1);
	_txtTitle->setText(tr("STR_PRODUCTION_ITEMS"));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtItem->setColor(Palette::blockOffset(15)+1);
	_txtItem->setText(tr("STR_ITEM"));

	_txtCategory->setColor(Palette::blockOffset(15)+1);
	_txtCategory->setText(tr("STR_CATEGORY"));

	_lstManufacture->setColumns(2, 156, 130);
	_lstManufacture->setSelectable(true);
	_lstManufacture->setBackground(_window);
	_lstManufacture->setMargin(2);
	_lstManufacture->setColor(Palette::blockOffset(13));
	_lstManufacture->setArrowColor(Palette::blockOffset(15)+1);
	_lstManufacture->onMouseClick((ActionHandler)&NewManufactureListState::lstProdClick);

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewManufactureListState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NewManufactureListState::btnOkClick, Options::keyCancel);
}

/**
 * Initializes state (fills list of possible productions).
 */
void NewManufactureListState::init()
{
	State::init();
	fillProductionList();
}

/**
 * Returns to the previous screen.
 * @param action A pointer to an Action.
 */
void NewManufactureListState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Opens the Production settings screen.
 * @param action A pointer to an Action.
*/
void NewManufactureListState::lstProdClick(Action *)
{
	RuleManufacture *rule = _possibleProductions[_lstManufacture->getSelectedRow()];
	if (rule->getCategory() == "STR_CRAFT" && _base->getAvailableHangars() - _base->getUsedHangars() == 0)
	{
		_game->pushState(new ErrorMessageState(tr("STR_NO_FREE_HANGARS_FOR_CRAFT_PRODUCTION"), _palette, Palette::blockOffset(15)+1, "BACK17.SCR", 6));
	}
	else if (rule->getRequiredSpace() > _base->getFreeWorkshops())
	{
		_game->pushState(new ErrorMessageState(tr("STR_NOT_ENOUGH_WORK_SPACE"), _palette, Palette::blockOffset(15)+1, "BACK17.SCR", 6));
	}
	else
	{
		_game->pushState(new ManufactureStartState(_base, rule));
	}
}


/**
 * Fills the list of possible productions.
 */
void NewManufactureListState::fillProductionList()
{
	_lstManufacture->clearList();
	_possibleProductions.clear();
	_game->getSavedGame()->getAvailableProductions(_possibleProductions, _game->getRuleset(), _base);

	for (std::vector<RuleManufacture *>::iterator it = _possibleProductions.begin (); it != _possibleProductions.end (); ++it)
	{
		_lstManufacture->addRow(2, tr((*it)->getName()).c_str(), tr((*it)->getCategory ()).c_str());
	}
}

}
