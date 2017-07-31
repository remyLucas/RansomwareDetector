#include "actions.h"

void init_action(action *a)
{

}

void init_actions(actions *a, HANDLE ppid)
{
	a->nbr_proc = 1;
	a->ppid = ppid;

	a->cursor = 0;
	a->size = 0;

	a->file_list_size = 0;
}

int realloc_actions_array()
{
	actions *a;

	a = ExAllocatePool(PagedPool, sizeof(actions)*nbr_actions_allocated * 2);

	if (a == NULL)
		return -1;

	RtlCopyMemory(a, actions_array, sizeof(actions)*nbr_actions_allocated);
	move_action_set_pointer(actions_array, a);
	ExFreePool(actions_array);

	actions_array = a;
	nbr_actions_allocated *= 2;

	return 0;
}

actions *new_actions(HANDLE ppid)
{
	actions *res;

	if (nbr_actions_used == nbr_actions_allocated)
	{
		if(realloc_actions_array() == -1)
			return NULL;
	}

	
	res = &actions_array[nbr_actions_used];
	init_actions(res,ppid);
	res->extensions_openned = new_extension_set();

	if (res->extensions_openned == NULL)
		return NULL;
	
	nbr_actions_used++;

	return res;
}

void free_actions(actions *a)
{
	DWORD_PTR pos;

	a->nbr_proc--;

	if (a->nbr_proc > 0)
		return;

	pos = a;
	pos = pos - (DWORD_PTR)actions_array;
	pos = pos / sizeof(actions);

	free_extension_set(&actions_array[pos].extensions_openned);
	actions_array[pos] = actions_array[nbr_actions_used - 1];
	nbr_actions_used--;
}

int start_actions()
{
	actions_array = ExAllocatePool(PagedPool, sizeof(actions)*DEFAULT_NBR_ACTIONS_ALLOCATED);

	if (actions_array == NULL)
		return -1;

	nbr_actions_used = 0;
	nbr_actions_allocated = DEFAULT_NBR_ACTIONS_ALLOCATED;

	return 0;
}

void add_action(actions *a, action_types t, ULONG hash)
{
	if (a->size == ACTIONS_ARRAY_SIZE)
	{
		remove_file(a, a->actions_list[a->cursor].file_name_hash);
	}

	a->actions_list[a->cursor].type = t;
	a->actions_list[a->cursor].file_name_hash = hash;
	KeQuerySystemTimePrecise(&a->actions_list[a->cursor].timestamp);

	a->cursor = (a->cursor + 1) % ACTIONS_ARRAY_SIZE;

	if (a->size < ACTIONS_ARRAY_SIZE)
		a->size++;

	add_file_hash(a, hash);
}

void move_extension_set_pointers(extension_set *old, extension_set *new)
{
	DWORD_PTR pos;

	for (int i = 0; i < nbr_actions_used; i++)
	{
		pos = (DWORD_PTR)actions_array[i].extensions_openned - (DWORD_PTR)old;
		(DWORD_PTR)actions_array[i].extensions_openned = (DWORD_PTR)new + pos;
	}
}

int add_file_hash(actions *a, ULONG hash)
{
	UINT32 min, max, pos;
	file_element e1, e2;

	for (min = 0, max = a->file_list_size, pos = 0; min != max;)
	{
		pos = (max + min) / 2;

		if (a->file_list[pos].hash > hash)
			min = pos + 1;
		else if (a->file_list[pos].hash < hash)
			max = pos;
		else
		{
			a->file_list[pos].nbr++;
			return 0;
		}
	}

	e1.hash = hash;
	e1.nbr = 1;
	a->file_list_size++;
	for (; pos<a->file_list_size; pos++)
	{
		e2 = a->file_list[pos];
		a->file_list[pos] = e1;
		e1 = e2;
	}

	return 1;
}

int remove_file(actions *a,ULONG hash)
{
	UINT32 min, max, pos;
	file_element e;

	for (min = 0, max = a->file_list_size, pos = 0; min != max;)
	{
		pos = (max + min) / 2;

		if (a->file_list[pos].hash > hash)
			min = pos + 1;
		else if (a->file_list[pos].hash < hash)
			max = pos;
		else
		{
			a->file_list[pos].nbr--;
			if (a->file_list[pos].nbr > 0)
				return 0;
			else
				break;
		}
	}

	a->file_list_size--;
	for (; pos<a->file_list_size; pos++)
	{
		e = a->file_list[pos + 1];
		a->file_list[pos] = e;
	}

	return 1;
}

int file_exist(actions *a, ULONG hash)
{
	UINT32 min, max, pos;

	for (min = 0, max = a->file_list_size, pos = 0; min != max;)
	{
		pos = (max + min) / 2;

		if (a->file_list[pos].hash > hash)
			min = pos + 1;
		else if (a->file_list[pos].hash < hash)
			max = pos;
		else
			return 1;
	}

	return 0;
}