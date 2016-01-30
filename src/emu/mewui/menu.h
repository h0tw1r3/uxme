// license:BSD-3-Clause
// copyright-holders:Dankan1890
/***************************************************************************

    mewui/statemenu.h

    MEWUI state options menu.

***************************************************************************/
#pragma once

#ifndef __MEWUI_MENU_H__
#define __MEWUI_MENU_H__

struct mewui_menu_option
{
	const char  *description;
	const char  *name;
};

//-------------------------------------------------
//  class mewui menu
//-------------------------------------------------
class mewui_menu : public ui_menu
{
public:
	mewui_menu(running_machine &machine, render_container *container, mewui_menu_option *options, int count);
	virtual ~mewui_menu();
	virtual void populate() override;
	virtual void handle() override;
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

private:
	mewui_menu_option *m_options;
	int m_count;
};

#endif /* __MEWUI_MENU_H__ */
