// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria, Aaron Giles
/***************************************************************************

    output.c

    General purpose output routines.
***************************************************************************/

#include "emu.h"
#include <zlib.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

/***************************************************************************
    CONSTANTS
***************************************************************************/

#define HASH_SIZE       53



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

class output_notify
{
public:
	output_notify(output_notifier_func callback, void *param)
		: m_next(NULL),
			m_notifier(callback),
			m_param(param) { }

	output_notify *next() const { return m_next; }

	output_notify *         m_next;           /* link to next item */
	output_notifier_func    m_notifier;       /* callback to call */
	void *                  m_param;          /* parameter to pass the callback */
};


struct output_item
{
	output_item *       next;           /* next item in list */
	std::string         name;           /* string name of the item */
	UINT32              hash;           /* hash for this item name */
	UINT32              id;             /* unique ID for this item */
	INT32               value;          /* current value */
	simple_list<output_notify> notifylist;     /* list of notifier callbacks */
};



/***************************************************************************
    GLOBAL VARIABLES
***************************************************************************/

static output_item *itemtable[HASH_SIZE];
static simple_list<output_notify> global_notifylist;
static UINT32 uniqueid = 12345;



/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

static void output_pause(running_machine &machine);
static void output_resume(running_machine &machine);
static void output_exit(running_machine &machine);



/***************************************************************************
    INLINE FUNCTIONS
***************************************************************************/

/*-------------------------------------------------
    get_hash - return the hash of an output value
-------------------------------------------------*/

INLINE UINT32 get_hash(const char *string)
{
	return crc32(0, (UINT8 *)string, (UINT32)strlen(string));
}


/*-------------------------------------------------
    find_item - find an item based on a string
-------------------------------------------------*/

INLINE output_item *find_item(const char *string)
{
	UINT32 hash = get_hash(string);
	output_item *item;

	/* use the hash as a starting point and find an entry */
	for (item = itemtable[hash % HASH_SIZE]; item != NULL; item = item->next)
		if (item->hash == hash && strcmp(string, item->name.c_str()) == 0)
			return item;

	return NULL;
}


/*-------------------------------------------------
    create_new_item - create a new item
-------------------------------------------------*/

INLINE output_item *create_new_item(const char *outname, INT32 value)
{
	output_item *item = global_alloc(output_item);
	UINT32 hash = get_hash(outname);

	/* fill in the data */
	item->next = itemtable[hash % HASH_SIZE];
	item->name.assign(outname);
	item->hash = hash;
	item->id = uniqueid++;
	item->value = value;

	/* add us to the hash table */
	itemtable[hash % HASH_SIZE] = item;
	return item;
}



/***************************************************************************
    CORE IMPLEMENTATION
***************************************************************************/

/*-------------------------------------------------
    output_init - initialize everything
-------------------------------------------------*/

void output_init(running_machine &machine)
{
	/* add pause callback */
	machine.add_notifier(MACHINE_NOTIFY_PAUSE, machine_notify_delegate(FUNC(output_pause), &machine));
	machine.add_notifier(MACHINE_NOTIFY_RESUME, machine_notify_delegate(FUNC(output_resume), &machine));

	/* get a callback when done */
	machine.add_notifier(MACHINE_NOTIFY_EXIT, machine_notify_delegate(FUNC(output_exit), &machine));

	/* reset the lists */
	memset(itemtable, 0, sizeof(itemtable));
	global_notifylist.reset();
}


/*-------------------------------------------------
    output_pause - send pause message
-------------------------------------------------*/

static void output_pause(running_machine &machine)
{
	output_set_value("pause", 1);
}

static void output_resume(running_machine &machine)
{
	output_set_value("pause", 0);
}


/*-------------------------------------------------
    output_exit - cleanup on exit
-------------------------------------------------*/

static void output_exit(running_machine &machine)
{
	output_item *item;
	int hash;

	/* remove all items */
	for (hash = 0; hash < HASH_SIZE; hash++)
		for (item = itemtable[hash]; item != NULL; )
		{
			output_item *next = item->next;

			/* free the name and the item */
			global_free(item);
			item = next;
		}

	/* remove all global notifiers */
	global_notifylist.reset();
}


/*-------------------------------------------------
    output_set_value - set the value of an output
-------------------------------------------------*/

void output_set_value(const char *outname, INT32 value)
{
	output_item *item = find_item(outname);
	INT32 oldval;

	/* if no item of that name, create a new one and send the item's state */
	if (item == NULL)
	{
		item = create_new_item(outname, value);
		oldval = value + 1;
	}

	else
	{
		/* set the new value */
		oldval = item->value;
		item->value = value;
	}

	/* if the value is different, signal the notifier */
	if (oldval != value)
	{
		/* call the local notifiers first */
		for (output_notify *notify = item->notifylist.first(); notify != NULL; notify = notify->next())
			(*notify->m_notifier)(outname, value, notify->m_param);

		/* call the global notifiers next */
		for (output_notify *notify = global_notifylist.first(); notify != NULL; notify = notify->next())
			(*notify->m_notifier)(outname, value, notify->m_param);
	}
}


/*-------------------------------------------------
    output_set_indexed_value - set the value of an
    indexed output
-------------------------------------------------*/

