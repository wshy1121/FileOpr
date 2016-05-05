#include "ftp_file.h"
#include "trace_worker.h"

CFtpFile::CFtpFile(const std::string &path)
{   trace_worker();
    m_path = path;
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


