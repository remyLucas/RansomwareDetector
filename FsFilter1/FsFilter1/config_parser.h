#pragma once

#define END_OF_FILE -1
#define NO_CHAR -2
#define READ_ERROR -3

typedef struct cursor
{
	HANDLE config_handle;
	int next_char;
}cursor;
