/*********************************************************************

    mewui/sndmenu.c

    Internal MEWUI user interface.

*********************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "mewui/sndmenu.h"
#include "mewui/selector.h"

#ifdef MEWUI_SDL
#include "../osd/sdl/osdsdl.h"
#else
#include "../osd/windows/winmain.h"
#endif

const int ui_menu_sound_options::sound_rate[] = { 11025, 22050, 44100, 48000 };

//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_menu_sound_options::ui_menu_sound_options(running_machine &machine, render_container *container) : ui_menu(machine, container)
{
#ifdef MEWUI_SDL
    sdl_options &options = downcast<sdl_options &>(machine.options());
#else
    windows_options &options = downcast<windows_options &>(machine.options());
#endif

    m_sample_rate = machine.options().sample_rate();
    m_sound = (strcmp(options.sound(), OSDOPTVAL_NONE) && strcmp(options.sound(), "0"));
    m_samples = machine.options().samples();

    int total = ARRAY_LENGTH(sound_rate);

    for (cur_rates = 0; cur_rates < total; cur_rates++)
        if (m_sample_rate == sound_rate[cur_rates])
            break;

    if (cur_rates == total)
        cur_rates = 2;
}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_menu_sound_options::~ui_menu_sound_options()
{
    std::string error_string;

    if (m_sound)
        machine().options().set_value(OSDOPTION_SOUND, OSDOPTVAL_AUTO, OPTION_PRIORITY_CMDLINE, error_string);
    else
        machine().options().set_value(OSDOPTION_SOUND, OSDOPTVAL_NONE, OPTION_PRIORITY_CMDLINE, error_string);

    machine().options().set_value(OPTION_SAMPLERATE, sound_rate[cur_rates], OPTION_PRIORITY_CMDLINE, error_string);
    machine().options().set_value(OPTION_SAMPLES, m_samples, OPTION_PRIORITY_CMDLINE, error_string);
}

//-------------------------------------------------
//  handle
//-------------------------------------------------

void ui_menu_sound_options::handle()
{
    bool changed = false;

    // process the menu
    const ui_menu_event *menu_event = process(0);

    if (menu_event != NULL && menu_event->itemref != NULL)
    {
        switch ((FPTR)menu_event->itemref)
        {
            case ENABLE_SOUND:
                if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT || menu_event->iptkey == IPT_UI_SELECT)
                {
                    m_sound = !m_sound;
                    changed = true;
                }
                break;

            case SAMPLE_RATE:
                if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT)
                {
                    (menu_event->iptkey == IPT_UI_LEFT) ? cur_rates-- : cur_rates++;
                    changed = true;
                }

                else if (menu_event->iptkey == IPT_UI_SELECT)
                {
                    int total = ARRAY_LENGTH(sound_rate);
                    std::string s_sel[total + 1];
                    for (int index = 0; index < total; index++)
                        strprintf(s_sel[index], "%d", sound_rate[index]);

                    ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_selector(machine(), container, s_sel, &cur_rates, total)));
                }
                break;

            case ENABLE_SAMPLES:
                if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT || menu_event->iptkey == IPT_UI_SELECT)
                {
                    m_samples = !m_samples;
                    changed = true;
                }
                break;
        }
    }

    if (changed)
        reset(UI_MENU_RESET_REMEMBER_REF);

}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_menu_sound_options::populate()
{
    UINT32 arrow_flags = get_arrow_flags(0, ARRAY_LENGTH(sound_rate) - 1, cur_rates);
    m_sample_rate = sound_rate[cur_rates];
    std::string s_text;
    strprintf(s_text, "%d", m_sample_rate);

    // add options items
    item_append("Sound", m_sound ? "On" : "Off", m_sound ? MENU_FLAG_RIGHT_ARROW : MENU_FLAG_LEFT_ARROW, (void *)ENABLE_SOUND);
    item_append("Sample Rate", s_text.c_str(), arrow_flags, (void *)SAMPLE_RATE);
    item_append("Use External Samples", m_samples ? "On" : "Off", m_samples ? MENU_FLAG_RIGHT_ARROW : MENU_FLAG_LEFT_ARROW, (void *)ENABLE_SAMPLES);
    item_append(MENU_SEPARATOR_ITEM, NULL, 0, NULL);

    customtop = machine().ui().get_line_height() + (3.0f * UI_BOX_TB_BORDER);
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void ui_menu_sound_options::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
    float x1, x2, y1, y2, width, maxwidth = origx2 - origx1;
    std::string tempbuf("Sound Options");

    machine().ui().draw_text_full(container, tempbuf.c_str(), 0.0f, 0.0f, 1.0f, JUSTIFY_CENTER, WRAP_TRUNCATE,
                                    DRAW_NONE, ARGB_WHITE, ARGB_BLACK, &width, NULL);
    width += 2 * UI_BOX_LR_BORDER;
    maxwidth = MAX(maxwidth, width);

    // compute our bounds
    x1 = 0.5f - 0.5f * maxwidth;
    x2 = x1 + maxwidth;
    y1 = origy1 - top;
    y2 = origy1 - UI_BOX_TB_BORDER;

    // draw a box
    machine().ui().draw_outlined_box(container, x1, y1, x2, y2, UI_GREEN_COLOR);

    // take off the borders
    x1 += UI_BOX_LR_BORDER;
    x2 -= UI_BOX_LR_BORDER;
    y1 += UI_BOX_TB_BORDER;

    // draw the text within it
    machine().ui().draw_text_full(container, tempbuf.c_str(), x1, y1, x2 - x1, JUSTIFY_CENTER, WRAP_TRUNCATE,
                                    DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL);
}
