/***************************************************************************

    mewui/inifile.c

    MEWUI inifile system.

***************************************************************************/

#include "emu.h"
#include "emuopts.h"
#include "drivenum.h"
#include "mewui/inifile.h"
#include "mewui/utils.h"
#include <fstream>

//-------------------------------------------------
//  GLOBAL VARIABLES
//-------------------------------------------------

IniFileIndex *inifile_manager::first_file_idx = NULL;
IniFileIndex *inifile_manager::last_file_idx = NULL;
IniFileIndex *inifile_manager::current_file_idx = NULL;
int inifile_manager::file_total = 0;
static bool first_load = true;
static bool first_f_load = true;

std::vector<ui_software_info> favorite_manager::favorite_list;
int favorite_manager::current_favorite = -1;

static const char *favorite_filename = "favorites.ini";

//-------------------------------------------------
//  ctor
//-------------------------------------------------

inifile_manager::inifile_manager(running_machine &machine)
    : m_machine(machine)
{
    if (first_load)
    {
        directory_scan();
        first_load = false;
    }
}

//-------------------------------------------------
//  scan directories and create index
//-------------------------------------------------

void inifile_manager::directory_scan()
{
    // open extra INIs folder
    file_enumerator path(machine().options().extraini_path());
    const osd_directory_entry *dir;

    // loop into folder's file
    while ((dir = path.next()) != NULL)
    {
        int length = strlen(dir->name);
        std::string file_name(dir->name);

        // skip mewui_favorite file
        if (!core_stricmp("mewui_favorite.ini", file_name.c_str()))
            continue;

        // check .ini file ending
        if ((length > 4) && dir->name[length - 4] == '.' && tolower((UINT8)dir->name[length - 3]) == 'i' &&
            tolower((UINT8)dir->name[length - 2]) == 'n' && tolower((UINT8)dir->name[length - 1]) == 'i')
        {
            // try to open file and indexing
            if (ParseOpen(file_name.c_str()))
            {
                IniFileIndex *tmp = allocate(file_name.c_str());
                int category_count = init_category(*tmp, file_name.c_str());
                ParseClose();

                // if no category, deallocate memory
                if (category_count == 0)
                    global_free(tmp);

                // else, sort category, copy file name and category number
                else
                {
                    tmp->category_number = category_count;
                    file_total++;
                }
            }
        }
    }
}

//-------------------------------------------------
//  initialize category
//-------------------------------------------------

int inifile_manager::init_category(IniFileIndex &index, const char *file_name)
{
    char    readbuf[MAX_CHAR_INFO];
    char    name[80];
    int     count = 0;

    while (fp->gets(readbuf, MAX_CHAR_INFO))
    {
        // do we have [...] ?

        if (readbuf[0] == '[')
        {
            int ch = 1;

            while (readbuf[ch] != ']')
            {
                name[ch - 1] = readbuf[ch];
                ch++;
            }

            name[ch - 1] = '\0';

            // is it [FOLDER_SETTINGS] or [ROOT_FOLDER] ?

            if (!strcmp(name, "FOLDER_SETTINGS"))
                continue;
            else
            {
                long offset = fp->tell();
                if (!strcmp(name, "ROOT_FOLDER"))
                {
                    long offset = fp->tell();
                    fp->gets(readbuf, MAX_CHAR_INFO);

                    if (isspace(readbuf[0])) continue;

                    int len = strlen(file_name) - 4;
                    strncpy(name, file_name, len);
                    name[len] = '\0';
                    add_item(index, name, &offset);
                    fp->seek(offset, SEEK_SET);
                }

                // must be [folder name]
                else
                    add_item(index, name, &offset);

                count++;
            }
        }
    }

    return count;
}

//-------------------------------------------------
//  closes the existing opened file (if any)
//-------------------------------------------------

