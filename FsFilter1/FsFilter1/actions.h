#pragma once
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <ntddk.h>
#include <stdlib.h>
#include <Ntstrsafe.h>
#include <Wdm.h>
#include "extension.h"

#define NBR_ACTIONS 10
#define DEFAULT_NBR_ACTIONS_ALLOCATED DEFAULT_NBR_EXTENSION_SET_ALLOCATED
#define ACTIONS_ARRAY_SIZE 1024

typedef enum action_types
{
	ACTION_READ = 0,
	ACTION_WRITE = 1,
	ACTION_OPEN = 2,
	ACTION_CREATE = 3,
	ACTION_DELETE = 4,
	ACTION_CLOSE = 5,
	ACTION_QUERY_DIR = 6,
	ACTION_RENAME = 7,
	ACTION_EXTENSION = 8,
	ACTION_RENAME_EXT = 9
}action_types;

typedef struct action
{
	action_types type;
	ULARGE_INTEGER timestamp;
	ULONG file_name_hash;
}action;

typedef struct file_element
{
	ULONG hash;
	UINT32 nbr;
}file_element;

//note : for read and write trace maybe count the number of bytes
typedef struct actions
{
	UINT32 nbr_proc;
	HANDLE ppid;

	action actions_list[ACTIONS_ARRAY_SIZE];
	UINT32 cursor,size;

	file_element file_list[ACTIONS_ARRAY_SIZE];
	UINT32 file_list_size;

	extension_set *extensions_openned;

}actions;

actions *actions_array;
UINT32 nbr_actions_allocated;
UINT32 nbr_actions_used;

KGUARDED_MUTEX action_mutex;

void move_action_set_pointer(actions *old, actions *new);
actions *new_actions(HANDLE ppid);