#ifndef _FTP_FILE_H_
#define _FTP_FILE_H_
#include "IFile.h"
#include "ftp_client.h"

class CFtpFile : public IFile
{
public:
    CFtpFile(IFile::FileKey &fileKey);
    ~CFtpFile();
public:
    static bool parseKey(const std::string &path, IFile::FileKey &fileKey);
public:    
    virtual bool open();
    virtual int write(const char *data, int dataLen);
    virtual bool close();
    virtual long size();
    virtual bool clean();
    virtual bool isOnline();
    virtual void reConnect();
private:
    std::string m_userName;
    std::string m_passWord;
    std::string m_ftpSerIp;
    bool m_isOnLine;
    CFTPManager m_ftpManager;
};
#endif

