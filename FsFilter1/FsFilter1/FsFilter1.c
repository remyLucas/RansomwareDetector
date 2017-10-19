#include "FsFilter1.h"

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

const FLT_OPERATION_REGISTRATION Callbacks[] = {
	{ IRP_MJ_CREATE,
	0,
	common_pre_operation,
	create_post_operation },

	{ IRP_MJ_READ,
	0,
	common_pre_operation,
	read_post_operation },

	{ IRP_MJ_WRITE,
	0,
	common_pre_operation,
	write_post_operation },
	{ IRP_MJ_CLOSE,
	0,
	common_pre_operation,
	cleanup_post_operation },
	{ IRP_MJ_CLEANUP,
	0,
	common_pre_operation,
	cleanup_post_operation },
	{ IRP_MJ_DIRECTORY_CONTROL,
	0,
	directory_control_pre_operation,
	NULL},
	{ IRP_MJ_SET_INFORMATION,
	0,
	set_information_pre_operation,
	set_information_post_operation },
	
#if 0 // TODO - List all of the requests to filter.
	{ IRP_MJ_FILE_SYSTEM_CONTROL,
	0,
	FsFilter1PreOperation,
	FsFilter1PostOperation },

    { IRP_MJ_CREATE_NAMED_PIPE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_INFORMATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_INFORMATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_EA,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_EA,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_FLUSH_BUFFERS,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_VOLUME_INFORMATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_VOLUME_INFORMATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_FILE_SYSTEM_CONTROL,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_DEVICE_CONTROL,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_INTERNAL_DEVICE_CONTROL,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SHUTDOWN,
      0,
      FsFilter1PreOperationNoPostOperation,
      NULL },                               //post operations not supported

    { IRP_MJ_LOCK_CONTROL,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_CLEANUP,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_CREATE_MAILSLOT,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_SECURITY,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_SECURITY,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_QUOTA,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_QUOTA,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_PNP,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_RELEASE_FOR_MOD_WRITE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_RELEASE_FOR_CC_FLUSH,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_NETWORK_QUERY_OPEN,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_MDL_READ,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_MDL_READ_COMPLETE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_PREPARE_MDL_WRITE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_MDL_WRITE_COMPLETE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_VOLUME_MOUNT,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_VOLUME_DISMOUNT,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

#endif // TODO

    { IRP_MJ_OPERATION_END }
};

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof( FLT_REGISTRATION ),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    NULL,                               //  Context
    Callbacks,                          //  Operation callbacks

    FsFilter1Unload,                           //  MiniFilterUnload

    FsFilter1InstanceSetup,                    //  InstanceSetup
    FsFilter1InstanceQueryTeardown,            //  InstanceQueryTeardown
    NULL,            //  InstanceTeardownStart
    NULL,         //  InstanceTeardownComplete
    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent

};

NTSTATUS
FsFilter1InstanceSetup (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeDeviceType );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    PAGED_CODE();

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Setup\n");

    return STATUS_SUCCESS;
}


NTSTATUS FsFilter1InstanceQueryTeardown (PCFLT_RELATED_OBJECTS FltObjects,FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FsFilter1!FsFilter1InstanceQueryTeardown: Entered\n");
	//on ne se détache jamais
    return STATUS_FLT_DO_NOT_DETACH;
}

void process_notify_callback(HANDLE  parent_pid, HANDLE  pid, BOOLEAN create)
{
	process_trace *p;
	PUNICODE_STRING img,img2;
	UINT8 buf[512],buf2[512];
	
	if (create == TRUE)
	{
		p = get_trace_from_pid(&process_trace_list, parent_pid);

		if (p != NULL)
		{
			img = buf;
			img2 = buf2;

			if (get_proc_img_by_pid(pid, img, 512) == 0)
			{
				if (get_proc_img_by_pid(parent_pid, img2, 512) == 0)
				{
					if (RtlCompareUnicodeString(img, img2, FALSE) == 0)
					{
						new_process_trace(&process_trace_list, pid, parent_pid);
						return 0;
					}
				}
			}
		}
	}
	else
	{
		free_trace_from_pid(&process_trace_list, pid);
	}
}

/*************************************************************************
    MiniFilter initialization and unload routines.
*************************************************************************/

