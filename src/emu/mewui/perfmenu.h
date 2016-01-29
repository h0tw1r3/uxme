// license:BSD-3-Clause
// copyright-holders:Dankan1890
/***************************************************************************

    mewui/perfmenu.h

    Internal MEWUI user interface.

***************************************************************************/
#pragma once

#ifndef __MEWUI_PERFMENU_H__
#define __MEWUI_PERFMENU_H__

//-------------------------------------------------
//  class performance menu
//-------------------------------------------------

class ui_menu_perf_menu : public ui_menu
{
public:
	ui_menu_perf_menu(running_machine &machine, render_container *container);
	virtual ~ui_menu_perf_menu();
	virtual void populate() override;
	virtual void handle() override;
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

private:
	struct perf_option
	{
		const char  *description;
		const char  *name;
	};

	static perf_option m_options[];
	int check_status(const char *status, const char *option);
};

#endif /* __MEWUI_PERFMENU_H__ */
