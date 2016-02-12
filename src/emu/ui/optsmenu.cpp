// license:BSD-3-Clause
// copyright-holders:Dankan1890
/*********************************************************************

    ui/optsmenu.cpp

    UI main options menu manager.

*********************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "ui/menu.h"
#include "ui/submenu.h"
#include "ui/datfile.h"
#include "ui/inifile.h"
#include "ui/selector.h"
#include "ui/custui.h"
#include "ui/sndmenu.h"
#include "ui/ctrlmenu.h"
#include "ui/dsplmenu.h"
#include "ui/miscmenu.h"
#include "ui/optsmenu.h"
#include "ui/custmenu.h"
#include "ui/inputmap.h"
#include "rendfont.h"

ui_submenu_option perf_submenu_options[] = {
	{ "Performance Options",                     nullptr },
	{ "Auto frame skip",                         OPTION_AUTOFRAMESKIP },
	{ "Frame skip",                              OPTION_FRAMESKIP },
	{ "Throttle",                                OPTION_THROTTLE },
	{ "Sleep",                                   OPTION_SLEEP },
	{ "Speed",                                   OPTION_SPEED },
	{ "Refresh speed",                           OPTION_REFRESHSPEED },
	{ "Fast start",                              OPTION_FASTSTART },
	{ "Fast start skip",                         OPTION_FASTSTART_SKIP }
};

ui_submenu_option rotate_submenu_options[] = {
	{ "Rotation Options",                        nullptr },
	{ "Rotate",                                  OPTION_ROTATE },
	{ "Rotate right",                            OPTION_ROR },
	{ "Rotate left",                             OPTION_ROL },
	{ "Auto rotate right",                       OPTION_AUTOROR },
	{ "Auto rotate left",                        OPTION_AUTOROL },
	{ "Flip X",                                  OPTION_FLIPX },
	{ "Flip Y",                                  OPTION_FLIPY },
};

ui_submenu_option artwork_submenu_options[] = {
	{ "Artwork Options",                         nullptr },
	{ "Artwork Crop",                            OPTION_ARTWORK_CROP },
	{ "Use Backdrops",                           OPTION_USE_BACKDROPS },
	{ "Use Overlays",                            OPTION_USE_OVERLAYS },
	{ "Use Bezels",                              OPTION_USE_BEZELS },
	{ "Use Control Panels",                      OPTION_USE_CPANELS },
	{ "Use Marquees",                            OPTION_USE_MARQUEES },
};

ui_submenu_option state_submenu_options[] = {
	{ "State/Playback Options",                  nullptr },
	{ "High score",                              OPTION_HISCORE },
	{ "Automatic save/restore",                  OPTION_AUTOSAVE },
	{ "Bilinear snapshot",                       OPTION_SNAPBILINEAR },
	{ "Burn-in",                                 OPTION_BURNIN },
};

ui_submenu_option input_submenu_options[] = {
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

ui_submenu_option misc_submenu_options[] = {
	{ "Miscellaneous Options",                           nullptr },
	{ "Re-select last machine played",                   OPTION_REMEMBER_LAST },
	{ "Enlarge images in the right panel",               OPTION_ENLARGE_SNAPS },
	{ "DATs info",                                       OPTION_DATS_ENABLED },
	{ "Cheats",                                          OPTION_CHEAT },
	{ "Show mouse pointer",                              OPTION_UI_MOUSE },
	{ "Confirm quit from machines",                      OPTION_UI_CONFIRM_QUIT },
	{ "Skip displaying information's screen at startup", OPTION_UI_SKIP_GAMEINFO },
	{ "Force 4:3 appearance for software snapshot",      OPTION_FORCED4X3 },
	{ "Use image as background",                         OPTION_USE_BACKGROUND },
	{ "Skip bios selection menu",                        OPTION_SKIP_BIOS_MENU },
	{ "Skip software parts selection menu",              OPTION_SKIP_PARTS_MENU },
	{ nullptr,                                           nullptr },
	{ "Info auto audit",                                 OPTION_INFO_AUTO_AUDIT },
	{ "Skip disclaimer screen",                          OPTION_UI_SKIP_DISCLAIMER },
	{ "Skip warnings",                                   OPTION_UI_SKIP_WARNINGS },
	{ "Skip loading messages",                           OPTION_UI_SKIP_LOADING },
	{ "Render border",                                   OPTION_UI_RENDER_BORDER },
};

//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_menu_game_options::ui_menu_game_options(running_machine &machine, render_container *container) : ui_menu(machine, container)
{
}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_menu_game_options::~ui_menu_game_options()
{
	ui_menu::menu_stack->reset(UI_MENU_RESET_SELECT_FIRST);
	save_ui_options(machine());
	ui_globals::switch_image = true;
}

//-------------------------------------------------
//  handle
//-------------------------------------------------

void ui_menu_game_options::handle()
{
	bool changed = false;

	// process the menu
//	ui_menu::menu_stack->parent->process(UI_MENU_PROCESS_NOINPUT);
//	const ui_menu_event *m_event = process(UI_MENU_PROCESS_LR_REPEAT | UI_MENU_PROCESS_NOIMAGE);
	const ui_menu_event *m_event = process(UI_MENU_PROCESS_LR_REPEAT);

	if (m_event != nullptr && m_event->itemref != nullptr)
		switch ((FPTR)m_event->itemref)
		{
			case FILTER_MENU:
			{
				if (m_event->iptkey == IPT_UI_LEFT || m_event->iptkey == IPT_UI_RIGHT)
				{
					(m_event->iptkey == IPT_UI_RIGHT) ? ++main_filters::actual : --main_filters::actual;
					changed = true;
				}
				else if (m_event->iptkey == IPT_UI_SELECT)
				{
					int total = main_filters::length;
					std::vector<std::string> s_sel(total);
					for (int index = 0; index < total; ++index)
						s_sel[index] = main_filters::text[index];

					ui_menu::stack_push(global_alloc_clear<ui_menu_selector>(machine(), container, s_sel, main_filters::actual));
				}
				break;
			}
			case FILE_CATEGORY_FILTER:
			{
				if (m_event->iptkey == IPT_UI_LEFT)
				{
					machine().inifile().current_file--;
					machine().inifile().current_category = 0;
					changed = true;
				}
				else if (m_event->iptkey == IPT_UI_RIGHT)
				{
					machine().inifile().current_file++;
					machine().inifile().current_category = 0;
					changed = true;
				}
				else if (m_event->iptkey == IPT_UI_SELECT)
				{
					inifile_manager &ifile = machine().inifile();
					int total = ifile.ini_index.size();
					std::vector<std::string> s_sel(total);
					machine().inifile().current_category = 0;
					for (size_t index = 0; index < total; ++index)
						s_sel[index] = ifile.ini_index[index].name;

					ui_menu::stack_push(global_alloc_clear<ui_menu_selector>(machine(), container, s_sel, ifile.current_file, SELECTOR_INIFILE));
				}
				break;
			}
			case CATEGORY_FILTER:
			{
				if (m_event->iptkey == IPT_UI_LEFT)
				{
					machine().inifile().current_category--;
					changed = true;
				}
				else if (m_event->iptkey == IPT_UI_RIGHT)
				{
					machine().inifile().current_category++;
					changed = true;
				}
				else if (m_event->iptkey == IPT_UI_SELECT)
				{
					inifile_manager &ifile = machine().inifile();
					int cfile = ifile.current_file;
					int total = ifile.ini_index[cfile].category.size();
					std::vector<std::string> s_sel(total);
					for (int index = 0; index < total; ++index)
						s_sel[index] = ifile.ini_index[cfile].category[index].name;

					ui_menu::stack_push(global_alloc_clear<ui_menu_selector>(machine(), container, s_sel, ifile.current_category, SELECTOR_CATEGORY));
				}
				break;
			}
			case MANUFACT_CAT_FILTER:
				if (m_event->iptkey == IPT_UI_LEFT || m_event->iptkey == IPT_UI_RIGHT)
				{
					(m_event->iptkey == IPT_UI_RIGHT) ? c_mnfct::actual++ : c_mnfct::actual--;
					changed = true;
				}
				else if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_menu_selector>(machine(), container, c_mnfct::ui, c_mnfct::actual));

				break;
			case YEAR_CAT_FILTER:
				if (m_event->iptkey == IPT_UI_LEFT || m_event->iptkey == IPT_UI_RIGHT)
				{
					(m_event->iptkey == IPT_UI_RIGHT) ? c_year::actual++ : c_year::actual--;
					changed = true;
				}
				else if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_menu_selector>(machine(), container, c_year::ui, c_year::actual));

				break;
			case SOUND_MENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_menu_sound_options>(machine(), container));
				break;
			case DISPLAY_MENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_menu_display_options>(machine(), container));
				break;
			case CUSTOM_MENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_menu_custom_ui>(machine(), container));
				break;
			case CONTROLLER_MENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_menu_controller_mapping>(machine(), container));
				break;

			case CGI_MENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_menu_input_groups>(machine(), container));
				break;

			case CUSTOM_FILTER:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_menu_custom_filter>(machine(), container));
				break;

			case STATE_SUBMENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_submenu>(machine(), container, state_submenu_options, ARRAY_LENGTH(state_submenu_options)));
				break;
			case PERF_SUBMENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_submenu>(machine(), container, perf_submenu_options, ARRAY_LENGTH(perf_submenu_options)));
				break;
			case ROTATE_SUBMENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_submenu>(machine(), container, rotate_submenu_options, ARRAY_LENGTH(rotate_submenu_options)));
				break;
			case ARTWORK_SUBMENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_submenu>(machine(), container, artwork_submenu_options, ARRAY_LENGTH(artwork_submenu_options)));
				break;
			case INPUT_SUBMENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_submenu>(machine(), container, input_submenu_options, ARRAY_LENGTH(input_submenu_options)));
				break;
			case MISC_SUBMENU:
				if (m_event->iptkey == IPT_UI_SELECT)
					ui_menu::stack_push(global_alloc_clear<ui_submenu>(machine(), container, misc_submenu_options, ARRAY_LENGTH(misc_submenu_options)));
				break;
		}

	if (changed)
		reset(UI_MENU_RESET_REMEMBER_REF);
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_menu_game_options::populate()
{
	if (strcmp(machine().options().ui(),"simple")!=0) 
	{	
		// set filter arrow
		std::string fbuff;

		// add filter item
		UINT32 arrow_flags = get_arrow_flags((int)FILTER_FIRST, (int)FILTER_LAST, main_filters::actual);
		item_append("Filter", main_filters::text[main_filters::actual], arrow_flags, (void *)(FPTR)FILTER_MENU);

		// add category subitem
		if (main_filters::actual == FILTER_CATEGORY && !machine().inifile().ini_index.empty())
		{
			inifile_manager &inif = machine().inifile();
			int afile = inif.current_file;

			arrow_flags = get_arrow_flags(0, inif.ini_index.size() - 1, afile);
			fbuff = " ^!File";
			convert_command_glyph(fbuff);
			item_append(fbuff.c_str(), inif.actual_file().c_str(), arrow_flags, (void *)(FPTR)FILE_CATEGORY_FILTER);

			arrow_flags = get_arrow_flags(0, inif.ini_index[afile].category.size() - 1, inif.current_category);
			fbuff = " ^!Category";
			convert_command_glyph(fbuff);
			item_append(fbuff.c_str(), inif.actual_category().c_str(), arrow_flags, (void *)(FPTR)CATEGORY_FILTER);
		}
		// add manufacturer subitem
		else if (main_filters::actual == FILTER_MANUFACTURER && c_mnfct::ui.size() > 0)
		{
			arrow_flags = get_arrow_flags(0, c_mnfct::ui.size() - 1, c_mnfct::actual);
			fbuff = "^!Manufacturer";
			convert_command_glyph(fbuff);
			item_append(fbuff.c_str(), c_mnfct::ui[c_mnfct::actual].c_str(), arrow_flags, (void *)(FPTR)MANUFACT_CAT_FILTER);
		}
		// add year subitem
		else if (main_filters::actual == FILTER_YEAR && c_year::ui.size() > 0)
		{
			arrow_flags = get_arrow_flags(0, c_year::ui.size() - 1, c_year::actual);
			fbuff.assign("^!Year");
			convert_command_glyph(fbuff);
			item_append(fbuff.c_str(), c_year::ui[c_year::actual].c_str(), arrow_flags, (void *)(FPTR)YEAR_CAT_FILTER);
		}
		// add custom subitem
		else if (main_filters::actual == FILTER_CUSTOM)
		{
			fbuff = "^!Setup custom filter";
			convert_command_glyph(fbuff);
			item_append(fbuff.c_str(), nullptr, 0, (void *)(FPTR)CUSTOM_FILTER);
		}

		item_append(MENU_SEPARATOR_ITEM, nullptr, 0, nullptr);

		// add options items
		item_append("Customize UI", nullptr, 0, (void *)(FPTR)CUSTOM_MENU);
	}
	item_append("Display Options", nullptr, 0, (void *)(FPTR)DISPLAY_MENU);
	item_append("Sound Options", nullptr, 0, (void *)(FPTR)SOUND_MENU);
	item_append(misc_submenu_options[0].description,    nullptr, 0, (void *)(FPTR)MISC_SUBMENU);
	item_append(rotate_submenu_options[0].description,  nullptr, 0, (void *)(FPTR)ROTATE_SUBMENU);
	item_append(artwork_submenu_options[0].description, nullptr, 0, (void *)(FPTR)ARTWORK_SUBMENU);
	item_append(input_submenu_options[0].description,   nullptr, 0, (void *)(FPTR)INPUT_SUBMENU);
	item_append(perf_submenu_options[0].description,    nullptr, 0, (void *)(FPTR)PERF_SUBMENU);
	item_append(state_submenu_options[0].description,   nullptr, 0, (void *)(FPTR)STATE_SUBMENU);
	item_append("Device Mapping", nullptr, 0, (void *)(FPTR)CONTROLLER_MENU);
	item_append("General Inputs", nullptr, 0, (void *)(FPTR)CGI_MENU);
	item_append(MENU_SEPARATOR_ITEM, nullptr, 0, nullptr);

	custombottom = 2.0f * machine().ui().get_line_height() + 3.0f * UI_BOX_TB_BORDER;
	customtop = machine().ui().get_line_height() + 3.0f * UI_BOX_TB_BORDER;
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void ui_menu_game_options::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	float width;
	ui_manager &mui = machine().ui();
	mui.draw_text_full(container, "Settings", 0.0f, 0.0f, 1.0f, JUSTIFY_CENTER, WRAP_TRUNCATE,
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
	mui.draw_text_full(container, "Settings", x1, y1, x2 - x1, JUSTIFY_CENTER, WRAP_TRUNCATE,
	                              DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, nullptr, nullptr);
}

//-------------------------------------------------
//  save ui options
//-------------------------------------------------

void save_ui_options(running_machine &machine)
{
	// attempt to open the output file
	emu_file file(machine.options().ini_path(), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);
	if (file.open("ui.ini") == FILERR_NONE)
	{
		// generate the updated INI
		std::string initext = machine.ui().options().output_ini();
		file.puts(initext.c_str());
		file.close();
	}
	else
		machine.popmessage("**Error to save ui.ini**", emulator_info::get_configname());
}
