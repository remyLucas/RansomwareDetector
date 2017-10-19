#include "extension.h"

//must be in lower case
WCHAR *default_extensions[] = { L"doc",
L"docx",
L"docb",
L"docm",
L"dot",
L"dotm",
L"dotx",
L"xls",
L"xlsx",
L"xlsm",
L"xlsb",
L"xlw",
L"xlt",
L"xlm",
L"xlc",
L"xltx",
L"xltm",
L"ppt",
L"pptx",
L"pptm",
L"pot",
L"pps",
L"ppsm",
L"ppsx",
L"ppam",
L"potx",
L"potm",
L"pst",
L"ost",
L"msg",
L"eml",
L"edb",
L"vsd",
L"vsdx",
L"txt",
L"csv",
L"rtf",
L"123",
L"wks",
L"wk1",
L"pdf",
L"dwg",
L"onetoc2",
L"snt",
L"hwp",
L"602",
L"sxi",
L"sti",
L"sldx",
L"sldm",
L"sldm",
L"vdi",
L"vmdk",
L"vmx",
L"gpg",
L"aes",
L"ARC",
L"PAQ",
L"bz2",
L"tbk",
L"bak",
L"tar",
L"tgz",
L"gz",
L"7z",
L"rar",
L"zip",
L"backup",
L"iso",
L"vcd",
L"jpeg",
L"jpg",
L"bmp",
L"png",
L"gif",
L"raw",
L"cgm",
L"tif",
L"tiff",
L"nef",
L"psd",
L"ai",
L"svg",
L"djvu",
L"m4u",
L"m3u",
L"mid",
L"wma",
L"flv",
L"3g2",
L"mkv",
L"3gp",
L"mp4",
L"mov",
L"avi",
L"asf",
L"mpeg",
L"vob",
L"mpg",
L"wmv",
L"fla",
L"swf",
L"wav",
L"mp3",
L"sh",
L"class",
L"jar",
L"java",
L"rb",
L"asp",
L"php",
L"jsp",
L"brd",
L"sch",
L"dch",
L"dip",
L"pl",
L"vb",
L"vbs",
L"ps1",
L"bat",
L"cmd",
L"js",
L"asm",
L"h",
L"pas",
L"cpp",
L"c",
L"cs",
L"suo",
L"sln",
L"ldf",
L"mdf",
L"ibd",
L"myi",
L"myd",
L"frm",
L"odb",
L"dbf",
L"db",
L"mdb",
L"accdb",
L"sql",
L"sqlitedb",
L"sqlite3",
L"asc",
L"lay6",
L"lay",
L"mml",
L"sxm",
L"otg",
L"odg",
L"uop",
L"std",
L"sxd",
L"otp",
L"odp",
L"wb2",
L"slk",
L"dif",
L"stc",
L"sxc",
L"ots",
L"ods",
L"3dm",
L"max",
L"3ds",
L"uot",
L"stw",
L"sxw",
L"ott",
L"odt",
L"pem",
L"p12",
L"csr",
L"crt",
L"key",
L"pfx",
L"der" };

int init_extension_list(extension_list *e)
{
	e->extensions = ExAllocatePool(PagedPool, sizeof(WCHAR)*DEFAULT_SIZE_EXTENSION_ALLOCATED);
	if (e->extensions == NULL)
		return -1;
	e->extension_size_allocated = DEFAULT_SIZE_EXTENSION_ALLOCATED;
	e->extension_size_used = 0;

	e->list = ExAllocatePool(PagedPool, sizeof(WCHAR*)*DEFAULT_NBR_LISTED_EXTENSION_ALLOCATED);
	if (e->list == NULL)
	{
		ExFreePool(e->extensions);
		return -1;
	}
	e->list_size = 0;
	e->list_size_allocated = DEFAULT_NBR_LISTED_EXTENSION_ALLOCATED;

	e->nbr_extension_set_allocated = 0;
	e->nbr_extension_set_used = 0;

	e->max_len = 0;
	return 0;
}

int realloc_extension_list_list(extension_list *e)
{
	WCHAR *c;

	c = ExAllocatePool(PagedPool, sizeof(WCHAR*)*e->list_size_allocated * 2);

	if (c == NULL)
		return -1;

	RtlCopyMemory(c, e->list, sizeof(WCHAR*)*e->list_size_allocated);
	ExFreePool(e->list);

	e->list = c;
	e->list_size_allocated *= 2;

	return 0;
}

int realloc_extension_list_extensions(extension_list *e)
{
	WCHAR *c;
	DWORD_PTR offset;
	int i;

	c = ExAllocatePool(PagedPool,sizeof(WCHAR)*e->extension_size_allocated*2);

	if(c == NULL)
		return -1;

	RtlCopyMemory(c, e->extensions, sizeof(WCHAR)*e->extension_size_allocated);

	for (i = 0; i < e->list_size; i++)
	{
		offset = (DWORD_PTR)e->list[i];
		offset -= (DWORD_PTR)e->extensions;
		e->list[i] = (PVOID)((DWORD_PTR)c + offset);
	}

	ExFreePool(e->extensions);

	e->extensions = c;
	e->extension_size_allocated *= 2;
	
	return 0;
}

int add_extension(extension_list *e, WCHAR *p)
{
	int i,j;
	WCHAR *p2;
	IO_STATUS_BLOCK io_status_block;

	if(e->list_size_allocated == e->list_size)
	{
		if(realloc_extension_list_list(e))
			return -1;
	}

	for(i=0;i<e->list_size;i++)
	{
		for(j=0;p[j] == e->list[i][j] && p[j] != L'\0';j++)
		{}

		if(p[j] < e->list[i][j])
			break;
	}

	e->list_size++;
	for(;i<e->list_size;i++)
	{
		p2 = e->list[i];
		e->list[i] = p;
		p = p2;
	}

	return 0;
}

