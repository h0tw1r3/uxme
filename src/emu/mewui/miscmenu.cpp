// license:BSD-3-Clause
// copyright-holders:Dankan1890
/***************************************************************************

    mewui/miscmenu.cpp

    MEWUI miscellaneous options menu.

***************************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "ui/menu.h"
#include "mewui/miscmenu.h"
#include "mewui/utils.h"
#include <limits>

ui_menu_misc_options::misc_option ui_menu_misc_options::m_options[] = {
	{ nullptr, nullptr },
	{ "Re-select last machine played",                   OPTION_REMEMBER_LAST },
	{ "Enlarge images in the right panel",               OPTION_ENLARGE_SNAPS },
	{ "DATs info",                                       OPTION_DATS_ENABLED },
	{ "Cheats",                                          OPTION_CHEAT },
	{ "Show mouse pointer",                              OPTION_UI_MOUSE },
	{ "Confirm quit from machines",                      OPTION_CONFIRM_QUIT },
	{ "Skip displaying information's screen at startup", OPTION_SKIP_GAMEINFO },
	{ "Skip displaying disclaimer screen at startup",    OPTION_SKIP_DISCLAIMER },
	{ "Skip displaying warning screen at startup",       OPTION_SKIP_WARNINGS },
	{ "Skip displaying loading messages at startup",     OPTION_SKIP_LOADING },
	{ "Render white border on UI message screens",       OPTION_RENDER_BORDER },
	{ "Force 4:3 appearance for software snapshot",      OPTION_FORCED4X3 },
	{ "Use image as background",                         OPTION_USE_BACKGROUND },
	{ "Skip bios selection menu",                        OPTION_SKIP_BIOS_MENU },
	{ "Skip software parts selection menu",              OPTION_SKIP_PARTS_MENU }
};

//-------------------------------------------------
//  ctor / dtor
//-------------------------------------------------

ui_menu_misc_options::ui_menu_misc_options(running_machine &machine, render_container *container) : ui_menu(machine, container)
{
}

ui_menu_misc_options::~ui_menu_misc_options()
{
}

//-------------------------------------------------
//  handlethe options menu
//-------------------------------------------------

void ui_menu_misc_options::handle()
{
	bool changed = false;
	emu_options::entry *entry;
	emu_options &mopts = machine().options();
	std::string error_string, tmptxt;
	int i_cur;
	float f_cur, f_step;

	// process the menu
	const ui_menu_event *m_event = process(0);
	if (m_event != nullptr && m_event->itemref != nullptr)
	{
		if (m_event->iptkey == IPT_UI_LEFT || m_event->iptkey == IPT_UI_RIGHT)
		{
			FPTR d = (FPTR)m_event->itemref;
			entry = mopts.find(m_options[d].name);

			switch (entry->type())
			{
				case OPTION_BOOLEAN:
					changed = true;
					mopts.set_value(m_options[d].name, !strcmp(entry->value(),"1") ? "0" : "1", OPTION_PRIORITY_CMDLINE, error_string);
					break;
				case OPTION_INTEGER:
					changed = true;
					i_cur = atof(entry->value());
					(m_event->iptkey == IPT_UI_LEFT) ? i_cur-- : i_cur++;
					mopts.set_value(m_options[d].name, i_cur, OPTION_PRIORITY_CMDLINE, error_string);
					break;
				case OPTION_FLOAT:
					changed = true;
					f_cur = atof(entry->value());
					if (entry->has_range())
					{
						f_step = atof(entry->minimum());
						if (f_step <= 0.0f) {
							int pmin = getprecisionchr(entry->minimum());
							int pmax = getprecisionchr(entry->maximum());
							tmptxt = '1' + std::string((pmin > pmax) ? pmin : pmax, '0');
							f_step = 1 / atof(tmptxt.c_str());
						}
					}
					else
					{
						int precision = getprecisionchr(entry->default_value());
						tmptxt = '1' + std::string(precision, '0');
						f_step = 1 / atof(tmptxt.c_str());
					}
					if (m_event->iptkey == IPT_UI_LEFT)
						f_cur -= f_step;
					else
						f_cur += f_step;
					strprintf(tmptxt, "%g", f_cur);
					mopts.set_value(m_options[d].name, tmptxt.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
					break;
			}
		}
	}

	if (changed)
		reset(UI_MENU_RESET_REMEMBER_REF);
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_menu_misc_options::populate()
{
	UINT32 arrow_flags;
	emu_options::entry *entry;
	emu_options &mopts = machine().options();
	std::string tmptxt;
	float f_min, f_max, f_cur;
	int i_min, i_max, i_cur;

	// add options
	for (int d = 1; d < ARRAY_LENGTH(m_options); ++d)
	{
		entry = mopts.find(m_options[d].name);

		switch (entry->type())
		{
			case OPTION_BOOLEAN:
				arrow_flags = mopts.bool_value(m_options[d].name) ? MENU_FLAG_RIGHT_ARROW : MENU_FLAG_LEFT_ARROW;
				item_append(m_options[d].description,
						(arrow_flags == MENU_FLAG_RIGHT_ARROW) ? "On" : "Off",
						arrow_flags,
						(void *)(FPTR)d);
				break;
			case OPTION_INTEGER:
				i_cur = atof(entry->value());
				if (entry->has_range())
				{
					i_min = atof(entry->minimum());
					i_max = atof(entry->maximum());
				}
				else
				{
					i_min = 0;
					i_max = std::numeric_limits<int>::max();
				}
				arrow_flags = get_arrow_flags(i_min, i_max, i_cur);
				item_append(m_options[d].description,
						entry->value(),
						arrow_flags,
						(void *)(FPTR)d);
				break;
			case OPTION_FLOAT:
				f_cur = atof(entry->value());
				if (entry->has_range())
				{
					f_min = atof(entry->minimum());
					f_max = atof(entry->maximum());
				}
				else
				{
					f_min = 0.0f;
					f_max = std::numeric_limits<float>::max();
				}
				arrow_flags = get_arrow_flags(f_min, f_max, f_cur);
				strprintf(tmptxt, "%g", f_cur);
				item_append(m_options[d].description,
						tmptxt.c_str(),
						arrow_flags,
						(void *)(FPTR)d);
				break;
			default:
				arrow_flags = MENU_FLAG_RIGHT_ARROW;
				item_append(m_options[d].description,
					machine().options().value(m_options[d].name),
					arrow_flags, (void *)(FPTR)d);
				break;
		}
	}

	item_append(MENU_SEPARATOR_ITEM, nullptr, 0, nullptr);
	customtop = machine().ui().get_line_height() + (3.0f * UI_BOX_TB_BORDER);
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void ui_menu_misc_options::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	float width;
	ui_manager &mui = machine().ui();

	mui.draw_text_full(container, "Miscellaneous Options", 0.0f, 0.0f, 1.0f, JUSTIFY_CENTER, WRAP_TRUNCATE,
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
	mui.draw_text_full(container, "Miscellaneous Options", x1, y1, x2 - x1, JUSTIFY_CENTER, WRAP_TRUNCATE,
	                              DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, nullptr, nullptr);
}
