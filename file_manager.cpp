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
        fileKey.fileInf.clear();
    }
    return bRet;
}

IFileHander CFileManager::createFile(const std::string &path)
{   trace_worker();
    trace_printf("path.c_str()  %s", path.c_str());

    IFile::FileKey fileKey;
    
    IFileHander fileStream;
    IFile::FileType fileType = IFile::tranceFileType(path);
    trace_printf("fileType  %d", fileType);
    switch (fileType)
    {
        case IFile::e_ftpFile:
            fileStream = IFileHander(new CFtpFile(path));
            break;
        case IFile::e_localeFile:
            fileStream = IFileHander(new CLocaleFile(path));
            break;
        default:
            break;
    }
    return fileStream;
}



