#include <stdio.h>
#include "example_dll.h"

__stdcall void hello(const char *s)
{
        printf("Hello %s\n", s);
}
int say2()
{
        return 2;
}

int say37(int i)
{
        return 37+i;
}

char* say44()
{
        char *output = "yo44";
		return (output); 
}

int Double(int x)
{
        return 2 * x;
}
void CppFunc(void)
{
        puts("CppFunc");
}