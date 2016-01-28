// license:BSD-3-Clause
// copyright-holders:Dankan1890
/***************************************************************************

    mewui/statemenu.h

    MEWUI state options menu.

***************************************************************************/
#pragma once

#ifndef __MEWUI_STATEMENU_H__
#define __MEWUI_STATEMENU_H__

//-------------------------------------------------
//  class stateellaneous options menu
//-------------------------------------------------
class ui_menu_state_options : public ui_menu
{
public:
	ui_menu_state_options(running_machine &machine, render_container *container);
	virtual ~ui_menu_state_options();
	virtual void populate() override;
	virtual void handle() override;
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

private:
	struct state_option
	{
		bool        status;
		const char  *description;
		const char  *option;
	};

	static state_option m_options[];
};

#endif /* __MEWUI_STATEMENU_H__ */