void inifile_manager::load_ini_category(std::vector<int> &temp_filter)
{
    if (!has_files())
        return;

    bool search_clones = false;
    std::string file_name(current_file_idx->file_name);
    long offset = current_file_idx->current_category->offset;

    if (!core_stricmp(file_name.c_str(), "category.ini") || !core_stricmp(file_name.c_str(), "alltime.ini"))
        search_clones = true;

    if (ParseOpen(file_name.c_str()))
    {
        int num_game = driver_list::total();
        char readbuf[MAX_CHAR_INFO];

        fp->seek(offset, SEEK_SET);

        while (fp->gets(readbuf, MAX_CHAR_INFO))
        {
            if (readbuf[0] == '[')
                break;

            char name[80];
            int li = strlen(readbuf) - 1;
            strncpy(name, readbuf, li);
            name[li] = '\0';
            int dfind = driver_list::find(name);

            // if game found and file is category.ini, add clones to the list
            if (dfind != -1 && search_clones)
            {
                temp_filter.push_back(dfind);

                int clone_of = driver_list::non_bios_clone(dfind);

                if (clone_of == -1)
                {
                    for (int x = 0; x < num_game; x++)
                        if (!strcmp(driver_list::driver(x).parent, name))
                            temp_filter.push_back(x);
                }
            }

            else if (dfind != -1)
                temp_filter.push_back(dfind);
        }

        ParseClose();
    }

    return;
}

//-------------------------------------------------
//  open up file for reading
//-------------------------------------------------

bool inifile_manager::ParseOpen(const char *filename)
{
    // Open file up in binary mode
    fp = global_alloc(emu_file(machine().options().extraini_path(), OPEN_FLAG_READ));

    if (fp->open(filename) != FILERR_NONE)
    {
        global_free(fp);
        fp = NULL;
        return FALSE;
    }

    return TRUE;
}

//-------------------------------------------------
//  closes the existing opened file (if any)
//-------------------------------------------------

void inifile_manager::ParseClose()
{
    // If the file is open, time for fclose.
    if (fp)
    {
        fp->close();
        global_free(fp);
    }

    fp = NULL;
}

//-------------------------------------------------
//  free allocated indices
//-------------------------------------------------

void inifile_manager::free_ini_index()
{
    // free ini file index
    if (first_file_idx)
    {
        IniFileIndex *pItem = first_file_idx;

        while (pItem != NULL)
        {
            IniCategoryIndex *pItems = pItem->first_category;

            while (pItems != NULL)
            {
                IniCategoryIndex *pDelItems = pItems;
                pItems = pItems->next;
                global_free_array(pDelItems->name);
                global_free(pDelItems);
            }

            pItem->first_category = pItem->last_category = NULL;
            IniFileIndex *pDelItem = pItem;
            pItem = pItem->next;
            global_free_array(pDelItem->file_name);
            global_free(pDelItem);
        }

        first_file_idx = last_file_idx = NULL;
    }
}

//-------------------------------------------------
//  add a category structure to file index
//-------------------------------------------------

void inifile_manager::add_item(IniFileIndex &tmp, const char *name, long *file_offset)
{
    IniCategoryIndex *tmp_item = global_alloc(IniCategoryIndex);

    tmp_item->name = global_alloc_array(char, strlen(name) + 1);
    strcpy(tmp_item->name, name);
    tmp_item->offset = *file_offset;

    if (tmp.first_category == NULL)
    {
        tmp.first_category = tmp.last_category = tmp.current_category = tmp_item;
        tmp_item->next = tmp_item->prev = NULL;
    }

    else
    {
        tmp.last_category->next = tmp_item;
        tmp_item->prev = tmp.last_category;
        tmp.last_category = tmp_item;
        tmp_item->next = NULL;
    }
}

//-------------------------------------------------
//  add a file to the index
//-------------------------------------------------

IniFileIndex *inifile_manager::allocate(const char *name)
{
    IniFileIndex *tmp = global_alloc(IniFileIndex);
    tmp->file_name = global_alloc_array(char, strlen(name) + 1);
    strcpy(tmp->file_name, name);
    tmp->first_category = tmp->last_category = tmp->current_category = NULL;

    if (first_file_idx == NULL)
    {
        first_file_idx = last_file_idx = current_file_idx = tmp;
        tmp->next = tmp->prev = NULL;
    }

    else
    {
        last_file_idx->next = tmp;
        tmp->prev = last_file_idx;
        last_file_idx = tmp;
        tmp->next = NULL;
    }

    return tmp;
}

