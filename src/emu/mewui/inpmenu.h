// license:BSD-3-Clause
// copyright-holders:Jeffrey Clark,Dankan1890
/***************************************************************************

    mewui/inpmenu.h

    MEWUI input options menu.

***************************************************************************/
#pragma once

#ifndef __MEWUI_INPMENU_H__
#define __MEWUI_INPMENU_H__

//-------------------------------------------------
//  class input menu
//-------------------------------------------------

class ui_menu_inp_menu : public ui_menu
{
public:
	ui_menu_inp_menu(running_machine &machine, render_container *container);
	virtual ~ui_menu_inp_menu();
	virtual void populate() override;
	virtual void handle() override;
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

private:
	struct inp_option
	{
		const char  *description;
		const char  *name;
	};

	static inp_option m_options[];
};

#endif /* __MEWUI_PERFMENU_H__ */
