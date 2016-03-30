// license:BSD-3-Clause
// copyright-holders:h0tw1r3
/***************************************************************************

    ui/submenu.cpp

    UI options menu

***************************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "ui/submenu.h"
#include "ui/utils.h"
#include <limits>

//-------------------------------------------------
//  ctor / dtor
//-------------------------------------------------

ui_submenu::ui_submenu(running_machine &machine, render_container *container, std::vector<ui_submenu_option> &suboptions)
	: ui_menu(machine, container),
	m_options(suboptions)
{
	for (auto & item : m_options)
	{
		if (item.name == nullptr)
			continue;

		item.entry = machine.options().find(item.name);
		if (item.entry == nullptr)
		{
			item.entry = machine.ui().options().find(item.name);
			item.options = dynamic_cast<core_options*>(&machine.ui().options());
		}
		else
		{
			item.options = dynamic_cast<core_options*>(&machine.options());
		}
	}
}

ui_submenu::~ui_submenu()
{
}

//-------------------------------------------------
//  handlethe options menu
//-------------------------------------------------

void ui_submenu::handle()
{
	bool changed = false;
	std::string error_string, tmptxt;
	int i_cur;
	float f_cur, f_step;

	// process the menu
	const ui_menu_event *m_event = process(UI_MENU_PROCESS_LR_REPEAT | 0);
	if (m_event != nullptr && m_event->itemref != nullptr)
	{
		if (m_event->iptkey == IPT_UI_LEFT || m_event->iptkey == IPT_UI_RIGHT)
		{
			FPTR d = (FPTR)m_event->itemref;

			switch (m_options[d].entry->type())
			{
				case OPTION_BOOLEAN:
					changed = true;
					m_options[d].options->set_value(m_options[d].name, !strcmp(m_options[d].entry->value(),"1") ? "0" : "1", OPTION_PRIORITY_CMDLINE, error_string);
					break;
				case OPTION_INTEGER:
					changed = true;
					i_cur = atof(m_options[d].entry->value());
					(m_event->iptkey == IPT_UI_LEFT) ? i_cur-- : i_cur++;
					m_options[d].options->set_value(m_options[d].name, i_cur, OPTION_PRIORITY_CMDLINE, error_string);
					break;
				case OPTION_FLOAT:
					changed = true;
					f_cur = atof(m_options[d].entry->value());
					if (m_options[d].entry->has_range())
					{
						f_step = atof(m_options[d].entry->minimum());
						if (f_step <= 0.0f) {
							int pmin = getprecisionchr(m_options[d].entry->minimum());
							int pmax = getprecisionchr(m_options[d].entry->maximum());
							tmptxt = '1' + std::string((pmin > pmax) ? pmin : pmax, '0');
							f_step = 1 / atof(tmptxt.c_str());
						}
					}
					else
					{
						int precision = getprecisionchr(m_options[d].entry->default_value());
						tmptxt = '1' + std::string(precision, '0');
						f_step = 1 / atof(tmptxt.c_str());
					}
					if (m_event->iptkey == IPT_UI_LEFT)
						f_cur -= f_step;
					else
						f_cur += f_step;
					tmptxt = string_format("%g", f_cur);
					m_options[d].options->set_value(m_options[d].name, tmptxt.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
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

void ui_submenu::populate()
{
	UINT32 arrow_flags;
	std::string tmptxt;
	float f_min, f_max, f_cur;
	int i_min, i_max, i_cur;

	// add options
	int d = 0;
	for (auto item = m_options.begin(); item < m_options.end(); item++, d++)
	{
		// skip option group title
		if (d == 0) continue;

		if (item->name == nullptr && item->description == nullptr)
		{
			item_append(MENU_SEPARATOR_ITEM, nullptr, 0, nullptr);
			continue;
		}

		switch (item->entry->type())
		{
			case OPTION_BOOLEAN:
				arrow_flags = item->options->bool_value(item->name) ? MENU_FLAG_RIGHT_ARROW : MENU_FLAG_LEFT_ARROW;
				item_append(item->description,
						(arrow_flags == MENU_FLAG_RIGHT_ARROW) ? "On" : "Off",
						arrow_flags,
						(void *)(FPTR)d);
				break;
			case OPTION_INTEGER:
				i_cur = atof(item->entry->value());
				if (item->entry->has_range())
				{
					i_min = atof(item->entry->minimum());
					i_max = atof(item->entry->maximum());
				}
				else
				{
					i_min = std::numeric_limits<int>::min();
					i_max = std::numeric_limits<int>::max();
				}
				arrow_flags = get_arrow_flags(i_min, i_max, i_cur);
				item_append(item->description,
						item->entry->value(),
						arrow_flags,
						(void *)(FPTR)d);
				break;
			case OPTION_FLOAT:
				f_cur = atof(item->entry->value());
				if (item->entry->has_range())
				{
					f_min = atof(item->entry->minimum());
					f_max = atof(item->entry->maximum());
				}
				else
				{
					f_min = 0.0f;
					f_max = std::numeric_limits<float>::max();
				}
				arrow_flags = get_arrow_flags(f_min, f_max, f_cur);
				tmptxt = string_format("%g", f_cur);
				item_append(item->description,
						tmptxt.c_str(),
						arrow_flags,
						(void *)(FPTR)d);
				break;
			default:
				arrow_flags = MENU_FLAG_RIGHT_ARROW;
				item_append(item->description,
					item->options->value(item->name),
					arrow_flags, (void *)(FPTR)d);
				break;
		}
	}

	item_append(MENU_SEPARATOR_ITEM, nullptr, 0, nullptr);
	custombottom = customtop = machine().ui().get_line_height() + (3.0f * UI_BOX_TB_BORDER);
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void ui_submenu::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	static int interval = 0;
	static FPTR lastref = 0;

	float width;
	ui_manager &mui = machine().ui();

	mui.draw_text_full(container, m_options[0].description, 0.0f, 0.0f, 1.0f, JUSTIFY_CENTER, WRAP_TRUNCATE,
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
	mui.draw_text_full(container, m_options[0].description, x1, y1, x2 - x1, JUSTIFY_CENTER, WRAP_TRUNCATE,
	                              DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, nullptr, nullptr);

	if (selectedref != nullptr)
	{
		if (lastref == (FPTR)selectedref) {
			if (interval <= 15) interval++;
		} else {
			lastref = (FPTR)selectedref;
			interval = 0;
		}

		if (interval > 15)
		{
			mui.draw_text_full(container, m_options[lastref].entry->description(), 0.0f, 0.0f, 1.0f, JUSTIFY_CENTER, WRAP_TRUNCATE,
					DRAW_NONE, ARGB_WHITE, ARGB_BLACK, &width, nullptr);

			width += 2 * UI_BOX_LR_BORDER;
			maxwidth = MAX(origx2 - origx1, width);

			// compute our bounds
			x1 = 0.5f - 0.5f * maxwidth;
			x2 = x1 + maxwidth;
			y1 = origy2 + UI_BOX_TB_BORDER;
			y2 = origy2 + bottom;

			// draw a box
			mui.draw_outlined_box(container, x1, y1, x2, y2, UI_RED_COLOR);

			// take off the borders
			x1 += UI_BOX_LR_BORDER;
			x2 -= UI_BOX_LR_BORDER;
			y1 += UI_BOX_TB_BORDER;

			// draw the text within it
			mui.draw_text_full(container, m_options[lastref].entry->description(), x1, y1, x2 - x1, JUSTIFY_CENTER, WRAP_NEVER,
					DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, nullptr, nullptr);
		}
	}
	else
	{
		lastref = 0;
		interval = 0;
	}
}
