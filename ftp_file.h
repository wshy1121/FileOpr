#ifndef _FTP_FILE_H_
#define _FTP_FILE_H_
#include "IFile.h"

class CFtpFile : public IFile
{
public:
    CFtpFile(const std::string &path);
public:    
    virtual bool open();
    virtual int write(const char *data, int dataLen);
    virtual bool close();
    virtual int size();
};
#endif

