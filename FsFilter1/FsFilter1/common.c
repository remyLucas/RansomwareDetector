#include "common.h"

int is_standard_file(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects)
{

	if (FltObjects->FileObject != NULL)
	{
		if (FltObjects->FileObject->Flags & (FO_NAMED_PIPE | FO_STREAM_FILE | FO_MAILSLOT | FO_VOLUME_OPEN))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
		return 0;
}

ULONG is_critical_file(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects)
{
	int pos;
	PFLT_FILE_NAME_INFORMATION info;
	ULONG hash;

	if (is_standard_file(Data, FltObjects))
	{
		if (get_file_info(Data, &info) == 0)
		{
			pos = get_extension(info->Name);
			RtlHashUnicodeString(&info->Name, FALSE, HASH_STRING_ALGORITHM_DEFAULT, &hash);
			FltReleaseFileNameInformation(info);

			if (pos != -1 || file_exist_for_current_process(&process_trace_list, hash))
			{
#ifdef LOG
				if (file_exist_for_current_process(&process_trace_list,hash) == 0)
				{
					log_new_file(hash, info->Name, FltObjects->Instance);
				}
#endif
				if(pos != -1)
					add_current_process_trace_extension(&process_trace_list, pos,hash,FltObjects->Instance);
				
				return hash;
			}
		}
	}

	return 0;
}

int get_proc_img_by_pid(HANDLE pid, PUNICODE_STRING img, ULONG size)
{
	NTSTATUS status;
	HANDLE proc_handle;
	OBJECT_ATTRIBUTES obj_attr;
	CLIENT_ID cid;

	cid.UniqueProcess = pid;
	cid.UniqueThread = NULL;

	InitializeObjectAttributes(&obj_attr, NULL, 0, NULL, NULL);
	status = ZwOpenProcess(&proc_handle, PROCESS_ALL_ACCESS, &obj_attr, &cid);

	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwOpenProcess failed : %x\n", status);
		return -1;
	}

	status = ZwQueryInformationProcess(proc_handle, ProcessImageFileName, img, size, NULL);
	
	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwOpenProcess failed : %x\n", status);
		return -1;
	}

	ZwClose(proc_handle);

	return 0;
}

int get_file_info(PFLT_CALLBACK_DATA Data, PFLT_FILE_NAME_INFORMATION *info)
{
	NTSTATUS status;

	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, info);

	if (status == STATUS_FLT_NAME_CACHE_MISS)
	{
		status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_FILESYSTEM_ONLY, info);
	}

	if (status == STATUS_SUCCESS)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}