//-------------------------------------------------
//  set current file
//-------------------------------------------------

void inifile_manager::setfile(int direction, int index)
{
    if (index == -1)
    {
        if (direction == -1)
            current_file_idx = current_file_idx->prev;
        else if (direction == 1)
            current_file_idx = current_file_idx->next;
        else
            current_file_idx = first_file_idx;
    }

    else
    {
        current_file_idx = first_file_idx;

        for (int x = 0; x != index; x++, current_file_idx = current_file_idx->next);
    }
}

//-------------------------------------------------
//  set current category
//-------------------------------------------------

void inifile_manager::setcategory(int direction, int index)
{
    if (index == -1)
    {
        if (direction == -1)
            current_file_idx->current_category = current_file_idx->current_category->prev;
        else if (direction == 1)
            current_file_idx->current_category = current_file_idx->current_category->next;
        else
            current_file_idx->current_category = current_file_idx->first_category;
    }

    else
    {
        current_file_idx->current_category = current_file_idx->first_category;

        for (int x = 0; x != index; x++, current_file_idx->current_category = current_file_idx->current_category->next);
    }
}

//-------------------------------------------------
//  get index of current file
//-------------------------------------------------

int inifile_manager::getfileindex()
{
    int index = 0;

    for (IniFileIndex *tmp = first_file_idx; tmp != current_file_idx; tmp = tmp->next, index++);

    return index;
}

//-------------------------------------------------
//  get index of current category
//-------------------------------------------------

int inifile_manager::getcategoryindex()
{
    int index = 0;

    for (IniCategoryIndex *tmp = current_file_idx->first_category; tmp != current_file_idx->current_category; tmp = tmp->next, index++);

    return index;
}

/**************************************************************************
    FAVORITE MANAGER
**************************************************************************/

//-------------------------------------------------
//  ctor
//-------------------------------------------------

favorite_manager::favorite_manager(running_machine &machine)
    : m_machine(machine)
{
    if (first_f_load)
    {
        parse_favorite();
        first_f_load = false;
    }
}

//-------------------------------------------------
//  add a game
//-------------------------------------------------

void favorite_manager::add_favorite_game(const game_driver *driver)
{
    ui_software_info tmpmatches;

    tmpmatches.shortname.assign(driver->name);
    tmpmatches.longname.assign(driver->description);
    tmpmatches.parentname.clear();
    tmpmatches.year.clear();
    tmpmatches.publisher.clear();
    tmpmatches.supported = 0;
    tmpmatches.part.clear();
    tmpmatches.driver = driver;
    tmpmatches.listname.clear();
    tmpmatches.interface.clear();
    tmpmatches.instance.clear();
    tmpmatches.startempty = 1;
    tmpmatches.parentlongname.clear();
    tmpmatches.usage.clear();
    tmpmatches.devicetype.clear();
    tmpmatches.available = true;

    favorite_list.push_back(tmpmatches);
    save_favorite_games();
}

//-------------------------------------------------
//  add a system
//-------------------------------------------------

void favorite_manager::add_favorite_game(ui_software_info &swinfo)
{
    favorite_list.push_back(swinfo);
    save_favorite_games();
}

//-------------------------------------------------
//  add a game / system
//-------------------------------------------------