//utile pour pouvoir décharger le driver
VOID DriverUnload(PDRIVER_OBJECT  DriverObject)
{
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FsFilter1!DriverUnload: Entered\n");
	free_trace_list(&process_trace_list);
	PsSetCreateProcessNotifyRoutine(process_notify_callback, TRUE);
}



NTSTATUS DriverEntry (_In_ PDRIVER_OBJECT DriverObject,_In_ PUNICODE_STRING RegistryPath)
{
    NTSTATUS status;
	UNICODE_STRING alt;
	PFLT_VOLUME VolumeList[10];
	ULONG NumberVolumesReturned;
	FLT_VOLUME_PROPERTIES volprop;
	ULONG lenght_needed;
	PFLT_FILE_NAME_INFORMATION info;
	UINT8 buffer[512];
	UNICODE_STRING volume_name,instance_name;
	PFLT_INSTANCE ret_instance;

    UNREFERENCED_PARAMETER( RegistryPath );

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FsFilter1!DriverEntry: Entered\n");

	DriverObject->DriverUnload = DriverUnload;

	status = FltRegisterFilter(DriverObject,
		&FilterRegistration,
		&gFilterHandle);

	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FltRegisterFilter() failed : %x\n", status);
		//return STATUS_FAILED_DRIVER_ENTRY;
	}

	if (start_actions())
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "start_actions fail\n");
		//return STATUS_FAILED_DRIVER_ENTRY;
	}

	if (load_default_extensions(&ext))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "load_default_extensions fail\n");
		//return STATUS_FAILED_DRIVER_ENTRY;
	}

	if (init_trace_list(&process_trace_list))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "init_trace_list fail\n");
		//return STATUS_FAILED_DRIVER_ENTRY;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "PsSetCreateProcessNotifyRoutine : %x\n",PsSetCreateProcessNotifyRoutine(process_notify_callback, FALSE));

	if (load_config(gFilterHandle, RegistryPath))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "load_config fail\n");
		//return STATUS_FAILED_DRIVER_ENTRY;
	}

#ifdef LOG 
	if (actions_log_file.file_obj != NULL)
	{
		status = FltGetFileNameInformationUnsafe(actions_log_file.file_obj, NULL, FLT_FILE_NAME_NORMALIZED, &info);

		if (status != STATUS_SUCCESS)
		{
			info = NULL;
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FltGetFileNameInformationUnsafe() failed : %x\n", status);
		}

		status = FltParseFileNameInformation(info);

		if (status != STATUS_SUCCESS)
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FltParseFileNameInformation() failed : %x\n", status);
		}
	}
	else
	{
		info = NULL;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Cannot get info!!!!!!!!!!!!!!!!!!!!!!!!\n", status);
	}
#endif
	
		status = FltStartFiltering(gFilterHandle);

		if (status != STATUS_SUCCESS)
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FltStartFiltering() failed : %x\n", status);
			FltUnregisterFilter(gFilterHandle);
			status = STATUS_FAILED_DRIVER_ENTRY;
		}
		else
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Filter started successfully!\n");
			status = FltEnumerateVolumes(gFilterHandle, VolumeList, 10, &NumberVolumesReturned);

			if (status != STATUS_SUCCESS)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FltEnumerateVolumes failed : %x\n", status);
				status = STATUS_FAILED_DRIVER_ENTRY;
			}
			else
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FltEnumerateVolumes find %d volumes\n", NumberVolumesReturned);

				RtlInitUnicodeString(&alt, L"03333");

				volume_name.Buffer = buffer;
				volume_name.Length = 0;
				volume_name.MaximumLength = 512;
				RtlInitUnicodeString(&instance_name, L"FsFilter1 Instance");

				log_instance = NULL;

				for (int i = 0; i < NumberVolumesReturned; i++)
				{
					FltGetVolumeProperties(VolumeList[i], &volprop, sizeof(FLT_VOLUME_PROPERTIES), &lenght_needed);

					FltGetVolumeName(VolumeList[i], &volume_name, NULL);

					switch(volprop.DeviceType)
					{
						case FILE_DEVICE_DFS_FILE_SYSTEM:
						case FILE_DEVICE_DISK_FILE_SYSTEM:
						case FILE_DEVICE_FILE_SYSTEM:
						case FILE_DEVICE_NETWORK_FILE_SYSTEM:
#ifdef LOG
							status = FltAttachVolumeAtAltitude(gFilterHandle, VolumeList[i], &alt, NULL, &ret_instance);
							//status = FltAttachVolume(gFilterHandle, VolumeList[i], NULL, &ret_instance);
#else
							//status = FltAttachVolumeAtAltitude(gFilterHandle, VolumeList[i], &alt, NULL, NULL);
							status = FltAttachVolume(gFilterHandle, VolumeList[i], NULL, NULL);
#endif
							if (status != STATUS_SUCCESS)
							{
								DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FltAttachVolume failed : %x\n", status);
							}
#ifdef LOG
							else if (info != NULL && RtlEqualUnicodeString(&volume_name, &info->Volume, TRUE) == TRUE)
							{
								log_instance = ret_instance;
								DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Log instance OK !!!!!!!!!!!!!!!!!!!!!\n");
							}
							else
							{
								FltObjectDereference(ret_instance);
							}
#endif
							break;
						default:
							break;
					}
				}
				status = STATUS_SUCCESS;
			}
		}
