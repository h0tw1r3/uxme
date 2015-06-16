/***************************************************************************

    mewui/auditmenu.h

    Internal MEWUI user interface.

***************************************************************************/

#pragma once

#ifndef __MEWUI_AUDIT_H__
#define __MEWUI_AUDIT_H__

//-------------------------------------------------
//  class audit menu
//-------------------------------------------------

class ui_menu_audit : public ui_menu
{
public:
	ui_menu_audit(running_machine &machine, render_container *container, std::vector<const game_driver *> &available, std::vector<const game_driver *> &unavailable, std::vector<const game_driver *> &availablesorted, std::vector<const game_driver *> &unavailablesorted, int audit_mode);
	virtual ~ui_menu_audit();
	virtual void populate();
	virtual void handle();

private:
	std::vector<const game_driver *> &m_available;
	std::vector<const game_driver *> &m_unavailable;
	std::vector<const game_driver *> &m_availablesorted;
	std::vector<const game_driver *> &m_unavailablesorted;
	std::vector<UINT8> m_included;
	std::vector<const game_driver *> m_remove;

	int m_audit_mode, steps, x;
};

#endif /* __MEWUI_AUDIT_H__ */
