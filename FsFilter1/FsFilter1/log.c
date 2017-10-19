#include "log.h"

void init_log_file(log_file *lf)
{
	lf->file_handle = NULL;
	lf->file_obj = NULL;
}

void init_log_files()
{
	actions_log_file.file_handle = NULL;
	actions_log_file.file_obj = NULL;

	pid_log_file.file_handle = NULL;
	pid_log_file.file_obj = NULL;

	filename_log_file.file_handle = NULL;
	filename_log_file.file_obj = NULL;
}

int new_log_file(log_file *lf,WCHAR *filename,HANDLE parent_directory, PFLT_FILTER filter_handle)
{
	NTSTATUS res;
	UNICODE_STRING path;
	OBJECT_ATTRIBUTES obj_attr;
	IO_STATUS_BLOCK   io_status_block;

	RtlInitUnicodeString(&path, filename);
	InitializeObjectAttributes(&obj_attr, &path, OBJ_KERNEL_HANDLE, parent_directory, NULL);

	res = FltCreateFile(
		filter_handle,
		NULL,
		&lf->file_handle,
		FILE_APPEND_DATA | SYNCHRONIZE,
		&obj_attr,
		&io_status_block,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN_IF,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0,
		IO_FORCE_ACCESS_CHECK
	);

	if (res == STATUS_SUCCESS)
	{
		res = ObReferenceObjectByHandle(
			lf->file_handle,
			GENERIC_WRITE,
			*IoFileObjectType,
			KernelMode,
			&lf->file_obj,
			NULL
		);

		if (res != STATUS_SUCCESS)
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ObReferenceObjectByHandle() failed : %x\n", res);
			return -1;
		}
	}
	else
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FltCreateFile() failed : %x\n", res);
		return -1;
	}

	return 0;
}

void free_log_file(log_file *lf)
{
	if (lf->file_obj != NULL)
	{
		ObDereferenceObject(lf->file_obj);
		FltClose(lf->file_handle);
	}

	init_log_file(lf);
}

int write_log_file(log_file *lf, PVOID buffer, ULONG length, PFLT_INSTANCE instance)
{
	NTSTATUS r;
	UNICODE_STRING path;
	OBJECT_ATTRIBUTES obj_attr;
	IO_STATUS_BLOCK io_status_block;
	ULONG total_byte_written,byte_written;
	LARGE_INTEGER writeOffset;

	if (lf->file_obj == NULL)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Cannot write : log file not openned\n");
		return -1;
	}

	total_byte_written = 0;

	writeOffset.HighPart = -1;
	writeOffset.LowPart = FILE_WRITE_TO_END_OF_FILE;

	do {

		r = FltWriteFile(
			log_instance,
			lf->file_obj,
			&writeOffset,
			length-total_byte_written,
			(&((char*)buffer)[total_byte_written]),
			(ULONG) 0,
			&byte_written,
			NULL,
			NULL
		);

		if (r != STATUS_SUCCESS)
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FltWriteFile() failed : %x %x %x\n", r, log_instance, lf->file_obj);
			return -1;
		}
		total_byte_written += byte_written;
		if (length != total_byte_written)
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "WOOOOOOOW %x %x\n", length,total_byte_written);
		}
	} while (length > total_byte_written);

	return 0;
}

int open_log_file(WCHAR *base_dir,PFLT_FILTER filter_handle)
{
	UNICODE_STRING log_file_path,parent_dir;
	OBJECT_ATTRIBUTES file_attr;
	IO_STATUS_BLOCK io_status_block;
	NTSTATUS ret;
	HANDLE directory_handle;

	RtlInitUnicodeString(&parent_dir, base_dir);

	InitializeObjectAttributes(&file_attr, &parent_dir, OBJ_KERNEL_HANDLE, NULL, NULL);

	ret = ZwCreateFile(&directory_handle, FILE_TRAVERSE, &file_attr, &io_status_block, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_DIRECTORY_FILE, NULL, 0);

	if (ret != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwCreateFile(LOG_DIR) failed : %x\n", ret);
		return -1;
	}

	WCHAR buffer[512];
	ULARGE_INTEGER time;

	KeQuerySystemTimePrecise(&time);

	RtlStringCbPrintfW(buffer, 512*sizeof(WCHAR), L"log_%llu.txt",time.QuadPart);

	if (new_log_file(&actions_log_file, buffer, directory_handle, filter_handle))
	{
		ZwClose(directory_handle);
		return -1;
	}

	RtlStringCbPrintfW(buffer, 512 * sizeof(WCHAR), L"pid_%llu.txt", time.QuadPart);

	if (new_log_file(&pid_log_file, buffer, directory_handle, filter_handle))
	{
		ZwClose(directory_handle);
		return -1;
	}

	RtlStringCbPrintfW(buffer, 512 * sizeof(WCHAR), L"file_%llu.txt", time.QuadPart);

	if (new_log_file(&filename_log_file, buffer, directory_handle, filter_handle))
	{
		ZwClose(directory_handle);
		return -1;
	}

	ZwClose(directory_handle);

	return 0;
}

void close_log_files()
{
	free_log_file(&actions_log_file);
	free_log_file(&pid_log_file);
	free_log_file(&filename_log_file);
}
int log_img_name_of_current_process(PFLT_INSTANCE instance)
{
	char buffer[512];
	UINT8 buf[512];
	PUNICODE_STRING name;
	size_t size;
	HANDLE pid;

	pid = PsGetCurrentProcessId();
	name = buf;

	if (get_proc_img_by_pid(pid, name, 512))
	{
		RtlStringCbPrintfA(buffer, 512, "%u,\"Unknown\"\r\n", pid);
	}
	else
	{
		RtlStringCbPrintfA(buffer, 512, "%u,\"%wZ\"\r\n", pid, name);
	}

	RtlStringCbLengthA(buffer, 512, &size);

	if (write_log_file(&pid_log_file, buffer, size, instance))
		return -1;

	return 0;
}

int log_pid(HANDLE pid, HANDLE ppid, PFLT_INSTANCE instance)
{
	char buffer[512];
	size_t size;

	RtlStringCbPrintfA(buffer, 512, "%u,%u\r\n", pid, ppid);

	RtlStringCbLengthA(buffer, 512, &size);

	if (write_log_file(&pid_log_file, buffer, size, instance))
		return -1;

	return 0;
}

char *actions_types_str[] = {"READ",
"WRITE",
"OPEN",
"CREATE",
"DELETE",
"CLOSE",
"QUERY_DIR",
"RENAME",
"EXTENSION",
"RENAME_EXT"};

int log_actions(action_types a, ULONG hash, PFLT_INSTANCE instance)
{
	char buffer[512];
	size_t size;
	ULARGE_INTEGER time;
	HANDLE pid;

	KeQuerySystemTimePrecise(&time);
	pid = PsGetCurrentProcessId();

	RtlStringCbPrintfA(buffer, 512, "%u,%lu,%s,%llu\r\n", pid,
		hash,
		actions_types_str[a],
		time.QuadPart
	);

	RtlStringCbLengthA(buffer, 512, &size);

	if (write_log_file(&actions_log_file, buffer, size, instance))
		return -1;

	return 0;
}

int log_new_file(ULONG hash, UNICODE_STRING name, PFLT_INSTANCE instance)
{
	char buffer[512];
	size_t size;

	RtlStringCbPrintfA(buffer, 512, "%lu,%wZ\r\n", hash, &name);

	RtlStringCbLengthA(buffer, 512, &size);

	if (write_log_file(&filename_log_file, buffer, size, instance))
		return -1;
		
	return 0;
}