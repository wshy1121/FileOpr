#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_
#include "IFile.h"
#include "trace_worker.h"

class CFileManager
{
public:    
    typedef enum FileType
    {
        e_errFile = 1,
        e_localeFile,
        e_ftpFile,
    }FileType;
public:    
    IFile *createFile(const std::string &path);
    static CFileManager* instance();
private:
    CFileManager();        
    FileType tranceFileType(const std::string &path);
private:
    static  CFileManager* _instance;
};
#endif

