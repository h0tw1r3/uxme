/***************************************************************************

    faststart.c

    Manages faststart option.

    This is an unofficial version based on MAME.
    Please do not send any reports from this build to the MAME team.

***************************************************************************/

#include "emu.h"
#include "faststart.h"
#include "emuopts.h"
#include "screen.h"

#define MAX_CONFIG_LINE_SIZE 16

//-------------------------------------------------
//  faststart - constructor
//-------------------------------------------------

fast_start::fast_start(running_machine &machine)
	: m_machine(machine)
	, m_timer(nullptr)
	, m_mute_timer(nullptr)
	, m_faststart_frames(0)
        , m_faststart_type(0)
{
	m_faststart_type = m_machine.options().faststart();

	if (m_faststart_type > 0 && m_faststart_type < 3)
	{
                load();
	}
        else if (m_faststart_type != 0)
        {
		osd_printf_error("Invalid value '%d' for option 'faststart'.\n", m_faststart_type);
        }
}


//-------------------------------------------------
//  faststart - destructor
//-------------------------------------------------

fast_start::~fast_start()
{
}


void fast_start::run()
{
	if (m_faststart_frames > 0)
	{
		screen_device *first_screen = screen_device_enumerator(m_machine.root_device()).first();
		attotime faststart_time = (first_screen->frame_period() * m_faststart_frames);
		m_timer = m_machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(fast_start::done), this));
		m_timer->adjust(faststart_time, 0, first_screen->frame_period());
		m_machine.video().set_faststart(true);
		if (m_machine.options().faststart_skip())
		{
			m_mute_timer = m_machine.scheduler().timer_alloc(timer_expired_delegate(FUNC(fast_start::mute), this));
			m_mute_timer->adjust(attotime::zero);
		}
	}
}
/*  matching_game_name is used to skip over lines until we find <gamename>: */
int fast_start::matching_game_name (const char *pBuf, const char *name)
{
	while (*name)
	{
		if (*name++ != *pBuf++)
			return 0;
	}
	return (*pBuf == ':');
}

void fast_start::done(void *ptr, int param)
{
	m_timer->enable(false);
	m_machine.sound().system_mute(false);
	m_machine.video().set_faststart(false);
}

void fast_start::mute(void *ptr, int param)
{
	m_mute_timer->enable(false);
	m_machine.sound().system_mute(true);
}

void fast_start::load()
{
	// configure the fast accessor
	const char *name = m_machine.system().name;

	emu_file datfile(m_machine.options().plugin_data_path(), OPEN_FLAG_READ);
	std::error_condition const filerr(datfile.open("faststart.dat"));

	// if that failed, do nothing
	if (filerr)
		return;

	emu_file f(OPEN_FLAG_READ);

	char buffer[MAX_CONFIG_LINE_SIZE];
	enum { FIND_NAME, FIND_DATA, FIND_NEXT_DATA } mode;
	mode = FIND_NAME;

	while (datfile.gets(buffer, MAX_CONFIG_LINE_SIZE))
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
			m_faststart_frames = atoi(buffer);

			if (m_faststart_type == 2 && mode == FIND_DATA)
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
	datfile.close();
}

