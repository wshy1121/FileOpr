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
    
    //get userName
    int strPos = 0;
    int findPos = path.find(":", strPos);
    if (findPos <= 0)
    {   trace_printf("NULL");
        return false;
    }
    
    std::string userName = path.substr(strPos, findPos-strPos);
    trace_printf("userName.c_str()  %s", userName.c_str());

    //get passWord
    strPos = findPos+1;
    findPos = path.find("@", strPos);
    if (findPos <= 0)
    {   trace_printf("NULL");
        return false;
    }
    
    std::string passWord = path.substr(strPos, findPos-strPos);
    trace_printf("passWord.c_str()  %s", passWord.c_str());

    //verify ftp
    strPos = findPos+1;
    int cmpFtpRes = path.compare(strPos, 6, "ftp://");
    if (cmpFtpRes != 0)
    {
        return false;
    }
    
    //get cmpFtpRes
    strPos += 6;
    findPos = path.find("/", strPos);
    if (findPos <= 0)
    {   trace_printf("NULL");
        return false;
    }
    std::string ftpSerIp = path.substr(strPos, findPos-strPos);
    trace_printf("ftpSerIp.c_str()  %s", ftpSerIp.c_str());

    //get path
    strPos = findPos;
    std::string ftpPath = path.substr(strPos);
    trace_printf("ftpPath.c_str()  %s", ftpPath.c_str());

    
    return true;
}


