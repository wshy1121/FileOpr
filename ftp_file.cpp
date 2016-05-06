#include "ftp_file.h"
#include "trace_worker.h"
#include "Json/json.h"

CFtpFile::CFtpFile(IFile::FileKey &fileKey)
{   trace_worker();
    std::string &serInf = fileKey.serInf;
    
    Json::Reader reader;  
    Json::Value fileInfValue;
    if (reader.parse(serInf, fileInfValue) == false)  
    {   trace_printf("NULL");
        return ;
    }

    m_userName = fileInfValue["userName"].asString();
    m_passWord = fileInfValue["passWord"].asString();
    m_ftpSerIp = fileInfValue["ftpSerIp"].asString();
    m_path = fileKey.path;
    
    trace_printf("%s  %s  %s  %s", m_userName.c_str(), m_passWord.c_str(), m_ftpSerIp.c_str(), m_path.c_str());
}

CFtpFile::~CFtpFile()
{   trace_worker();

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

    Json::Value fileInfValue;
    fileInfValue["userName"] = userName;
    fileInfValue["passWord"] = passWord;
    fileInfValue["ftpSerIp"] = ftpSerIp;
    
    fileKey.type = e_ftpFile;
    fileKey.serInf = fileInfValue.toStyledString();
    fileKey.path = ftpPath;
    trace_printf("fileKey.serInf.c_str()  %s", fileKey.serInf.c_str());
    return true;
}


