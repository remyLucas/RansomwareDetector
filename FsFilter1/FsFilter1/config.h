#pragma once
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <ntddk.h>
#include <stdlib.h>
#include "log.h"

#define DEFAULT_NBR_FILE_ALLOCATED 16
#define DEFAULT_SIZE_PATHS_ALLOCATED 256

typedef struct file_filtered
{
	UNICODE_STRING path;
	//flags
}file_filtered;

typedef struct file_list
{
	file_filtered *files;
	UINT32 nbr_files, nbr_allocated_files;
	WCHAR *paths;
	UINT32 paths_size_used, paths_size_allocated;
}file_list;

file_list honey_pot;

int init_file_list(file_list *l);

int realloc_file_list_files(file_list *l);

int realloc_file_list_paths(file_list *l);

int add_file(file_list *l, HANDLE config_handle, LARGE_INTEGER *pos);

int load_config(PFLT_FILTER filter_handle, PUNICODE_STRING registery_path);

int is_honey_pot_file(PFLT_FILE_NAME_INFORMATION info, file_list *l);

int terminate_current_process();