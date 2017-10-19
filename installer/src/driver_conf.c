#include "driver_conf.h"

int compute_driver_path(driver_conf *conf)
{
	UINT32 name_len,path_len,total_len;

	name_len = lstrlen(conf->name);
	path_len = lstrlen(conf->installation_path);

	total_len = name_len + path_len + 6; // + '\' + ".sys" + '\0'

	conf->driver_path = malloc(total_len*sizeof(TCHAR));

	if(conf->driver_path == NULL)
		return -1;
	
	memcpy(conf->driver_path,conf->installation_path,path_len*sizeof(TCHAR));
	conf->driver_path[path_len] = _T('\\');
	memcpy(&conf->driver_path[path_len+1],conf->name,name_len);
	conf->driver_path[path_len+1+name_len] = _T('.');
	conf->driver_path[path_len+2+name_len] = _T('s');
	conf->driver_path[path_len+3+name_len] = _T('y');
	conf->driver_path[path_len+4+name_len] = _T('s');
	conf->driver_path[path_len+5+name_len] = _T('\0');

	return 0;
}

int copy_driver(TCHAR *origin, driver_conf *conf)
{
	/*TCHAR *fullname;
	int path_len,name_len;

	path_len = lstrlen(conf->driver_path);
	name_len = lstrlen(conf->name);

	fullname = malloc((path_len + name_len + 1)*sizeof(TCHAR));

	memcpy(fullname,conf->driver_path,path_len*sizeof(TCHAR));
	memcpy(&fullname[path_len],conf->name,name_len*sizeof(TCHAR));
	fullname[path_len+name_len] = _T('\0');*/

#ifdef UNICODE
	printf("%S %S\r\n",conf->driver_path,origin);
#else
	printf("%s %s\r\n",conf->driver_path,origin);
#endif

	if(CopyFile(origin,conf->driver_path,FALSE) == 0)
	{
		printf("CopyFile failed %x\r\n",GetLastError());
		//free(fullname);
		return -1;
	}

	//free(fullname);

	return 0;
}

int install_service(driver_conf *conf)
{
	SC_HANDLE services_manager,service;
	TCHAR dependencies[8*sizeof(TCHAR)] = {_T('F'),_T('l'),_T('t'),_T('M'),_T('g'),_T('r'),_T('\0'),_T('\0')};

	services_manager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE);

	if (services_manager == NULL)
	{
		printf("cannot open service manager %d\r\n",GetLastError());
		return -1;
	}

	service = CreateService(services_manager,conf->name,conf->name, SC_MANAGER_CREATE_SERVICE, SERVICE_FILE_SYSTEM_DRIVER, conf->start_type, SERVICE_ERROR_NORMAL,
		conf->driver_path,
		NULL,NULL, dependencies,NULL,NULL);

	if (service == NULL)
	{
		CloseServiceHandle(services_manager);
		printf("cannot create service %d\r\n",GetLastError());
		return -1;
	}

	CloseServiceHandle(service);
	CloseServiceHandle(services_manager);
	return 0;
}

//ne pas oublier de free base_key_name et de close les keys
int add_registry_keys(driver_conf *conf)
{
	TCHAR *base_key_name,*key_name,*valuename,*value;
	UINT32 name_len;
	HKEY base_key,subkey,key;
	LONG error;
	int res;

	res = 0;
	name_len = lstrlen(conf->name);

	base_key_name = malloc((name_len + 35)*sizeof(TCHAR)); // + "SYSTEM\CurrentControlSet\Services\" + '\0'

	if(base_key_name == NULL)
		return -1;

	key_name = TEXT("SYSTEM\\CurrentControlSet\\Services\\");
	memcpy(base_key_name,key_name,34 * sizeof(TCHAR));
	memcpy(&base_key_name[34],conf->name,name_len+1); // + '\0'

	error = RegCreateKeyEx(HKEY_LOCAL_MACHINE,base_key_name,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&base_key,NULL);
	if(error != ERROR_SUCCESS)
	{
		printf("cannot open base registery key %d\r\n",error);
		res = -1;
		goto error_1;
	}

	key_name = TEXT("Instances");
	error = RegCreateKeyEx(base_key,key_name,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&key,NULL);
	if(error != ERROR_SUCCESS)
	{
		printf("cannot create registery key %d\r\n",error);
		res = -1;
		goto error_2;
	}

	valuename = TEXT("DefaultInstance");

	error = RegSetKeyValue(key,NULL,valuename,REG_SZ,conf->name,name_len);

	if(error != ERROR_SUCCESS)
	{
		printf("cannot set value %d\r\n",error);
		res = -1;
		goto error_3;
	}

	error = RegCreateKeyEx(key,conf->name,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subkey,NULL);
	if(error != ERROR_SUCCESS)
	{
		printf("cannot create registery key 2 %d\r\n",error);
		res = -1;
		goto error_3;
	}

	valuename = TEXT("Altitude");

	error = RegSetKeyValue(subkey,NULL,valuename,REG_SZ,conf->altitude,lstrlen(conf->altitude));

	if(error != ERROR_SUCCESS)
	{
		printf("cannot set value 2 %d\r\n",error);
		res = -1;
		goto error_4;
	}

	valuename = TEXT("path");
	value = convert_path(conf->installation_path);

	error = RegSetKeyValue(base_key,NULL,valuename,REG_SZ,value,lstrlen(value));

	if(error != ERROR_SUCCESS)
	{
		printf("cannot set value 2 %d\r\n",error);
		res = -1;
		goto error_4;
	}

	error_4 :
		RegCloseKey(subkey);
	error_3 :
		RegCloseKey(key);
	error_2 :
		RegCloseKey(base_key);
	error_1 :
		free(base_key_name);

	return res;
}

int install(driver_conf *conf)
{
	compute_driver_path(conf);

	copy_driver(conf->origin_driver_path,conf);

	install_service(conf);

	add_registry_keys(conf);

	return 0;
}

int uninstall(TCHAR *service_name)
{
	SC_HANDLE services_manager,service;

	services_manager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE);

	if (services_manager == NULL)
	{
		printf("cannot open service manager %d\r\n",GetLastError());
		return -1;
	}

	service = OpenService(services_manager,service_name,DELETE);

	if (service == NULL)
	{
		printf("cannot open service %d\r\n",GetLastError());
		return 0;
	}

	if(DeleteService(service) == 0)
	{
		printf("cannot delete service %d\r\n",GetLastError());
		return 0;
	}

	return 0;
}
