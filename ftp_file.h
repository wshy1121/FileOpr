#ifndef _FTP_FILE_H_
#define _FTP_FILE_H_
#include "IFile.h"

class CFtpFile : public IFile
{
public:
    CFtpFile(const std::string &fileInf);
public:
    static bool parseKey(const std::string &path, IFile::FileKey &fileKey);
public:    
    virtual bool open();
    virtual int write(const char *data, int dataLen);
    virtual bool close();
    virtual int size();
    virtual bool clean();
};
#endif