void favorite_manager::add_favorite_game()
{
    if ((machine().system().flags & GAME_TYPE_ARCADE) != 0)
    {
        add_favorite_game(&machine().system());
        return;
    }

    ui_software_info tmpmatches;

    image_interface_iterator iter(machine().root_device());
    bool software_avail = false;

    for (device_image_interface *image = iter.first(); image != NULL; image = iter.next())
    {
        // get the base name
        if (image->basename() != NULL)
        {
            const software_info *swinfo = image->software_entry();

                if (swinfo->shortname()) tmpmatches.shortname.assign(swinfo->shortname());
                if (swinfo->longname()) tmpmatches.longname.assign(swinfo->longname());
                if (swinfo->parentname()) tmpmatches.parentname.assign(swinfo->parentname());
                if (swinfo->year()) tmpmatches.year.assign(swinfo->year());
                if (swinfo->publisher()) tmpmatches.publisher.assign(swinfo->publisher());
                tmpmatches.supported = swinfo->supported();
                if (image->part_entry()->name()) tmpmatches.part.assign(image->part_entry()->name());
                tmpmatches.driver = &machine().system();
                if (image->software_list_name()) tmpmatches.listname.assign(image->software_list_name());
                if (image->part_entry()->interface()) tmpmatches.interface.assign(image->part_entry()->interface());
                tmpmatches.instance.clear();
                tmpmatches.startempty = 0;
                tmpmatches.parentlongname.clear();
                tmpmatches.usage.clear();
                if (image->image_type_name()) tmpmatches.devicetype.assign(image->image_type_name());
                tmpmatches.available = true;

                software_avail = true;
        }
    }

    if (!software_avail)
    {
        add_favorite_game(&machine().system());
        return;
    }

    favorite_list.push_back(tmpmatches);
    save_favorite_games();
}

//-------------------------------------------------
//  remove a favorite from list
//-------------------------------------------------

void favorite_manager::remove_favorite_game(ui_software_info &swinfo)
{
	for (size_t x = 0; x < favorite_list.size(); x++)
		if (favorite_list[x] == swinfo)
		{
			favorite_list.erase(favorite_list.begin() + x);
			break;
		}

    save_favorite_games();
}

void favorite_manager::remove_favorite_game()
{
	favorite_list.erase(favorite_list.begin() + current_favorite);
    save_favorite_games();
}
//-------------------------------------------------
//  check if game is already in favorite list
//-------------------------------------------------

bool favorite_manager::isgame_favorite()
{
    if ((machine().system().flags & GAME_TYPE_ARCADE) != 0)
        return isgame_favorite(&machine().system());

    image_interface_iterator iter(machine().root_device());
    bool image_loaded = false;

    for (device_image_interface *image = iter.first(); image != NULL; image = iter.next())
    {
        if (image->filename() != NULL)
        {
            image_loaded = true;
            const software_info *swinfo = image->software_entry();
            const software_part *swpart = image->part_entry();

            for (size_t current = 0; current < favorite_list.size(); current++)
                if (favorite_list[current].shortname.compare(swinfo->shortname()) == 0)
                    if (swpart != NULL)
                        if (swpart->name() != NULL && favorite_list[current].part.compare(swpart->name()) == 0)
                        {
                            current_favorite = current;
                            return true;
                        }
        }
    }

    if (!image_loaded)
        for (size_t current = 0; current < favorite_list.size(); current++)
            if (favorite_list[current].driver == &machine().system())
            {
                current_favorite = current;
                return true;
            }

    current_favorite = -1;
    return false;
}

//-------------------------------------------------
//  check if game is already in favorite list
//-------------------------------------------------

bool favorite_manager::isgame_favorite(const game_driver *driver)
{
    for (size_t x = 0; x < favorite_list.size(); x++)
        if (favorite_list[x].driver == driver && favorite_list[x].shortname.compare(driver->name) == 0)
        {
            current_favorite = x;
            return true;
        }

    current_favorite = -1;
    return false;
}

//-------------------------------------------------
//  check if game is already in favorite list
//-------------------------------------------------

bool favorite_manager::isgame_favorite(ui_software_info &swinfo)
{
    for (size_t x = 0; x < favorite_list.size(); x++)
		if (favorite_list[x] == swinfo)
        {
            current_favorite = x;
            return true;
        }

    current_favorite = -1;
    return false;
}

//-------------------------------------------------
//  parse favorite file
//-------------------------------------------------

