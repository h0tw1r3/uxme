/***************************************************************************

    faststart.h

    Manages faststart option.

    This is an unofficial version based on MAME.
    Please do not send any reports from this build to the MAME team.

***************************************************************************/

#pragma once

#ifndef __FASTSTART_H__
#define __FASTSTART_H__

// buffer to handle copy/paste/insert of keys
class fast_start
{
	DISABLE_COPYING(fast_start);

public:
	fast_start(running_machine &machine);
	virtual ~fast_start();

        void run();

private:
        static int matching_game_name(const char *pBuf, const char *name);
        void load();
	void done(void *ptr, int param);
	void mute(void *ptr, int param);

	running_machine &  m_machine;
	emu_timer *        m_timer;
	emu_timer *        m_mute_timer;
        int                m_faststart_frames;
        int                m_faststart_type;
};

#endif	/* __FASTSTART_H__ */
