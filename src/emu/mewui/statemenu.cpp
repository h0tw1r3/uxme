// license:BSD-3-Clause
// copyright-holders:Dankan1890
/***************************************************************************

    mewui/statemenu.cpp

    MEWUI stateellaneous options menu.

***************************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "ui/menu.h"
#include "mewui/statemenu.h"
#include "mewui/utils.h"

ui_menu_state_options::state_option ui_menu_state_options::m_options[] = {
	{ 0, nullptr, nullptr },
	{ 0, "High score support",                              OPTION_HISCORE },
	{ 0, "Automatic machine state save and restore",        OPTION_AUTOSAVE },
	{ 0, "Bilinear snapshot filtering",                     OPTION_SNAPBILINEAR },
	{ 0, "Burn-in snapshots",                               OPTION_BURNIN },
};

//-------------------------------------------------
//  ctor / dtor
//-------------------------------------------------

ui_menu_state_options::ui_menu_state_options(running_machine &machine, render_container *container) : ui_menu(machine, container)
{
	for (int d = 1; d < ARRAY_LENGTH(m_options); ++d)
		m_options[d].status = machine.options().bool_value(m_options[d].option);
}

ui_menu_state_options::~ui_menu_state_options()
{
	std::string error_string;
	for (int d = 1; d < ARRAY_LENGTH(m_options); ++d)
		machine().options().set_value(m_options[d].option, m_options[d].status, OPTION_PRIORITY_CMDLINE, error_string);
	mewui_globals::reset = true;
}

//-------------------------------------------------
//  handlethe options menu
//-------------------------------------------------

void ui_menu_state_options::handle()
{
	bool changed = false;

	// process the menu
	const ui_menu_event *m_event = process(0);
	if (m_event != nullptr && m_event->itemref != nullptr)
	{
		if (m_event->iptkey == IPT_UI_LEFT || m_event->iptkey == IPT_UI_RIGHT || m_event->iptkey == IPT_UI_SELECT)
		{
			changed = true;
			int value = (FPTR)m_event->itemref;
			if (!strcmp(m_options[value].option, OPTION_ENLARGE_SNAPS))
				mewui_globals::switch_image = true;
			m_options[value].status = !m_options[value].status;
		}
	}

	if (changed)
		reset(UI_MENU_RESET_REMEMBER_REF);
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_menu_state_options::populate()
{
	// add options items
	for (int opt = 1; opt < ARRAY_LENGTH(m_options); ++opt)
		item_append(m_options[opt].description, m_options[opt].status ? "On" : "Off", m_options[opt].status ? MENU_FLAG_RIGHT_ARROW : MENU_FLAG_LEFT_ARROW, (void *)(FPTR)opt);

	item_append(MENU_SEPARATOR_ITEM, nullptr, 0, nullptr);
	customtop = machine().ui().get_line_height() + (3.0f * UI_BOX_TB_BORDER);
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void ui_menu_state_options::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	float width;
	ui_manager &mui = machine().ui();

	mui.draw_text_full(container, "State/Playback Options", 0.0f, 0.0f, 1.0f, JUSTIFY_CENTER, WRAP_TRUNCATE,
	                              DRAW_NONE, ARGB_WHITE, ARGB_BLACK, &width, nullptr);
	width += 2 * UI_BOX_LR_BORDER;
	float maxwidth = MAX(origx2 - origx1, width);

	// compute our bounds
	float x1 = 0.5f - 0.5f * maxwidth;
	float x2 = x1 + maxwidth;
	float y1 = origy1 - top;
	float y2 = origy1 - UI_BOX_TB_BORDER;

	// draw a box
	mui.draw_outlined_box(container, x1, y1, x2, y2, UI_GREEN_COLOR);

	// take off the borders
	x1 += UI_BOX_LR_BORDER;
	x2 -= UI_BOX_LR_BORDER;
	y1 += UI_BOX_TB_BORDER;

	// draw the text within it
	mui.draw_text_full(container, "State/Playback Options", x1, y1, x2 - x1, JUSTIFY_CENTER, WRAP_TRUNCATE,
	                              DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, nullptr, nullptr);
}