#ifdef LOG 
		if (info != NULL)
		{
			FltReleaseFileNameInformation(info);
		}
#endif

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Exit Driver Entry\n");
    //return status;
	return STATUS_SUCCESS;
}

//utile pour pouvoir décharger le driver
NTSTATUS FsFilter1Unload (_In_ FLT_FILTER_UNLOAD_FLAGS Flags)
{
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "FsFilter1!FsFilter1Unload: Entered\n");

#ifdef LOG
	close_log_files();
	if(log_instance != NULL)
		FltObjectDereference(log_instance);
#endif

    FltUnregisterFilter( gFilterHandle );

    return STATUS_SUCCESS;
}

/*************************************************************************
    MiniFilter callback routines.
*************************************************************************/

FLT_PREOP_CALLBACK_STATUS FsFilter1PreOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID *CompletionContext)
{
	NTSTATUS status;
	PFLT_FILE_NAME_INFORMATION info;
	HANDLE pid;
	process_trace *p;
	UNICODE_STRING s;
	int i,pos;

	UNREFERENCED_PARAMETER( FltObjects );
	UNREFERENCED_PARAMETER(CompletionContext);

	
			/*if (Data->Iopb->IrpFlags & IRP_CREATE_OPERATION)
			{
				FltParseFileNameInformation(info);//ne pas oublier de free ca !!!!!!
				if (is_honey_pot_file(info, &honey_pot))
				{
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "A thread is trying to acces honey pot file : %wZ\n", &info->Name);
					Data->IoStatus.Status = STATUS_FILE_NOT_AVAILABLE;
					return FLT_PREOP_COMPLETE;
				}
			}
			if (Data->Iopb->IrpFlags & IRP_WRITE_OPERATION)
			{
				FltParseFileNameInformation(info);
				if (is_honey_pot_file(info,&honey_pot))
				{
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "A thread is trying to acces honey pot file : %wZ\n", &info->Name);
					terminate_current_process();
					Data->IoStatus.Status = STATUS_FILE_LOCKED_WITH_ONLY_READERS;
					return FLT_PREOP_COMPLETE;
				}
			}*/


	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS common_pre_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext)
{
	FLT_POSTOP_CALLBACK_STATUS  status;
	ULONG res;

	*CompletionContext = &res;

	if (FltDoCompletionProcessingWhenSafe(Data, FltObjects, *CompletionContext, 0, common_pre_operation_safe, &status) == TRUE)
	{
		*CompletionContext = res;
		return status;
	}
	else
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
}

FLT_POSTOP_CALLBACK_STATUS common_pre_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	ULONG res;

	res = is_critical_file(Data, FltObjects);
	*((ULONG*)CompletionContext) = res;
	if (res)
	{
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS read_post_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	FLT_POSTOP_CALLBACK_STATUS  status;
	
	if (FltDoCompletionProcessingWhenSafe(Data, FltObjects, CompletionContext, Flags, read_post_operation_safe, &status) == TRUE)
	{
		return status;
	}
	else
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}

}

