#include <windows.h> 
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include "original.h"
#include "vasim.h"


int _tmain(VOID)
{
    /*
    original server;
    server.Init();
    */
    vasim server;
    server.Init();

    return 0;
}

