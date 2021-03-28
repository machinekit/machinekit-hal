//    Copyright 2003-2007, various authors
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef RTAPI_APP_H
#define RTAPI_APP_H

/*
  for Linux kernel modules, exactly one file needs to
  include <linux/module.h>. We put this in this header.
  If we ever support non-Linux platforms, this file will
  get full of ifdefs.
*/

#include "user_pci/module.h"

/*  Turn the first instance of rtapi_app_* into a function declaration, then
    export the symbol, then re-create the function definition.  This way the
    symbol is only exported if the function exists in the code, and we don't
    have to have the ability to 'rewind' the C preprocessor.
 */
#define rtapi_app_main(a)           \
    rtapi_app_main(a);              \
    EXPORT_SYMBOL(rtapi_app_main);  \
    int rtapi_app_main(a)
#define rtapi_app_exit(a)           \
    rtapi_app_exit(a);              \
    EXPORT_SYMBOL(rtapi_app_exit);  \
    void rtapi_app_exit(a)

#endif /* RTAPI_APP_H */