void output_set_indexed_value(const char *basename, int index, int value)
{
	char buffer[100];
	char *dest = buffer;

	/* copy the string */
	while (*basename != 0)
		*dest++ = *basename++;

	/* append the index */
	if (index >= 1000) *dest++ = '0' + ((index / 1000) % 10);
	if (index >= 100) *dest++ = '0' + ((index / 100) % 10);
	if (index >= 10) *dest++ = '0' + ((index / 10) % 10);
	*dest++ = '0' + (index % 10);
	*dest++ = 0;

	/* set the value */
	output_set_value(buffer, value);
}


/*-------------------------------------------------
    output_get_value - return the value of an
    output
-------------------------------------------------*/

INT32 output_get_value(const char *outname)
{
	output_item *item = find_item(outname);

	/* if no item, value is 0 */
	if (item == NULL)
		return 0;
	return item->value;
}


/*-------------------------------------------------
    output_get_indexed_value - get the value of an
    indexed output
-------------------------------------------------*/

INT32 output_get_indexed_value(const char *basename, int index)
{
	char buffer[100];
	char *dest = buffer;

	/* copy the string */
	while (*basename != 0)
		*dest++ = *basename++;

	/* append the index */
	if (index >= 1000) *dest++ = '0' + ((index / 1000) % 10);
	if (index >= 100) *dest++ = '0' + ((index / 100) % 10);
	if (index >= 10) *dest++ = '0' + ((index / 10) % 10);
	*dest++ = '0' + (index % 10);
	*dest++ = 0;

	/* set the value */
	return output_get_value(buffer);
}


/*-------------------------------------------------
    output_set_notifier - sets a notifier callback
    for a particular output, or for all outputs
    if NULL is specified
-------------------------------------------------*/

void output_set_notifier(const char *outname, output_notifier_func callback, void *param)
{
	/* if an item is specified, find it */
	if (outname != NULL)
	{
		output_item *item = find_item(outname);

		/* if no item of that name, create a new one */
		if (item == NULL)
			item = create_new_item(outname, 0);
		item->notifylist.append(*global_alloc(output_notify(callback, param)));
	}
	else
		global_notifylist.append(*global_alloc(output_notify(callback, param)));
}


/*-------------------------------------------------
    output_notify_all - immediately call the given
    notifier for all outputs
-------------------------------------------------*/

void output_notify_all(output_notifier_func callback, void *param)
{
	output_item *item;
	int hash;

	/* remove all items */
	for (hash = 0; hash < HASH_SIZE; hash++)
		for (item = itemtable[hash]; item != NULL; item = item->next)
			(*callback)(item->name.c_str(), item->value, param);
}


/*-------------------------------------------------
    output_name_to_id - returns a unique ID for
    a given name
-------------------------------------------------*/

UINT32 output_name_to_id(const char *outname)
{
	output_item *item = find_item(outname);

	/* if no item, ID is 0 */
	if (item == NULL)
		return 0;
	return item->id;
}


/*-------------------------------------------------
    output_id_to_name - returns a name that maps
    to a given unique ID
-------------------------------------------------*/

const char *output_id_to_name(UINT32 id)
{
	output_item *item;
	int hash;

	/* remove all items */
	for (hash = 0; hash < HASH_SIZE; hash++)
		for (item = itemtable[hash]; item != NULL; item = item->next)
			if (item->id == id)
				return item->name.c_str();

	/* nothing found, return NULL */
	return NULL;
}

// convert 'long long' into decimal or hex
// 4 per conversion call, make round robin bigger if needed
// as thread safe as I64FMT calls being replaced
// assumes unsigned. A quick glance at all I64FMT being replaced
// yielded nothing but positive values


char *I64_to_hex_unpadded(UINT64 i) {
	static int which = 0;
	static char retstring[4][24];
	static char *r;
	static char lowhalf[10];
	UINT64 t;
	t = i&0xffffffff00000000LL;
	which = (which+1) & 3;
	r = (char *)&retstring[which];
	if (t == 0) {
		sprintf(r,"%x",(int)(i&0xffffffffLL));
		return r;
	} else {
		sprintf(r,"%x",(int)(t>>32LL));
		sprintf(lowhalf,"%08x",(int)(i&0xffffffffLL));
		strcat(r,lowhalf);
		return r;
	}
}

char *I64_to_hex_padded(UINT64 i) {
	static int which = 0;
	static char retstring[4][24];
	static char *r;
	static char lowhalf[10];
	UINT64 t;
	t = i&0xffffffff00000000LL;
	which = (which+1) & 3;
	r = (char *)&retstring[which];
	if (t == 0) {
		sprintf(r,"%016x",(int)(i&0xffffffffLL));
		return r;
	} else {
		sprintf(r,"%08x",(int)(t>>32LL));
		sprintf(lowhalf,"%08x",(int)(i&0xffffffffLL));
		strcat(r,lowhalf);
		return r;
	}
}


char *I64_to_base10(UINT64 i) {
	static int which = 0;
	static char retstring[4][24];
	static char *r;
	static std::string temp;
	static std::ostringstream s;
 // reset static stream to first char. \0 required because some versions of VS
 // can't seekp until a char has been inserted into string
	s << '\0'; s.seekp(std::ios::beg);
	s << i;
	which = (which+1) & 3;
	r = (char *) &retstring[which];
	temp = s.str();
	std::strcpy(r, temp.c_str());
	return r;
}
