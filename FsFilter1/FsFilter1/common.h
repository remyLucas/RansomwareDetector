#pragma once
#include "extension.h"
#include "trace.h"

int is_standard_file(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects);
ULONG is_critical_file(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects);
