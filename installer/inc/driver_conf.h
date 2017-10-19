#ifndef DRIVER_CONF_H
#define DRIVER_CONF_H

#include <windows.h>
#include "common.h"

typedef struct driver_conf
{
	TCHAR* name;
	DWORD start_type;
	TCHAR *instances_name;
	TCHAR *altitude;

	TCHAR *installation_path;
	TCHAR *driver_path;
	TCHAR *origin_driver_path;
}driver_conf;


int install(driver_conf *conf);
int uninstall(TCHAR *service_name);

#endif
