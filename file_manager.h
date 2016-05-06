#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_
#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include "IFile.h"
#include "trace_worker.h"


class CFileManager
{
public:
    typedef boost::shared_ptr<boost::thread> WorkThread;    
    typedef std::map<IFile::FileKey, IFileHander> FileMap;

    IFileHander getFileHander(const std::string &path);
    static CFileManager* instance();
private:
    CFileManager();
    ~CFileManager();
    bool getFileKey(const std::string &path, IFile::FileKey &fileKey);
    IFileHander createFileHander(IFile::FileKey &fileKey);
    void cleanHanderThread();
    void dispFileMap();
private:
    
    static  CFileManager* _instance;
    FileMap m_fileMap;
    boost::mutex m_fileMapMutex;
    WorkThread  m_cleanHanderThread;
};
#endif

