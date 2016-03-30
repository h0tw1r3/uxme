// license:BSD-3-Clause
// copyright-holders:h0tw1r3
/***************************************************************************

    ui/submenu.h

    UI options menu.

***************************************************************************/
#pragma once

#ifndef __UI_SUBMENU_H__
#define __UI_SUBMENU_H__

#include "ui/menu.h"

struct ui_submenu_option
{
	const char  *description;
	const char  *name;
	core_options::entry *entry;
	core_options (*options);
};

static std::vector<ui_submenu_option> perf_submenu_options = {
	{ "Performance Options",                     nullptr },
	{ "Auto frame skip",                         OPTION_AUTOFRAMESKIP },
	{ "Frame skip",                              OPTION_FRAMESKIP },
	{ "Throttle",                                OPTION_THROTTLE },
	{ "Sleep",                                   OPTION_SLEEP },
	{ "Speed",                                   OPTION_SPEED },
	{ "Refresh speed",                           OPTION_REFRESHSPEED },
};

static std::vector<ui_submenu_option> rotate_submenu_options = {
	{ "Rotation Options",                        nullptr },
	{ "Rotate",                                  OPTION_ROTATE },
	{ "Rotate right",                            OPTION_ROR },
	{ "Rotate left",                             OPTION_ROL },
	{ "Auto rotate right",                       OPTION_AUTOROR },
	{ "Auto rotate left",                        OPTION_AUTOROL },
	{ "Flip X",                                  OPTION_FLIPX },
	{ "Flip Y",                                  OPTION_FLIPY },
};

static std::vector<ui_submenu_option> artwork_submenu_options = {
	{ "Artwork Options",                         nullptr },
	{ "Artwork Crop",                            OPTION_ARTWORK_CROP },
	{ "Use Backdrops",                           OPTION_USE_BACKDROPS },
	{ "Use Overlays",                            OPTION_USE_OVERLAYS },
	{ "Use Bezels",                              OPTION_USE_BEZELS },
	{ "Use Control Panels",                      OPTION_USE_CPANELS },
	{ "Use Marquees",                            OPTION_USE_MARQUEES },
};

static std::vector<ui_submenu_option> state_submenu_options = {
	{ "State/Playback Options",                  nullptr },
	{ "Automatic save/restore",                  OPTION_AUTOSAVE },
	{ "Bilinear snapshot",                       OPTION_SNAPBILINEAR },
	{ "Burn-in",                                 OPTION_BURNIN },
};

static std::vector<ui_submenu_option> input_submenu_options = {
	{ "Input Options",                           nullptr },
	{ "Coin lockout",                            OPTION_COIN_LOCKOUT },
	{ "Mouse",                                   OPTION_MOUSE },
	{ "Joystick",                                OPTION_JOYSTICK },
	{ "Lightgun",                                OPTION_LIGHTGUN },
	{ "Multi-keyboard",                          OPTION_MULTIKEYBOARD },
	{ "Multi-mouse",                             OPTION_MULTIMOUSE },
	{ "Steadykey",                               OPTION_STEADYKEY },
	{ "UI active",                               OPTION_UI_ACTIVE },
	{ "Offscreen reload",                        OPTION_OFFSCREEN_RELOAD },
	{ "Joystick deadzone",                       OPTION_JOYSTICK_DEADZONE },
	{ "Joystick saturation",                     OPTION_JOYSTICK_SATURATION },
	{ "Natural keyboard",                        OPTION_NATURAL_KEYBOARD },
	{ "Simultaneous contradictory",              OPTION_JOYSTICK_CONTRADICTORY },
	{ "Coin impulse",                            OPTION_COIN_IMPULSE },
};

static std::vector<ui_submenu_option> misc_submenu_options = {
	{ "Miscellaneous Options",                           nullptr },
	{ "Re-select last machine played",                   OPTION_REMEMBER_LAST },
	{ "Enlarge images in the right panel",               OPTION_ENLARGE_SNAPS },
	{ "DATs info",                                       OPTION_DATS_ENABLED },
	{ "Cheats",                                          OPTION_CHEAT },
	{ "Show mouse pointer",                              OPTION_UI_MOUSE },
	{ "Confirm quit from machines",                      OPTION_CONFIRM_QUIT },
	{ "Skip displaying information's screen at startup", OPTION_SKIP_GAMEINFO },
	{ "Force 4:3 appearance for software snapshot",      OPTION_FORCED4X3 },
	{ "Use image as background",                         OPTION_USE_BACKGROUND },
	{ "Skip bios selection menu",                        OPTION_SKIP_BIOS_MENU },
	{ "Skip software parts selection menu",              OPTION_SKIP_PARTS_MENU },
};

//-------------------------------------------------
//  class ui menu
//-------------------------------------------------
class ui_submenu : public ui_menu
{
public:
	ui_submenu(running_machine &machine, render_container *container, std::vector<ui_submenu_option> &suboptions);
	virtual ~ui_submenu();
	virtual void populate() override;
	virtual void handle() override;
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

private:
	std::vector<ui_submenu_option> &m_options;
};

#endif /* __UI_SUBMENU_H__ */
