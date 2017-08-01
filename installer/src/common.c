#include <common.h>

TCHAR *convert_path(TCHAR *dos_path)
{
	TCHAR drive_letter[3];
	TCHAR *real_name;
	ULONG real_name_size;
	HRESULT res;

	drive_letter[0] = dos_path[0];
	drive_letter[1] = dos_path[1];
	drive_letter[2] = _T('\0');

	real_name_size = MAX_PATH;

	for(;;)
	{
		real_name = malloc(real_name_size*sizeof(TCHAR));

		if(real_name == NULL)
			return NULL;

		if(QueryDosDevice(drive_letter,real_name,real_name_size))
			break;

		if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			free(real_name);
			real_name_size *= 2;
		}else
		{
			free(real_name);
			printf("QueryDosDevice fail %x\n",GetLastError());
			return NULL;
		}
	}

	for(;;)
	{
		res = StringCbCat(real_name,real_name_size*sizeof(TCHAR),&dos_path[2]);

		if(res == S_OK)
			return real_name;
		else if(res != STRSAFE_E_INSUFFICIENT_BUFFER)
			return NULL;

		real_name_size *= 2;
		real_name = realloc(real_name,real_name_size*sizeof(TCHAR));
	}
}
