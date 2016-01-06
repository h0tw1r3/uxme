/***************************************************************************

    hiscore.c

    Manages the hiscore system.

    This is an unofficial version based on MAME.
    Please do not send any reports from this build to the MAME team.

***************************************************************************/

#include "emu.h"
#include "emuopts.h"
#include "hiscore.h"

#define MAX_CONFIG_LINE_SIZE 48

static emu_timer *timer;

struct memory_range
{
	UINT32 cpu, addr, num_bytes, start_value, end_value;
	struct memory_range *next;
};

static struct
{
	int hiscores_have_been_loaded;
	memory_range *mem_range;
} state;


static void copy_to_memory (running_machine &machine, int cpu, int addr, UINT8 *source, int num_bytes)
{
	bool mem_as_data = (strstr(machine.system().source_file,"cinemat.c") > 0);
	address_space *targetspace = &machine.cpu[cpu]->memory().space(mem_as_data ? AS_DATA : AS_PROGRAM);

	for (int i=0; i<num_bytes; i++)
		targetspace->write_byte(addr+i, source[i]);
}

static void copy_from_memory (running_machine &machine, int cpu, int addr, UINT8 *dest, int num_bytes)
{
	bool mem_as_data = (strstr(machine.system().source_file,"cinemat.c") > 0);
	address_space *targetspace = &machine.cpu[cpu]->memory().space(mem_as_data ? AS_DATA : AS_PROGRAM);

	for (int i=0; i<num_bytes; i++)
		dest[i] = targetspace->read_byte(addr+i);
}

/*  hexstr2num extracts and returns the value of a hexadecimal field from the
    character buffer pointed to by pString.
    When hexstr2num returns, *pString points to the character following
    the first non-hexadecimal digit, or nullptr if an end-of-string marker
    (0x00) is encountered. */
static UINT32 hexstr2num (const char **pString)
{
	const char *string = *pString;
	UINT32 result = 0;

	if (string)
	{
		for(;;)
		{
			char c = *string++;
			int digit;

			if (c>='0' && c<='9')
				digit = c-'0';
			else if (c>='a' && c<='f')
				digit = 10+c-'a';
			else if (c>='A' && c<='F')
				digit = 10+c-'A';
			else
			{
				/* not a hexadecimal digit */
				/* safety check for premature EOL */
				if (!c)
					string = nullptr;
				break;
			}
			result = result*16 + digit;
		}
		*pString = string;
	}
	return result;
}

/*  given a line in the hiscore.dat file, determine if it encodes a
    memory range (or a game name).
    For now we assume that CPU number is always a decimal digit, and
    that no game name starts with a decimal digit. */
