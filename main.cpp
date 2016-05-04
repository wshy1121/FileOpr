#include <stdio.h>
#include "ftp_client.h"

int main()
{
    trace_start("127.0.0.1", 8889, "/root/nfs/Log/TraceWorkerDebug.cpp");
    CFTPManager ftpManager;
    ftpManager.login2Server("10.17.128.105:21");
    ftpManager.inputUserName("huangyuan1");
    ftpManager.inputPassWord("7ujMko0admin");
    ftpManager.Put("/Log/1.txt", "libFileOpr.a");
    return 0;
}

