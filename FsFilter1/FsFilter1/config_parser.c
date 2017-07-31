#include "config_parser.h"

int get_wchar(cursor *c)
{
	int a;
	IO_STATUS_BLOCK io_status_block;

	if(c->next_char == NO_CHAR)
	{
		ret = ZwReadFile(c->config_handle, NULL, NULL, NULL, &io_status_block, &a, sizeof(WCHAR), NULL, NULL);

		if(ret == STATUS_END_OF_FILE)
			a = END_OF_FILE;
		else if(ret != STATUS_SUCCESS)
			a = READ_ERROR;
	}else
	{
		a = c->next_char;
		c->next_char = NO_CHAR;
	}

	return a;
}

int next_wchar(cursor *c)
{
	if(c->next_char == NO_CHAR)
		c->next_char = get_char(c);

	return c->next_char;
}

int open_config_file(cursor *c, PUNICODE_STRING config_file_path)
{
	OBJECT_ATTRIBUTES file_attr;
	IO_STATUS_BLOCK io_status_block;
	NTSTATUS ret;

	c->next_char = NO_CHAR;

	InitializeObjectAttributes(&file_attr, config_file_path, 0, NULL, NULL);

	ret = ZwCreateFile(&c->config_handle, GENERIC_READ, &file_attr, &io_status_block, NULL, FILE_ATTRIBUTE_NORMAL,0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

	if (ret != STATUS_SUCCESS)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Can't open config file : %x\n", ret);

		return -1;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "config file openned successfully\n");

	return 0;
}

int add_file(file_list *l,cursor *c)
{
	UINT32 actual_file;
	int i;
	int a;

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
			a = get_wchar(c);
		} while (a == L'\r');

		if (a == L'\n' || a == END_OF_FILE)
		{
			break;
		}else if (a == READ_ERROR)
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
		
		l->files[actual_file].path.Buffer[i] = a;
		l->paths_size_used += sizeof(WCHAR);
	}

	l->files[actual_file].path.Length = i * sizeof(WCHAR);
	l->files[actual_file].path.MaximumLength = i * sizeof(WCHAR);
	

	if (a == END_OF_FILE)
		return -1;
	else
		return 0;
}
