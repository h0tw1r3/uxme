/***************************************************************************

    faststart.c

    Manages faststart option.

    This is an unofficial version based on MAME.
    Please do not send any reports from this build to the MAME team.

***************************************************************************/

#include "emu.h"
#include "emuopts.h"
#include "faststart.h"

#define MAX_CONFIG_LINE_SIZE 16

static emu_timer *timer;
static emu_timer *mutetimer;
static int faststart_frames;
static int faststart_type;

/*  matching_game_name is used to skip over lines until we find <gamename>: */
static int matching_game_name (const char *pBuf, const char *name)
{
	while (*name)
	{
		if (*name++ != *pBuf++)
			return 0;
	}
	return (*pBuf == ':');
}

void faststart_load (running_machine &machine)
{
	const char *name = machine.system().name;

	emu_file f(machine.ui().options().history_path(), OPEN_FLAG_READ);

	if(f.open("faststart", ".dat") == osd_file::error::NONE)
	{
		char buffer[MAX_CONFIG_LINE_SIZE];
		enum { FIND_NAME, FIND_DATA, FIND_NEXT_DATA } mode;
		mode = FIND_NAME;

		while (f.gets(buffer, MAX_CONFIG_LINE_SIZE))
		{
			if (buffer[0] == '#')
				continue;

			if (mode == FIND_NAME)
			{
				if (matching_game_name(buffer, name))
					mode = FIND_DATA;
			}
			else if (atoi(buffer) > 0)
			{
				faststart_frames = atoi(buffer);

				if (faststart_type == 2 && mode == FIND_DATA)
				{
					mode = FIND_NEXT_DATA;
					continue;
				}
				break;
			}
			else if (mode == FIND_NEXT_DATA)
			{
				break;
			}
		}
		f.close();
	}
}

static TIMER_CALLBACK( faststart_done )
{
	timer->enable(false);
	machine.sound().system_mute(false);
	machine.video().set_faststart(false);
}

static TIMER_CALLBACK( faststart_mute )
{
	mutetimer->enable(false);
	machine.sound().system_mute(true);
}

void faststart_init (running_machine &machine)
{
	faststart_type = machine.options().fast_start();

	if (faststart_type > 0 && faststart_type < 3)
	{
		faststart_load(machine);

		if (faststart_frames > 0)
		{
			attotime faststart_time = (machine.first_screen()->frame_period() * faststart_frames);
			timer = machine.scheduler().timer_alloc(FUNC(faststart_done));
			timer->adjust(faststart_time, 0, machine.first_screen()->frame_period());
			machine.video().set_faststart(true);
			if (machine.options().fast_start_skip())
			{
				mutetimer = machine.scheduler().timer_alloc(FUNC(faststart_mute));
				mutetimer->adjust(attotime::zero);
			}
		}
	} else if (faststart_type != 0)
		osd_printf_error("Invalid value '%d' for option 'faststart'.\n", faststart_type);
}
