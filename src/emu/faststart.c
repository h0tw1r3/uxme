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
static int faststart_frames;

void faststart_load (running_machine &machine)
{
	file_error filerr;
	const char *name = machine.system().name;

	emu_file f(machine.options().dat_path(), OPEN_FLAG_READ);
	filerr = f.open("faststart", ".dat");

	if(filerr == FILERR_NONE)
	{
		char buffer[MAX_CONFIG_LINE_SIZE];
		enum { FIND_NAME, FIND_DATA, FETCH_DATA } mode;
		mode = FIND_NAME;

		while (f.gets(buffer, MAX_CONFIG_LINE_SIZE))
		{
			if (mode == FIND_NAME)
			{
				if (strncmp(name, buffer, strlen(name)) == 0)
					mode = FIND_DATA;
			}
			else if (atoi(buffer) > 0)
			{
				faststart_frames = atoi(buffer);
				break;
			}
		}
		f.close();
	}
}

static TIMER_CALLBACK( faststart_done )
{
	timer->enable(false);
	machine.video().set_faststart(false);
}

void faststart_init (running_machine &machine)
{
	const char *faststart = machine.options().fast_start();

	if (faststart != NULL && faststart[0] != 0)
	{
		if (strcmp(faststart, "auto") == 0)
			faststart_load(machine);
		else
			faststart_frames = atoi(faststart);

		if (faststart_frames > 0)
		{
			attotime faststart_time = (machine.first_screen()->frame_period() * faststart_frames);
			timer = machine.scheduler().timer_alloc(FUNC(faststart_done));
			timer->adjust(faststart_time, 0, machine.first_screen()->frame_period());
			machine.video().set_faststart(true);
		}
		else if (!(strcmp(faststart, "auto") == 0) && strcmp(faststart, "0") != 0)
		{
			osd_printf_error("Invalid value '%s' for option 'faststart'.\n", faststart);
		}
	}

}
