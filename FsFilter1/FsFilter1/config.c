#include "config.h"

int init_file_list(file_list *l)
{
	l->files = ExAllocatePool(PagedPool, sizeof(file_filtered)*DEFAULT_NBR_FILE_ALLOCATED);
	if (l->files == NULL)
		return -1;
	l->nbr_files = 0;
	l->nbr_allocated_files = DEFAULT_NBR_FILE_ALLOCATED;

	l->paths = ExAllocatePool(PagedPool, DEFAULT_SIZE_PATHS_ALLOCATED);
	if (l->paths == NULL)
		return -1;
	l->paths_size_used = 0;
	l->paths_size_allocated = DEFAULT_SIZE_PATHS_ALLOCATED;

	return 0;
}

int realloc_file_list_files(file_list *l)
{
	file_filtered *f;

	f = ExAllocatePool(PagedPool, sizeof(file_filtered)*l->nbr_allocated_files * 2);

	if (f == NULL)
		return -1;

	RtlCopyMemory(f, l->files, sizeof(file_filtered)*l->nbr_allocated_files);
	ExFreePool(l->files);

	l->files = f;
	l->nbr_allocated_files *= 2;

	return 0;
}

int realloc_file_list_paths(file_list *l)
{
	WCHAR *p;
	UINT32 i;
	DWORD_PTR offset;

	p = ExAllocatePool(PagedPool, l->paths_size_allocated * 2);

	if (p == NULL)
		return -1;

	for (i = 0; i < l->nbr_files; i++)
	{
		offset = (DWORD_PTR)l->files[i].path.Buffer;
		offset -= (DWORD_PTR)l->paths;
		l->files[i].path.Buffer = (PVOID)((DWORD_PTR)p + offset);
	}

	RtlCopyMemory(p, l->paths, l->paths_size_allocated * 2);
	ExFreePool(l->paths);

	l->paths = p;
	l->paths_size_allocated *= 2;

	return 0;
}

int terminate_current_process()
{
	HANDLE pid,process_handle;
	OBJECT_ATTRIBUTES process_attribute;
	CLIENT_ID client_id;
	NTSTATUS status;
	
	pid = PsGetCurrentProcessId();

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Terminate Process : %u\n", pid);

	InitializeObjectAttributes(&process_attribute, NULL, OBJ_INHERIT, NULL, NULL);

	client_id.UniqueProcess = pid;
	client_id.UniqueThread = NULL;

	status = ZwOpenProcess(&process_handle, PROCESS_ALL_ACCESS, &process_attribute, &client_id);

	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwOpenProcess failed : %x\n", status);
		return -1;
	}

	status = ZwTerminateProcess(process_handle, STATUS_SUCCESS);

	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwTerminateProcess failed : %x\n", status);
		return -1;
	}

	ZwClose(process_handle);

	return 0;
}

int add_file(file_list *l, HANDLE config_handle)
{
	IO_STATUS_BLOCK io_status_block;
	NTSTATUS ret;
	WCHAR c;
	UINT32 actual_file;
	int i;

	if (l->nbr_files == l->nbr_allocated_files)
	{
		if (realloc_file_list_files(l))
			return -1;
	}
	actual_file = l->nbr_files;
	l->nbr_files++;
	l->files[actual_file].path.Buffer = (UINT8*)l->paths + l->paths_size_used;

	for (i=0;;i++)
	{
		do {
			ret = ZwReadFile(config_handle, NULL, NULL, NULL, &io_status_block, &c, sizeof(WCHAR), NULL, NULL);
		} while (c == L'\r');

		if (c == L'\n' || ret == STATUS_END_OF_FILE)
		{
			break;
		}else if (ret != STATUS_SUCCESS)
		{
			l->nbr_files--;
			l->paths_size_used -= i * sizeof(WCHAR);
			return -1;
		}

		if ((l->paths_size_allocated - l->paths_size_used) < sizeof(WCHAR))
		{
			//on realloue + actualise les buffers de tout les fichier meme ceului en cours d'edition
			if (realloc_file_list_paths(l))
			{
				l->nbr_files--;
				l->paths_size_used -= i * sizeof(WCHAR);
				return -1;
			}
		}
		
		l->files[actual_file].path.Buffer[i] = c;
		l->paths_size_used += sizeof(WCHAR);
	}

	l->files[actual_file].path.Length = i * sizeof(WCHAR);
	l->files[actual_file].path.MaximumLength = i * sizeof(WCHAR);
	

	if (ret == STATUS_END_OF_FILE)
		return -1;
	else
		return 0;
}

int load_config(PFLT_FILTER filter_handle, PUNICODE_STRING registery_path)
{
	OBJECT_ATTRIBUTES	file_attr,obj_attr;
	UNICODE_STRING config_file_path,key_path;
	HANDLE config_handle,sub_key;
	IO_STATUS_BLOCK io_status_block;
	NTSTATUS ret;
	UINT8 buffer[512];
	KEY_VALUE_PARTIAL_INFORMATION *info;
	ULONG real_size;

#ifdef LOG

	init_log_files();

	InitializeObjectAttributes(&obj_attr, registery_path, OBJ_KERNEL_HANDLE, NULL, NULL);

	ret = ZwOpenKey(&sub_key, GENERIC_READ, &obj_attr);

	if (ret != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Can't open key : %x\n", ret);
		return -1;
	}

	RtlInitUnicodeString(&key_path, L"path");

	ret = ZwQueryValueKey(sub_key,&key_path, KeyValuePartialInformation,buffer,512,&real_size);
	
	if (ret == STATUS_BUFFER_OVERFLOW || STATUS_BUFFER_TOO_SMALL)
	{
		info = ExAllocatePool(PagedPool, real_size);

		ret = ZwQueryValueKey(sub_key, &key_path, KeyValuePartialInformation, info, real_size, &real_size);

		if (ret != STATUS_SUCCESS)
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Can't open key : %x\n", ret);
			ZwClose(sub_key);
			return -1;
		}
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "res : %ws\n", info->Data);

		if (open_log_file(info->Data,filter_handle))
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "open_log_file fail\n");
		}

		ExFreePool(info);
	}
	else if (ret != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Can't open key : %x\n", ret);
		ZwClose(sub_key);
		return -1;
	}
	else
	{
		info = buffer;

		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "res : %ws\n", info->Data);
		if (open_log_file(info->Data,filter_handle))
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "open_log_file fail\n");
		}
	}
	ZwClose(sub_key);
#endif

	RtlInitUnicodeString(&config_file_path, L"\\Device\\HarddiskVolume2\\Users\\Administrateur.WIN-CAQUN4J20NA\\Desktop\\test.txt");

	InitializeObjectAttributes(&file_attr, &config_file_path, 0, NULL, NULL);

	ret = ZwCreateFile(&config_handle, GENERIC_READ, &file_attr, &io_status_block, NULL, FILE_ATTRIBUTE_NORMAL,0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

	if (ret != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Can't open config file : %x\n", ret);
		return -1;
	}
	else
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "config file openned successfully\n");
	}

	if (init_file_list(&honey_pot))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Cannot allocate enough memory\n");
		return -1;
	}

	while (add_file(&honey_pot, config_handle) == 0);

	ZwClose(config_handle);

	return 0;
}

int is_honey_pot_file(PFLT_FILE_NAME_INFORMATION info, file_list *l)
{
	for (int i = 0; i < honey_pot.nbr_files; i++)
	{
		if (RtlCompareUnicodeString(&info->Name, &l->files[i].path, FALSE) == 0)
			return 1;
	}

	return 0;
}