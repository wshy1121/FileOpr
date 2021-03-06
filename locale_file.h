#ifndef _LOCALE_FILE_H_
#define _LOCALE_FILE_H_
#include <boost/thread/mutex.hpp>
#include "IFile.h"

class CLocaleFile : public IFile
{
public:
    CLocaleFile(IFile::FileKey &fileKey);
    ~CLocaleFile();
public:
    static bool parseKey(const std::string &path, IFile::FileKey &fileKey);
public:    
    virtual bool open();
    virtual int write(const char *data, int dataLen);
    virtual bool close();
    virtual long size();
    virtual bool clean();
    virtual bool isOnline(){return true;}
    virtual void reConnect(){}
private:
    boost::mutex m_fpMutex;
    FILE *m_fp;
};
#endif

