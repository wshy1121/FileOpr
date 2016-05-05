#ifndef _FILE_FACTOR_H_
#define _FILE_FACTOR_H_
#include <string>

class IFile
{
public:
    typedef enum FileType
    {
        e_errFile = 1,
        e_localeFile,
        e_ftpFile,
    }FileType;
    IFile();
    virtual ~IFile();
public:
    static IFile *CreateFile(const std::string &path);
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
    
private:
    static FileType tranceFileType(const std::string &path);
protected:
    std::string m_path;
};

#endif