FLT_POSTOP_CALLBACK_STATUS read_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	if (Data->IoStatus.Status == STATUS_SUCCESS)
	{
		add_trace_current_process(&process_trace_list, ACTION_READ, CompletionContext,FltObjects->Instance);
#ifdef LOG
		log_actions(ACTION_READ, CompletionContext,FltObjects->Instance);
#endif
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_POSTOP_CALLBACK_STATUS write_post_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	FLT_POSTOP_CALLBACK_STATUS  status;

	if (FltDoCompletionProcessingWhenSafe(Data, FltObjects, CompletionContext, Flags, write_post_operation_safe, &status) == TRUE)
	{
		return status;
	}
	else
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
}

FLT_POSTOP_CALLBACK_STATUS write_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	if (Data->IoStatus.Status == STATUS_SUCCESS)
	{
		add_trace_current_process(&process_trace_list, ACTION_WRITE, CompletionContext, FltObjects->Instance);
#ifdef LOG
		log_actions(ACTION_WRITE, CompletionContext, FltObjects->Instance);
#endif
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_POSTOP_CALLBACK_STATUS create_post_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	FLT_POSTOP_CALLBACK_STATUS  status;

	if (FltDoCompletionProcessingWhenSafe(Data, FltObjects, CompletionContext, Flags, create_post_operation_safe, &status) == TRUE)
	{
		return status;
	}
	else
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
}

FLT_POSTOP_CALLBACK_STATUS create_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	if (Data->IoStatus.Status == STATUS_SUCCESS)
		switch (Data->IoStatus.Information)
		{
		case FILE_CREATED:
			add_trace_current_process(&process_trace_list, ACTION_CREATE, CompletionContext, FltObjects->Instance);
#ifdef LOG
			log_actions(ACTION_CREATE, CompletionContext, FltObjects->Instance);
#endif
			break;
		case FILE_OVERWRITTEN:
			add_trace_current_process(&process_trace_list, ACTION_DELETE, CompletionContext, FltObjects->Instance);
#ifdef LOG
			log_actions(ACTION_DELETE, CompletionContext, FltObjects->Instance);
#endif
		case FILE_OPENED:
			add_trace_current_process(&process_trace_list, ACTION_OPEN, CompletionContext, FltObjects->Instance);
#ifdef LOG
			log_actions(ACTION_OPEN, CompletionContext, FltObjects->Instance);
#endif
			break;
		}

	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS directory_control_pre_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext)
{
	FLT_POSTOP_CALLBACK_STATUS  status;

	if (FltDoCompletionProcessingWhenSafe(Data, FltObjects, NULL, 0, directory_control_pre_operation_safe, &status) == TRUE)
	{
		return status;
	}
	else
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
}

FLT_POSTOP_CALLBACK_STATUS directory_control_pre_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	ULONG hash;
	PFLT_FILE_NAME_INFORMATION info;

	if (Data->Iopb->MinorFunction == IRP_MN_QUERY_DIRECTORY)
	{

		if (is_standard_file(Data, FltObjects))
		{
			if (get_file_info(Data, &info) == 0)
			{
				RtlHashUnicodeString(&info->Name, FALSE, HASH_STRING_ALGORITHM_DEFAULT, &hash);

				FltReleaseFileNameInformation(info);

				switch (Data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass)
				{
				case FileBothDirectoryInformation:
				case FileDirectoryInformation:
				case FileFullDirectoryInformation:
				case FileIdBothDirectoryInformation:
				case FileIdFullDirectoryInformation:
				case FileNamesInformation:
#ifdef LOG
					if (file_exist_for_current_process(&process_trace_list, hash) == 0)
					{
						log_new_file(hash, info->Name,FltObjects->Instance);
					}
					log_actions(ACTION_QUERY_DIR, hash, FltObjects->Instance);
#endif
					add_trace_current_process(&process_trace_list, ACTION_QUERY_DIR, hash, FltObjects->Instance);
					break;
				default:
					break;
				}
			}
		}
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS set_information_pre_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext)
{
	FLT_POSTOP_CALLBACK_STATUS  status;
	set_information_data *data;

	if (FltDoCompletionProcessingWhenSafe(Data, FltObjects, &data, 0, set_information_pre_operation_safe, &status) == TRUE)
	{
		*CompletionContext = data;
		return status;
	}
	else
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
}

