// license:BSD-3-Clause
// copyright-holders:Dankan1890
/***************************************************************************

    mewui/rotmenu.h

    MEWUI rotation options menu.

***************************************************************************/
#pragma once

#ifndef __MEWUI_ROTMENU_H__
#define __MEWUI_ROTMENU_H__

//-------------------------------------------------
//  class rotation menu
//-------------------------------------------------

class ui_menu_rot_menu : public ui_menu
{
public:
	ui_menu_rot_menu(running_machine &machine, render_container *container);
	virtual ~ui_menu_rot_menu();
	virtual void populate() override;
	virtual void handle() override;
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

private:
	struct rot_option
	{
		const char  *description;
		const char  *name;
	};

	static rot_option m_options[];
	int check_status(const char *status, const char *option);
};

#endif /* __MEWUI_PERFMENU_H__ */
