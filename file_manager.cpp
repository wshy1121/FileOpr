#include <boost/thread/mutex.hpp>
#include "file_manager.h"
#include "ftp_file.h"
#include "locale_file.h"


static boost::mutex g_insMutex;

CFileManager* CFileManager::_instance = NULL;

CFileManager* CFileManager::instance() 
{
	if (NULL == _instance)
	{
		boost::unique_lock<boost::mutex> lock(g_insMutex);
		if (NULL == _instance)
		{
			_instance = new CFileManager;
		}
	}
	return _instance;
}

CFileManager::CFileManager()
{
}

bool CFileManager::getFileKey(const std::string &path, IFile::FileKey &fileKey)
{   trace_worker();
    fileKey.type = IFile::tranceFileType(path);

    bool bRet = false;
    IFile::FileType &fileType = fileKey.type;
    switch (fileType)
    {
        case IFile::e_ftpFile:
            bRet = CFtpFile::parseKey(path, fileKey);
            break;
        case IFile::e_localeFile:
            bRet = CLocaleFile::parseKey(path, fileKey);
            break;
        default:
            break;
    }
    
    if (bRet == false)
    {
        fileKey.type = IFile::e_errFile;
        fileKey.serInf.clear();
        fileKey.path.clear();
    }
    return bRet;
}

IFileHander CFileManager::getFileHander(const std::string &path)
{   trace_worker();
    trace_printf("path.c_str()  %s", path.c_str());
    
    IFile::FileKey fileKey;
    if (getFileKey(path, fileKey) == false)
    {   trace_printf("NULL");
        return NULL;
    }

    
    IFileHander fileHander;
    FileMap::iterator iter = m_fileMap.find(fileKey);
    if (iter != m_fileMap.end())
    {   trace_printf("NULL");     
        fileHander = iter->second;
    }
    else
    {   trace_printf("NULL");  
        fileHander = createFileHander(fileKey);
        if (fileHander != NULL)
        {   trace_printf("NULL");
            m_fileMap.insert(std::make_pair(fileKey, fileHander));
        }
    }
    
    cleanFileHander();
    fileHander->setPath(fileKey.path);
    return fileHander;
}

IFileHander CFileManager::createFileHander(IFile::FileKey &fileKey)
{   trace_worker();
    IFileHander fileHander;
    
    switch (fileKey.type)
    {
        case IFile::e_ftpFile:
            fileHander = IFileHander(new CFtpFile(fileKey));
            break;
        case IFile::e_localeFile:
            fileHander = IFileHander(new CLocaleFile(fileKey));
            break;
        default:
            break;
    }
    return fileHander;
}

void CFileManager::cleanFileHander()
{   trace_worker();
	FileMap::iterator iter;	
	
    for(iter = m_fileMap.begin(); iter!=m_fileMap.end();)
    {
        IFileHander &fileHander = iter->second;
        trace_printf("fileHander.use_count  %ld", fileHander.use_count());
        if (fileHander.use_count() == 1)
        {   trace_printf("NULL");
            m_fileMap.erase(iter++);
        }
        else
        {   trace_printf("NULL");
            ++iter;
        }
    }
}

void CFileManager::dispFileMap()
{   trace_worker();
    FileMap::iterator iter;	
    for(iter = m_fileMap.begin(); iter!=m_fileMap.end(); ++iter)
    {
        IFile::FileKey fileKey = iter->first; 
        trace_printf("fileKey.type, fileKey.serInf  %d  %s", fileKey.type, fileKey.serInf.c_str());
        
    }
}