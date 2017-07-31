#pragma once
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <ntddk.h>
#include <stdlib.h>
#include <Ntstrsafe.h>
#include <Wdm.h>
#include "actions.h"
#include "log.h"

#define DEFAULT_NBR_TRACES_ALLOCATED 30
#define COEFFICIENT 5
#define PRECISION 10000000 //s

//note : for read and write trace maybe count the number of octet
typedef struct process_trace
{
	HANDLE pid;
#ifdef LOG
	HANDLE is_unlogged;
#endif
	actions * actions_set;
}process_trace;

typedef struct trace_list
{
	process_trace *traces;
	UINT32 nbr_traces;
	UINT32 nb_traces_allocated;
}trace_list;

trace_list process_trace_list;

int init_process_trace(process_trace *trace, HANDLE pid,actions *a);
int init_trace_list(trace_list *t);
int add_current_process_trace_extension(trace_list *t, int extension_pos, ULONG hash, PFLT_INSTANCE instance);
process_trace *get_trace_from_pid(trace_list *t, HANDLE pid);
void free_trace(trace_list *l, process_trace*);
void free_trace_list(trace_list *t);
int is_ransomware(trace_list *t, HANDLE pid);
int add_trace_current_process(trace_list *t, action_types a, ULONG hash, PFLT_INSTANCE instance);
process_trace *new_process_trace(trace_list *t, HANDLE pid, HANDLE ppid);