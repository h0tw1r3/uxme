// license:BSD-3-Clause
// copyright-holders:h0tw1r3
/***************************************************************************

    ui/submenu.h

    UI options menu.

***************************************************************************/
#pragma once

#ifndef __UI_SUBMENU_H__
#define __UI_SUBMENU_H__

#include "ui/menu.h"

struct ui_submenu_option
{
	const char  *description;
	const char  *name;
	emu_options::entry *entry;
	core_options (*options);
};

//-------------------------------------------------
//  class ui menu
//-------------------------------------------------
class ui_submenu : public ui_menu
{
public:
	ui_submenu(running_machine &machine, render_container *container, ui_submenu_option *options, int count);
	virtual ~ui_submenu();
	virtual void populate() override;
	virtual void handle() override;
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

private:
	ui_submenu_option *m_options;
	int m_count;
};

#endif /* __UI_SUBMENU_H__ */
