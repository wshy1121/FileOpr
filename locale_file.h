#ifndef _LOCALE_FILE_H_
#define _LOCALE_FILE_H_
#include "IFile.h"

class CLocaleFile : public IFile
{
public:
    CLocaleFile(const std::string &path);
public:    
    virtual bool open();
    virtual int write(const char *data, int dataLen);
    virtual bool close();
    virtual int size();
private:
    FILE *m_fp;
};
#endif

