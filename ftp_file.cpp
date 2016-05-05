#include "ftp_file.h"

CFtpFile::CFtpFile(const std::string &path)
{
    m_path = path;
}

bool CFtpFile::open()
{
    return true;
}

int CFtpFile::write(const char *data, int dataLen)
{
    return 0;
}


bool CFtpFile::close()
{
    return true;
}

int CFtpFile::size()
{
    return 0;
}

bool CFtpFile::clean()
{
    return true;
}