FLT_POSTOP_CALLBACK_STATUS set_information_pre_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	set_information_data *data;
	int pos;
	PFLT_FILE_NAME_INFORMATION info;

	data = (set_information_data*)ExAllocatePool(PagedPool, sizeof(set_information_data));

	if (data == NULL)
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	
	*((set_information_data**)CompletionContext) = data;

	if (is_standard_file(Data, FltObjects))
	{
		if (get_file_info(Data, &info) == 0)
		{
			pos = get_extension(info->Name);
			RtlHashUnicodeString(&info->Name, FALSE, HASH_STRING_ALGORITHM_DEFAULT, &data->hash);
			FltReleaseFileNameInformation(info);

			if (pos != -1)
			{
#ifdef LOG
				if(file_exist_for_current_process(&process_trace_list, &data->hash) == 0)
				{
					log_new_file(data->hash, info->Name, FltObjects->Instance);
				}
#endif
				data->extension = pos;
				add_current_process_trace_extension(&process_trace_list, pos, data->hash, FltObjects->Instance);

				return FLT_PREOP_SUCCESS_WITH_CALLBACK;
			}
		}
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS set_information_post_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	FLT_POSTOP_CALLBACK_STATUS  status;

	if (FltDoCompletionProcessingWhenSafe(Data, FltObjects, CompletionContext, Flags, set_information_post_operation_safe, &status) == TRUE)
	{
		return status;
	}
	else
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
}

FLT_POSTOP_CALLBACK_STATUS set_information_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	ULONG hash;
	PFLT_FILE_NAME_INFORMATION info;
	set_information_data *data;
	int extension;

	data = CompletionContext;

	if (Data->IoStatus.Status == STATUS_SUCCESS && Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileRenameInformation)
	{
		if (get_file_info(Data, &info) == 0)
		{
			RtlHashUnicodeString(&info->Name, FALSE, HASH_STRING_ALGORITHM_DEFAULT, &hash);

			extension = get_extension(info->Name);

#ifdef LOG
			if (file_exist_for_current_process(&process_trace_list, &hash) == 0)
			{
				log_new_file(hash, info->Name, FltObjects->Instance);
			}
#endif

			if (extension != data->extension)
			{
				add_trace_current_process(&process_trace_list, ACTION_RENAME_EXT, hash, FltObjects->Instance);
#ifdef LOG
				log_actions(ACTION_RENAME_EXT, hash, FltObjects->Instance);
#endif
			}
			else
			{
				add_trace_current_process(&process_trace_list, ACTION_RENAME, hash, FltObjects->Instance);
#ifdef LOG
				log_actions(ACTION_RENAME, hash, FltObjects->Instance);
#endif
			}

			FltReleaseFileNameInformation(info);
		}
	}

	ExFreePool(data);

	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_POSTOP_CALLBACK_STATUS cleanup_post_operation(PFLT_CALLBACK_DATA Data,PCFLT_RELATED_OBJECTS FltObjects,PVOID CompletionContext,FLT_POST_OPERATION_FLAGS Flags)
{
	FLT_POSTOP_CALLBACK_STATUS status;

	if (FltDoCompletionProcessingWhenSafe(Data, FltObjects, CompletionContext, Flags, cleanup_post_operation_safe, &status) == TRUE)
	{
		return status;
	}
	else
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
}

FLT_POSTOP_CALLBACK_STATUS cleanup_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	NTSTATUS status;
	FILE_STANDARD_INFORMATION fileInfo;
	FLT_POSTOP_CALLBACK_STATUS  postop_status;

	if (NT_SUCCESS(Data->IoStatus.Status))
	{
		status = FltQueryInformationFile(Data->Iopb->TargetInstance,
			Data->Iopb->TargetFileObject,
			&fileInfo,
			sizeof(fileInfo),
			FileStandardInformation,
			NULL);

		if (STATUS_FILE_DELETED == status)
		{
			add_trace_current_process(&process_trace_list, ACTION_DELETE, CompletionContext, FltObjects->Instance);
#ifdef LOG
			log_actions(ACTION_DELETE, CompletionContext, FltObjects->Instance);
#endif
		}

		add_trace_current_process(&process_trace_list, ACTION_CLOSE, CompletionContext, FltObjects->Instance);
#ifdef LOG
		log_actions(ACTION_CLOSE, CompletionContext, FltObjects->Instance);
#endif
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}
