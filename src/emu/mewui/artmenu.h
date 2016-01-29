// license:BSD-3-Clause
// copyright-holders:Jeffrey Clark,Dankan1890
/***************************************************************************

    mewui/artmenu.h

    MEWUI artwork options menu.

***************************************************************************/
#pragma once

#ifndef __MEWUI_ARTMENU_H__
#define __MEWUI_ARTMENU_H__

//-------------------------------------------------
//  class artwork menu
//-------------------------------------------------

class ui_menu_art_menu : public ui_menu
{
public:
	ui_menu_art_menu(running_machine &machine, render_container *container);
	virtual ~ui_menu_art_menu();
	virtual void populate() override;
	virtual void handle() override;
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

private:
	struct art_option
	{
		const char  *description;
		const char  *name;
	};

	static art_option m_options[];
	int check_status(const char *status, const char *option);
};

#endif /* __MEWUI_PERFMENU_H__ */
