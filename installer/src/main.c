#include <stdio.h>
#include <stdlib.h>
//#define UNICODE
#include <windows.h>
#include <string.h>
#include "common.h"
#include "driver_conf.h"

/*int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND win;
	MSG msg;

	win = CreateWindow(NULL,NULL,WS_VISIBLE,
	  CW_USEDEFAULT,
	  CW_USEDEFAULT,
	  800,
	  600,
	  NULL,
	  NULL,
	  hInstance,
	  NULL
	);

	while(GetMessage(&msg,win,WM_KEYFIRST,WM_KEYLAST));
}*/

void usage(char *path)
{
#ifdef UNICODE
	printf("usage : %S [install|uninstall] name path altitude start_type\r\n\r\n",path);
#else
	printf("usage : %s [install|uninstall] name path altitude service_start_type\r\n\r\n",path);
#endif
	printf("SERVICE_BOOT_START %d\r\n",SERVICE_BOOT_START);
	printf("SERVICE_SYSTEM_START %d\r\n",SERVICE_SYSTEM_START);
	printf("SERVICE_AUTO_START %d\r\n",SERVICE_AUTO_START);
	printf("SERVICE_DEMAND_START %d\r\n",SERVICE_DEMAND_START);
	printf("SERVICE_DISABLED %d\r\n",SERVICE_DISABLED);
}

int _tmain(int argc, TCHAR **argv)
{
	driver_conf conf;

	if(argc < 3)
	{
		usage(argv[0]);
		return 1;
	}

#ifdef UNICODE
	if(wcscmp(argv[1],TEXT("install")) == 0)
#else
	if(strcmp(argv[1],TEXT("install")) == 0)
#endif
	{
		printf("nbr args %d\n",argc);
		if(argc == 7)
		{
			conf.origin_driver_path = argv[6];
		}else if(argc == 6)
		{
			conf.origin_driver_path = TEXT("./FsFilter1");
		}else
		{
			usage(argv[0]);
			return 1;
		} 

		conf.name = argv[2];

#ifdef UNICODE
		conf.start_type = _wtoi(argv[5]);
#else
		conf.start_type = atoi(argv[5]);
#endif
		if(conf.start_type < 0 || conf.start_type > 4)
		{
			usage(argv[0]);
			return 1;
		}

		conf.instances_name = argv[2];
		
		conf.altitude = argv[4];

		conf.installation_path = argv[3];

		install(&conf);
	}
#ifdef UNICODE
	else if(wcscmp(argv[1],TEXT("uninstall")) == 0)
#else
	else if(strcmp(argv[1],TEXT("uninstall")) == 0)
#endif
	{
		uninstall(argv[2]);
	}else
	{
		usage(argv[0]);
		return 1;
	}

	return 0;
}
