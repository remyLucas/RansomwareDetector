#pragma once

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <ntddk.h>
#include <stdlib.h>
#include "config.h"
#include "trace.h"
#include "extension.h"
#include "common.h"

typedef struct set_information_data
{
	ULONG hash;
	int extension;
}set_information_data;

PFLT_FILTER gFilterHandle;

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
);

NTSTATUS
FsFilter1InstanceSetup(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
);

NTSTATUS
FsFilter1Unload(
	_In_ FLT_FILTER_UNLOAD_FLAGS Flags
);

NTSTATUS
FsFilter1InstanceQueryTeardown(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

FLT_PREOP_CALLBACK_STATUS
FsFilter1PreOperation(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

FLT_PREOP_CALLBACK_STATUS common_pre_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects,PVOID *CompletionContext);
FLT_PREOP_CALLBACK_STATUS directory_control_pre_operation(PFLT_CALLBACK_DATA Data,PCFLT_RELATED_OBJECTS FltObjects,PVOID *CompletionContext);
FLT_PREOP_CALLBACK_STATUS set_information_pre_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext);

FLT_POSTOP_CALLBACK_STATUS set_information_pre_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS common_pre_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS directory_control_pre_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);

FLT_POSTOP_CALLBACK_STATUS cleanup_post_operation(PFLT_CALLBACK_DATA Data,PCFLT_RELATED_OBJECTS FltObjects,PVOID CompletionContext,FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS read_post_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS write_post_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS create_post_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS set_information_post_operation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);

FLT_POSTOP_CALLBACK_STATUS cleanup_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS read_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS write_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS create_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS set_information_post_operation_safe(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, FsFilter1Unload)
#pragma alloc_text(PAGE, FsFilter1InstanceQueryTeardown)
#pragma alloc_text(PAGE, FsFilter1InstanceSetup)

#pragma alloc_text(PAGE, common_pre_operation)
#pragma alloc_text(PAGE, directory_control_pre_operation)
#pragma alloc_text(PAGE, set_information_pre_operation)

#pragma alloc_text(PAGE, common_pre_operation_safe)
#pragma alloc_text(PAGE, directory_control_pre_operation_safe)
#pragma alloc_text(PAGE, set_information_pre_operation_safe)

#pragma alloc_text(PAGE, cleanup_post_operation)
#pragma alloc_text(PAGE, set_information_post_operation)
#pragma alloc_text(PAGE, read_post_operation)
#pragma alloc_text(PAGE, write_post_operation)
#pragma alloc_text(PAGE, create_post_operation)

#pragma alloc_text(PAGE, cleanup_post_operation_safe)
#pragma alloc_text(PAGE, set_information_post_operation_safe)
#pragma alloc_text(PAGE, read_post_operation_safe)
#pragma alloc_text(PAGE, write_post_operation_safe)
#pragma alloc_text(PAGE, create_post_operation_safe)

#endif