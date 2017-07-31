#pragma once

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <ntddk.h>
#include <stdlib.h>
#include <Ntstrsafe.h>
#include <Wdm.h>
#include "actions.h"
#include "common.h"

#define LOG 1
#define NBR_FILE_KEEP 20

typedef struct log_file
{
	HANDLE file_handle;
	PFILE_OBJECT file_obj;
}log_file;

log_file actions_log_file, pid_log_file, filename_log_file;
PFLT_FILTER gFilterHandle;
PFLT_INSTANCE log_instance;

int open_log_file(WCHAR *base_dir, PFLT_FILTER filter_handle);
int log_actions(action_types a, ULONG hash, PFLT_INSTANCE instance);
int log_img_name_of_current_process(PFLT_INSTANCE instance);
int log_pid(HANDLE pid, HANDLE ppid, PFLT_INSTANCE instance);
int log_new_file(ULONG hash, UNICODE_STRING name, PFLT_INSTANCE instance);