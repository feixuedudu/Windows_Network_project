#include "common.h"

CRITICAL_SECTION g_csout = { 0 };

void print(const char *pszFormat, ...)
{
	va_list arg_ptr;
	va_start(arg_ptr, pszFormat);

	int nValue = va_arg(arg_ptr, int);
	printf("next_parm : %d\n", nValue);
	double dValue = va_arg(arg_ptr, double);
	printf("next_parm : %f\n", dValue);
	char* pValue = va_arg(arg_ptr, char *);
	printf("next_parm : %s\n", pValue);

	va_end(arg_ptr);
}