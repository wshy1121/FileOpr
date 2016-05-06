#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_
#include <map>
#include "IFile.h"
#include "trace_worker.h"


class CFileManager
{
public:    
    IFile *createFile(const std::string &path);
    void destroyFile(IFile *iFile);
    static CFileManager* instance();
private:
    CFileManager();        
    bool getFileKey(const std::string &path, IFile::FileKey &fileKey);
private:
    static  CFileManager* _instance;
    typedef std::map<IFile::FileKey, IFile *> FileMap;
    FileMap m_fileMap;
};
#endif