int load_default_extensions(extension_list *e)
{
	int i,j,len;
	WCHAR *p;
	UNICODE_STRING s;

	if(init_extension_list(e))
		return -1;

	for(i=0;i<(sizeof(default_extensions)/sizeof(WCHAR*));i++)
	{
		for(j=0;default_extensions[i][j] != L'\0';j++,e->extension_size_used++)
		{
			if(e->extension_size_used == e->extension_size_allocated)
			{
				if(realloc_extension_list_extensions(e))
					return -1;
			}
			e->extensions[e->extension_size_used] = default_extensions[i][j];
		}

		if(e->extension_size_used == e->extension_size_allocated)
		{
			if(realloc_extension_list_extensions(e))
				return -1;
		}
		e->extensions[e->extension_size_used] = L'\0';
		e->extension_size_used++;

		if (j > e->max_len)
			e->max_len = j;
		
		p = &(e->extensions[e->extension_size_used-(j+1)]);

		if(add_extension(e,p))
			return -1;
	}

	for (i = 0; i < e->list_size; i++)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Extension : %ws\n", e->list[i]);
	}
	return 0;
}

WCHAR* filter_extensions_rec(WCHAR **list,int len, PUNICODE_STRING s)
{
	int pos = len / 2,i;

	if (len < 1)
		return NULL;

	for (i = 0;i < (s->Length/sizeof(WCHAR)) && list[pos][i] != L'\0' && list[pos][i] == s->Buffer[i]; i++)
	{}

	if (i >= (s->Length / sizeof(WCHAR)))
	{
		if (list[pos][i] == L'\0')
		{
			return ((DWORD_PTR)list) + pos*sizeof(WCHAR*);
		}
		else
			i--;
	}

	if (list[pos][i] > s->Buffer[i])
	{
		return filter_extensions_rec(list, len / 2, s);
	}
	else
	{
		return filter_extensions_rec(&list[(len / 2)+1], len - ((len / 2) + 1), s);
	}
}

int filter_extensions(extension_list *e, PUNICODE_STRING s)
{
	WCHAR *p;
	UINT8 buf[64];
	UNICODE_STRING lower_case;

	lower_case.Buffer = buf;
	lower_case.Length = 0;
	lower_case.MaximumLength = 64;

	RtlDowncaseUnicodeString(&lower_case, s, FALSE);
	p = filter_extensions_rec(e->list, e->list_size, &lower_case);

	if (p == NULL)
		return -1;

	return (((DWORD_PTR)p) - ((DWORD_PTR)e->list))/sizeof(WCHAR*);
}

int get_extension(UNICODE_STRING s)
{
	NTSTATUS status;
	int i, pos;


	for (i = ((s.Length) / sizeof(WCHAR)) - 1; i >= 0 && s.Buffer[i] != L'.'; i--)
	{}

	if (i < 0)
		s.Length = 0;
	else
	{
		s.Buffer = &s.Buffer[i + 1];
		s.Length = s.Length - ((i + 1) * sizeof(WCHAR));
	}

	if (s.Length > 0 && s.Length <= ext.max_len)
	{
		pos = filter_extensions(&ext, &s);
		return pos;
	}

	return -1;
}

void init_extension_set(extension_set *s)
{
	for (int i = 0; i < ext.list_size; i++)
	{
		s->tab[i] = 0;
	}
}

int alloc_extension_set()
{
	ext.nbr_extension_set_allocated = DEFAULT_NBR_EXTENSION_SET_ALLOCATED;

	ext.set_list = ExAllocatePool(PagedPool, sizeof(UINT8)*ext.nbr_extension_set_allocated * ext.list_size);

	if (ext.set_list == NULL)
		return -1;

	return 0;
}

int realloc_extension_set()
{
	void *v;

	v = ExAllocatePool(PagedPool, sizeof(UINT8)*ext.nbr_extension_set_allocated * ext.list_size*2);

	if (v == NULL)
		return -1;

	RtlCopyMemory(v, ext.set_list, sizeof(UINT8)*ext.nbr_extension_set_allocated * ext.list_size);
	move_extension_set_pointers(ext.set_list, v);
	ExFreePool(ext.set_list);

	ext.set_list = v;
	ext.nbr_extension_set_allocated *= 2;

	return 0;
}

extension_set* new_extension_set()
{
	extension_set *res;

	if (ext.nbr_extension_set_allocated == 0)
	{
		if (alloc_extension_set() == -1)
			return NULL;
	}
	else if (ext.nbr_extension_set_used == ext.nbr_extension_set_allocated)
	{
		if (realloc_extension_set() == -1)
			return NULL;
	}

	(DWORD_PTR)res = (DWORD_PTR)ext.set_list + (sizeof(UINT8)*ext.nbr_extension_set_used*ext.list_size);
	init_extension_set(res);
	ext.nbr_extension_set_used++;

	return res;
}

void free_extension_set(extension_set *s)
{
	extension_set *last;

	(DWORD_PTR)last = (DWORD_PTR)ext.set_list + (sizeof(UINT8)*(ext.nbr_extension_set_used-1)*ext.list_size);

	*s = *last;

	ext.nbr_extension_set_used--;
}

int is_not_already_used(extension_set*s, int pos)
{
	if (s->tab[pos] == 1)
		return 0;

	s->tab[pos] = 1;
	return 1;
}