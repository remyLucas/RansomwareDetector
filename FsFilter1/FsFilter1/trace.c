#include "trace.h"

void free_trace_from_pid(trace_list *l, HANDLE pid)
{
	process_trace *p;

	p = get_trace_from_pid(l, pid);

	if (p == NULL)
		return;

	free_trace(l,p);
}

void free_trace(trace_list *l, process_trace *p)
{
	DWORD_PTR pos;

	pos = p;
	pos = pos - (DWORD_PTR)l->traces;
	pos = pos / sizeof(process_trace);

	free_actions(&l->traces[pos].actions_set);
	l->nbr_traces--;

	if (pos < l->nbr_traces)
		l->traces[pos] = l->traces[l->nbr_traces];
}

int init_process_trace(process_trace *trace, HANDLE pid, actions *a)
{
	trace->pid = pid;
	trace->actions_set = a;
#ifdef LOG
	trace->is_unlogged = 0;
#endif
}

int init_trace_list(trace_list *t)
{
	t->traces = ExAllocatePool(PagedPool, sizeof(process_trace)*DEFAULT_NBR_TRACES_ALLOCATED);
	if (t->traces == NULL)
		return -1;

	t->nbr_traces = 0;
	t->nb_traces_allocated = DEFAULT_NBR_TRACES_ALLOCATED;

	return 0;
}

void free_trace_list(trace_list *t)
{
	while (t->nbr_traces > 0)
	{
		free_trace(t, t->traces);
	}

	ExFreePool(t->traces);

	t->nbr_traces = 0;
	t->nb_traces_allocated = 0;
}

void move_action_set_pointer(actions *old, actions *new)
{
	DWORD_PTR pos;

	for (int i = 0; i < process_trace_list.nbr_traces; i++)
	{
		pos = ((DWORD_PTR)process_trace_list.traces[i].actions_set - (DWORD_PTR)old) / sizeof(actions);
		process_trace_list.traces[i].actions_set = &new[pos];
	}
}

process_trace *get_trace_from_pid(trace_list *t, HANDLE pid)
{
	for (int i = 0; i < t->nbr_traces; i++)
	{
		if (pid == t->traces[i].pid)
		{
			return &t->traces[i];
		}
	}

	return NULL;
}

int realloc_trace_list(trace_list *t)
{
	process_trace *p;

	p = ExAllocatePool(PagedPool, sizeof(process_trace)*t->nb_traces_allocated * 2);

	if (p == NULL)
		return -1;

	RtlCopyMemory(p, t->traces, sizeof(process_trace)*t->nb_traces_allocated);
	ExFreePool(t->traces);

	t->traces = p;
	t->nb_traces_allocated *= 2;

	return 0;
}

process_trace *new_process_trace(trace_list *t, HANDLE pid, HANDLE ppid)
{
	actions *a;
	process_trace *p;

	if (t->nbr_traces == t->nb_traces_allocated)
	{
		if (realloc_trace_list(t))
		{
			return NULL;
		}
	}

	if (ppid != 0)
	{
		p = get_trace_from_pid(t, ppid);

		if (p == NULL)
		{
			return NULL;
		}

		p->actions_set->nbr_proc++;
		a = p->actions_set;
	}
	else
	{
		a = new_actions(pid);

		if (a == NULL)
		{
			return NULL;
		}
	}

	init_process_trace(&(t->traces[t->nbr_traces]), pid, a);
	t->nbr_traces++;

#ifdef LOG
	if (ppid != 0)
	{
		p->is_unlogged = ppid;
	}
#endif

	return &(t->traces[t->nbr_traces - 1]);
}

int add_current_process_trace_extension(trace_list *t, int extension_pos,ULONG hash, PFLT_INSTANCE instance)
{
	process_trace *p;

	p = get_trace_from_pid(t, PsGetCurrentProcessId());
	if (p == NULL)
	{
		return 0;
	}

	if (is_not_already_used(p->actions_set->extensions_openned,extension_pos))
	{
		add_trace_current_process(t, ACTION_EXTENSION,hash,instance);
#ifdef LOG
		log_actions(ACTION_EXTENSION, hash,instance);
#endif
	}
}

int add_trace_current_process(trace_list *t, action_types a, ULONG hash, PFLT_INSTANCE instance)
{
	process_trace *p;
	int ret;
	HANDLE pid;

	pid = PsGetCurrentProcessId();

	p = get_trace_from_pid(t, pid);

	if (p == NULL)
	{
		p = new_process_trace(t, pid,0);
		if (p == NULL)
		{
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "add trace failed\n");
			return -1;
		}

#ifdef LOG
		log_img_name_of_current_process(instance);
#endif
	}
#ifdef LOG
	if (p->is_unlogged)
	{
		log_pid(pid, p->is_unlogged, instance);
		p->is_unlogged = 0;
	}
#endif
	
	add_action(p->actions_set,a,hash);
	
	return 0;
}

int is_ransomware(trace_list *t, HANDLE pid)
{

	return 0;
}

int file_exist_for_current_process(trace_list *t, ULONG hash)
{
	process_trace *p;

	p = get_trace_from_pid(t, PsGetCurrentProcessId());

	if (p == NULL)
		return 0;

	return file_exist(p->actions_set, hash);
}