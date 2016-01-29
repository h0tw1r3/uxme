// license:BSD-3-Clause
// copyright-holders:Dankan1890,Jeffrey Clark
/*********************************************************************

    mewui/perfmenu.cpp

    MEWUI performance options menu.

*********************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "ui/menu.h"
#include "mewui/perfmenu.h"
#include <limits>

ui_menu_perf_menu::perf_option ui_menu_perf_menu::m_options[] = {
	{ nullptr, nullptr },
	{ "Automatic frame skipping",                OPTION_AUTOFRAMESKIP },
	{ "Fixed frame skip value",                  OPTION_FRAMESKIP },
	{ "Seconds to run",                          OPTION_SECONDS_TO_RUN },
	{ "Throttle",                                OPTION_THROTTLE },
	{ "Sleep",                                   OPTION_SLEEP },
	{ "Speed",                                   OPTION_SPEED },
	{ "Auto adjust speed to lower than refresh", OPTION_REFRESHSPEED },
	{ "Fast start",                              OPTION_FASTSTART },
	{ "Fast start skip",                         OPTION_FASTSTART_SKIP }
};

//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_menu_perf_menu::ui_menu_perf_menu(running_machine &machine, render_container *container) : ui_menu(machine, container)
{
}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_menu_perf_menu::~ui_menu_perf_menu()
{
}

//-------------------------------------------------
//  handle
//-------------------------------------------------

void ui_menu_perf_menu::handle()
{
	bool changed = false;
	emu_options::entry *entry;
	emu_options &mopts = machine().options();
	std::string error_string;
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
						f_step = atof(entry->minimum());
					else
						f_step = 0.1f;
					if (m_event->iptkey == IPT_UI_LEFT)
						f_cur -= f_step;
					else
						f_cur += f_step;
					mopts.set_value(m_options[d].name, f_cur, OPTION_PRIORITY_CMDLINE, error_string);
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

void ui_menu_perf_menu::populate()
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
	customtop =  machine().ui().get_line_height() + (3.0f * UI_BOX_TB_BORDER);
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void ui_menu_perf_menu::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	float width, maxwidth = origx2 - origx1;
	ui_manager &mui = machine().ui();

	mui.draw_text_full(container, "Performance Options", 0.0f, 0.0f, 1.0f, JUSTIFY_CENTER, WRAP_TRUNCATE,
	                              DRAW_NONE, ARGB_WHITE, ARGB_BLACK, &width, nullptr);
	width += 2 * UI_BOX_LR_BORDER;
	maxwidth = MAX(maxwidth, width);

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
	mui.draw_text_full(container, "Performance Options", x1, y1, x2 - x1, JUSTIFY_CENTER, WRAP_TRUNCATE,
	                              DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, nullptr, nullptr);

}
