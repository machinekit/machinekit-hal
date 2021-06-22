/* Copyright (C) 2006-2008 Jeff Epler <jepler@unpythonic.net>
 * Copyright (C) 2012-2014 Michael Haberler <license@mah.priv.at>
 * Copyright (C) 2020 John Morris <john@dovetail-automata.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "rtapi_compat.h" // get_elf_section()
#include <limits.h>       // PATH_MAX
#include <string>

// dlinfo()
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <link.h>

class Module
{
  public:
    string name;
    void *handle;
    char *errmsg;

    Module() = default;
    ~Module();

    int load(string module);
    string path();
    void clear_err();
    template <class T> T sym(const string &name);
    template <class T> T sym(const char *name);
    int elf_section(const char *section_name, void **dest);
    int close();
};

int Module::load(string module)
{
    char module_path[PATH_MAX];
    bool is_rpath;
    string dlpath;
    struct stat st;

    // For module given as a path (sans `.so`), use the path basename as the
    // module name
    if (module.find_last_of("/") != string::npos) {
        name = module.substr(module.find_last_of("/") + 1);
        is_rpath = false; // `module` contains `/` chars
    } else {
        name = module;
        is_rpath = true; // `module` contains no `/` chars
    }
    dlpath = "mod" + module + ".so";
    handle = NULL;

    // First look in `$MK_MODULE_DIR` if `module` contains no `/` chars
    if (getenv("MK_MODULE_DIR") != NULL && is_rpath) {
        // If $MK_MODULE_DIR/module.so exists, load it (or fail)
        strncpy(module_path, getenv("MK_MODULE_DIR"), PATH_MAX - 1);
        strncat(module_path, ("/" + dlpath).c_str(), PATH_MAX - 1);
        if (stat(module_path, &st) == 0) {
            handle = dlopen(module_path, RTLD_GLOBAL | RTLD_NOW);
            if (!handle) {
                errmsg = dlerror();
                return -1;
            }
        }
    }
    // Otherwise load it with dlopen()'s logic (or fail)
    if (!handle) {
        handle = dlopen(dlpath.c_str(), RTLD_GLOBAL | RTLD_NOW);
    }
    if (!handle) {
        errmsg = dlerror();
        rtapi_print_msg(RTAPI_MSG_ERR, "load(%s): %s", module.c_str(), errmsg);
        return -1;
    }

    // Success
    errmsg = NULL;
    return 0;
}

string Module::path()
{
    if (!handle) {
        rtapi_print_msg(RTAPI_MSG_ERR,
                        "Trying to get path of non-loaded module %s\n",
                        name.c_str());
        return "";
    }

    struct link_map *map;
    dlinfo(handle, RTLD_DI_LINKMAP, &map);
    if (!map) {
        rtapi_print_msg(RTAPI_MSG_ERR, "dlinfo(%s, %s):  %s", name.c_str(),
                        "RTLD_DI_LINKMAP", dlerror());
        return NULL;
    }

    // Should not happen, however passing nullptr to constructor of std::string
    // is undefined
    if (map->l_name == nullptr) {
        rtapi_print_msg(RTAPI_MSG_ERR, "link_map->l_name for %s is nullptr!",
                        name.c_str());
        return NULL;
    }
    return std::string(map->l_name);
}

void Module::clear_err()
{
    dlerror();
    errmsg = NULL;
}

template <class T> T Module::sym(const char *sym_name)
{
    if (!handle) {
        rtapi_print_msg(RTAPI_MSG_ERR,
                        "Trying to sym symbol on non-loaded module %s\n",
                        name.c_str());
        return nullptr;
    }
    clear_err();
    T res = (T)(dlsym(handle, sym_name));
    if (!res) errmsg = dlerror();
    return res;
}

template <class T> T Module::sym(const string &sym_name)
{
    return sym<T>(sym_name.c_str());
}

int Module::close()
{
    if (handle) {
        return dlclose(handle);
    }
    rtapi_print_msg(RTAPI_MSG_ERR, "Trying to unload not loaded module %s\n",
                    name.c_str());
    return -1;
}

Module::~Module() { close(); }

int Module::elf_section(const char *section_name, void **dest)
{
    return get_elf_section(path().c_str(), section_name, dest);
}
