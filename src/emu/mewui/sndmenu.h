/***************************************************************************

    mewui/sndmenu.h

    Internal MEWUI user interface.

***************************************************************************/

#pragma once

#ifndef __SNDMENU_H__
#define __SNDMENU_H__

//-------------------------------------------------
//  class sound options menu
//-------------------------------------------------
class ui_menu_sound_options : public ui_menu
{
public:
    ui_menu_sound_options(running_machine &machine, render_container *container);
    virtual ~ui_menu_sound_options();
    virtual void populate();
    virtual void handle();
    virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2);

private:
    enum
    {
        ENABLE_SOUND = 1,
        SAMPLE_RATE,
        ENABLE_SAMPLES
    };

    UINT16              cur_rates;
    static const int    sound_rate[];
    int                 m_sample_rate;
    bool                m_samples;
    bool                m_sound;
};

#endif /* __SNDMENU_H__ */
