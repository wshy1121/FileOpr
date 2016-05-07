#include <stdio.h>
#include "ftp_client.h"

int main()
{
    trace_start("127.0.0.1", 8889, "/home/share/Log/TraceWorkerDebug.cpp");
    CFTPManager ftpManager;
    trace_printf("NULL");
    ftpManager.login2Server("10.17.128.105:21");
    trace_printf("NULL");
    ftpManager.inputUserName("huangyuan1");
    trace_printf("NULL");
    ftpManager.inputPassWord("7ujMko0admin");
    trace_printf("NULL");
    //ftpManager.Put("/Log/Makefile", "Makefile");

    for (int i=0; i<5; ++i)
    {
        ftpManager.WriteData("/Log/WriteTest.txt", "12345\n", 7);
    }
    trace_printf("NULL");
    ftpManager.quitServer();
    return 0;
}

