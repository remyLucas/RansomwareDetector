#pragma once
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <ntddk.h>
#include <stdlib.h>

#define DEFAULT_SIZE_EXTENSION_ALLOCATED 300
#define DEFAULT_NBR_LISTED_EXTENSION_ALLOCATED 100
#define DEFAULT_NBR_EXTENSION_SET_ALLOCATED 20

typedef struct extension_set
{
	UINT8 tab[1];
}extension_set;

typedef struct extension_list
{
	WCHAR *extensions;
	UINT32 extension_size_allocated,extension_size_used;
	WCHAR **list;
	UINT32 list_size,list_size_allocated;
	UINT32 max_len;

	UINT32 nbr_extension_set_allocated;
	UINT32 nbr_extension_set_used;
	extension_set *set_list;
}extension_list;

extension_list ext;

int load_default_extensions(extension_list *e);
int filter_extensions(extension_list *e, PUNICODE_STRING s);
void move_extension_set_pointers(extension_set *, extension_set *);
int is_not_already_used(extension_set*, int);
extension_set* new_extension_set();