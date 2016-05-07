#ifndef _FILE_FACTOR_H_
#define _FILE_FACTOR_H_
#include <string>
#include <boost/shared_ptr.hpp>

class IFile
{
public:    
    typedef enum FileType
    {
        e_errFile = 1,
        e_localeFile,
        e_ftpFile,
    }FileType;
    typedef struct FileKey
    {
        FileType type;
        std::string serInf;
        std::string path;
        bool operator < (const struct FileKey &fileKey) const
    	{
    		if (type < fileKey.type)
    		{
    			return true;
    		}
    		else if (type == fileKey.type)
    		{
    			return serInf < fileKey.serInf;
    		}
    		return false;
    	}
        
    }FileKey;

public:
    IFile();
    virtual ~IFile();
public:    
    static FileType tranceFileType(const std::string &path);
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
    virtual long size() = 0;
    virtual bool clean() = 0;
    
protected:
    std::string m_path;
};

typedef boost::shared_ptr<IFile> IFileHander;
#endif

