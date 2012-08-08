// -*- tab-width:4 ; indent-tabs-mode:nil -*-
/**
\file table.h

Provides replicated value tables.

*/

#include <hre/user.h>
#include <util-lib/tables.h>

#ifndef HRE_TABLE_H
#define HRE_TABLE_H

/// create a value table, which is synchronized across the context.
extern value_table_t HREcreateTable(hre_context_t ctx,const char* name);

/** Grey box newmap wrapper for table create.
    Use hre_context as newmap_context;
 */
extern void* HREgreyboxNewmap(void*newmap_context);

extern int HREgreyboxC2I(void*map,void*data,int len);

extern void* HREgreyboxI2C(void*map,int idx,int*len);

extern int HREgreyboxCount(void*map);

#endif