static int is_mem_range (const char *pBuf)
{
	char c = 0;

	for(;;)
	{
		c = *pBuf++;
		if (c == 0)
			return 0; /* premature EOL */
		if (c == ':')
			break;
	}

	c = *pBuf; /* character following first ':' */

	return (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F');
}

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

/* safe_to_load checks the start and end values of each memory range */
static int safe_to_load (running_machine &machine)
{
	memory_range *mem_range = state.mem_range;
	bool mem_as_data = (strstr(machine.system().source_file,"cinemat.c") > 0);
	address_space *srcspace = &machine.cpu[mem_range->cpu]->memory().space(mem_as_data ? AS_DATA: AS_PROGRAM);

	while (mem_range)
	{
		if (srcspace->read_byte(mem_range->addr) != mem_range->start_value)
			return 0;

		if (srcspace->read_byte(mem_range->addr + mem_range->num_bytes - 1) != mem_range->end_value)
			return 0;

		mem_range = mem_range->next;
	}
	return 1;
}

/* hiscore_free disposes of the mem_range linked list */
static void hiscore_free (void)
{
	memory_range *mem_range = state.mem_range;

	while (mem_range)
	{
		memory_range *next = mem_range->next;
		global_free_array(mem_range);
		mem_range = next;
	}
	state.mem_range = nullptr;
}

static void hiscore_load (running_machine &machine)
{
	file_error filerr;
	emu_file f(machine.options().hiscore_directory(), OPEN_FLAG_READ);
	filerr = f.open(machine.basename(), ".hi");
	state.hiscores_have_been_loaded = 1;

	if (filerr == FILERR_NONE)
	{
		memory_range *mem_range = state.mem_range;

		while (mem_range)
		{
			std::unique_ptr<UINT8[]> data = std::make_unique<UINT8[]>(mem_range->num_bytes);

			if (data)
			{
				/*  this buffer will almost certainly be small
				    enough to be dynamically allocated, but let's
				    avoid memory trashing just in case */
				f.read(data.get(), mem_range->num_bytes);
				copy_to_memory (machine, mem_range->cpu, mem_range->addr, data.get(), mem_range->num_bytes);
			}
			mem_range = mem_range->next;
		}
		f.close();
	}
}

static void hiscore_save (running_machine &machine)
{
	file_error filerr;
	emu_file f(machine.options().hiscore_directory(), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);
	filerr = f.open(machine.basename(), ".hi");

	if (filerr == FILERR_NONE)
	{
		memory_range *mem_range = state.mem_range;

		while (mem_range)
		{
			std::unique_ptr<UINT8[]> data = std::make_unique<UINT8[]>(mem_range->num_bytes);

			if (data)
			{
				/*  this buffer will almost certainly be small
				    enough to be dynamically allocated, but let's
				    avoid memory trashing just in case */
				copy_from_memory (machine, mem_range->cpu, mem_range->addr, data.get(), mem_range->num_bytes);
				f.write(data.get(), mem_range->num_bytes);
			}
			mem_range = mem_range->next;
		}
		f.close();
	}
}

/* call hiscore_update periodically (i.e. once per frame) */
static TIMER_CALLBACK( hiscore_periodic )
{
	if (state.mem_range && !state.hiscores_have_been_loaded && safe_to_load(machine))
	{
				hiscore_load(machine);
				timer->enable(false);
	}
}

/* call hiscore_close when done playing game */
void hiscore_close (running_machine &machine)
{
	if (state.hiscores_have_been_loaded)
		hiscore_save(machine);
	hiscore_free();
}

/* call hiscore_open once after loading a game */
void hiscore_init (running_machine &machine)
{
	memory_range *mem_range = state.mem_range;
	address_space *initspace;
	file_error filerr;
	const char *name = machine.system().name;
	state.hiscores_have_been_loaded = 0;
	bool mem_as_data = (strstr(machine.system().source_file,"cinemat.c") > 0);

	while (mem_range)
	{
		initspace = &machine.cpu[mem_range->cpu]->memory().space(mem_as_data ? AS_DATA : AS_PROGRAM);
		initspace->write_byte(mem_range->addr, ~mem_range->start_value);
		initspace->write_byte(mem_range->addr + mem_range->num_bytes-1, ~mem_range->end_value);
		mem_range = mem_range->next;
	}

	state.mem_range = nullptr;
	emu_file f(machine.options().dat_path(), OPEN_FLAG_READ);
	filerr = f.open("hiscore.dat");

	if(filerr == FILERR_NONE)
	{
		char buffer[MAX_CONFIG_LINE_SIZE];
		enum { FIND_NAME, FIND_DATA, FETCH_DATA } mode;
		mode = FIND_NAME;

		while (f.gets(buffer, MAX_CONFIG_LINE_SIZE))
		{
			if (mode == FIND_NAME)
			{
				if (matching_game_name (buffer, name))
					mode = FIND_DATA;
			}
			else if (is_mem_range (buffer))
			{
				const char *pBuf = buffer;
				mem_range = (memory_range *)malloc(sizeof(memory_range));

				if (mem_range)
				{
					mem_range->cpu = hexstr2num (&pBuf);
					mem_range->addr = hexstr2num (&pBuf);
					mem_range->num_bytes = hexstr2num (&pBuf);
					mem_range->start_value = hexstr2num (&pBuf);
					mem_range->end_value = hexstr2num (&pBuf);

					mem_range->next = nullptr;
					{
						memory_range *last = state.mem_range;
						while (last && last->next) last = last->next;

						if (last == nullptr)
							state.mem_range = mem_range;
						else
							last->next = mem_range;
					}

					mode = FETCH_DATA;
				}
				else
				{
					hiscore_free();
					break;
				}
			}
			else
			{
				/* line is a game name */
				if (mode == FETCH_DATA)
					break;
			}
		}
		f.close();
	}

	timer = machine.scheduler().timer_alloc(FUNC(hiscore_periodic));
	timer->adjust(machine.first_screen()->frame_period(), 0, machine.first_screen()->frame_period());

	machine.add_notifier(MACHINE_NOTIFY_EXIT, machine_notify_delegate(FUNC(hiscore_close), &machine));
}
