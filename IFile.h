#ifndef _FILE_FACTOR_H_
#define _FILE_FACTOR_H_
#include <string>

class IFile
{
public:
    IFile();
    virtual ~IFile();
public:
    std::string &getPath()
    {
        return m_path;
    }
    void setPath(const std::string &path)
    {
        m_path = path;
    }
    virtual bool open() = 0;
    virtual int write(const char *data, int dataLen) = 0;
    virtual bool close() = 0;
    virtual int size() = 0;
    virtual bool clean() = 0;
    
protected:
    std::string m_path;
};

#endif

