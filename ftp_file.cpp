#include "ftp_file.h"
#include "trace_worker.h"

CFtpFile::CFtpFile(const std::string &fileInf)
{   trace_worker();
    m_path = fileInf;
}

bool CFtpFile::open()
{   trace_worker();
    return true;
}

int CFtpFile::write(const char *data, int dataLen)
{   trace_worker();
    return 0;
}


bool CFtpFile::close()
{   trace_worker();
    return true;
}

int CFtpFile::size()
{   trace_worker();
    return 0;
}

bool CFtpFile::clean()
{   trace_worker();
    return true;
}

//huangyuan1:7ujMko0admin@ftp://127.0.0.1/Log/TraceWorkerDebug.cpp
bool CFtpFile::parseKey(const std::string &path, IFile::FileKey &fileKey)
{   trace_worker();
    return true;
}