void favorite_manager::parse_favorite()
{
    if (parseOpen(favorite_filename))
    {

        std::ifstream myfile(fp->fullpath());
        std::string readbuf;

        std::getline(myfile, readbuf);
        while (readbuf[0] == '[')
            std::getline(myfile, readbuf);

        while (std::getline(myfile, readbuf))
        {
            ui_software_info tmpmatches;

            tmpmatches.shortname = readbuf;
            std::getline(myfile, tmpmatches.longname);
            std::getline(myfile, tmpmatches.parentname);
            std::getline(myfile, tmpmatches.year);
            std::getline(myfile, tmpmatches.publisher);
            std::getline(myfile, readbuf);
            tmpmatches.supported = readbuf[0] - '0';
            std::getline(myfile, tmpmatches.part);
            std::getline(myfile, readbuf);
            int dx = driver_list::find(readbuf.c_str());
            tmpmatches.driver = &driver_list::driver(dx);
            std::getline(myfile, tmpmatches.listname);
            std::getline(myfile, tmpmatches.interface);
            std::getline(myfile, tmpmatches.instance);
            std::getline(myfile, readbuf);
            tmpmatches.startempty = readbuf[0] - '0';
            std::getline(myfile, tmpmatches.parentlongname);
            std::getline(myfile, tmpmatches.usage);
            std::getline(myfile, tmpmatches.devicetype);
            std::getline(myfile, readbuf);
            tmpmatches.available = readbuf[0] - '0';

            favorite_list.push_back(tmpmatches);
        }
        myfile.close();
        parseClose();
    }
}

//-------------------------------------------------
//  save favorite
//-------------------------------------------------

void favorite_manager::save_favorite_games()
{
    // if the list is empty, then deletes the file
    if (favorite_list.empty())
    {
        if (parseOpen(favorite_filename))
        {
            fp->remove_on_close();
            parseClose();
        }
        return;
    }

    // attempt to open the output file
    emu_file file(MEWUI_DIR, OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);

    if (file.open(favorite_filename) == FILERR_NONE)
    {
        std::string filename(file.fullpath());
        file.close();

        std::ofstream myfile(filename.c_str());

        // generate the favorite INI
        std::string favtext;
        std::string headtext("[ROOT_FOLDER]\n[Favorite]\n\n");
        myfile << headtext;

        for (size_t current = 0; current < favorite_list.size(); current++)
        {
            myfile << favorite_list[current].shortname << "\n";
            myfile << favorite_list[current].longname << "\n";
            myfile << favorite_list[current].parentname << "\n";
            myfile << favorite_list[current].year << "\n";
            myfile << favorite_list[current].publisher << "\n";
            strprintf(favtext, "%d\n", favorite_list[current].supported);
            myfile << favtext;
            myfile << favorite_list[current].part << "\n";
            myfile << favorite_list[current].driver->name << "\n";
            myfile << favorite_list[current].listname << "\n";
            myfile << favorite_list[current].interface << "\n";
            myfile << favorite_list[current].instance << "\n";
            strprintf(favtext, "%d\n", favorite_list[current].startempty);
            myfile << favtext;
            myfile << favorite_list[current].parentlongname << "\n";
            myfile << favorite_list[current].usage << "\n";
            myfile << favorite_list[current].devicetype << "\n";
            strprintf(favtext, "%d\n", favorite_list[current].available);
            myfile << favtext;
        }
        myfile.close();
    }
}

//-------------------------------------------------
//  open up file for reading
//-------------------------------------------------

bool favorite_manager::parseOpen(const char *filename)
{
    // Open file up in binary mode
    fp = global_alloc(emu_file(MEWUI_DIR, OPEN_FLAG_READ));

    if (fp->open(filename) != FILERR_NONE)
    {
        global_free(fp);
        fp = NULL;
        return FALSE;
    }

    return TRUE;
}

//-------------------------------------------------
//  closes the existing opened file (if any)
//-------------------------------------------------

void favorite_manager::parseClose()
{
    // If the file is open, time for fclose.
    if (fp)
    {
        fp->close();
        global_free(fp);
    }

    fp = NULL;
}
