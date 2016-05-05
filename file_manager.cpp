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

CFileManager::FileType CFileManager::tranceFileType(const std::string &path)
{   trace_worker();
    if (path.find("@ftp://") != std::string::npos)
    {   trace_printf("e_ftpFile");
        return e_ftpFile;
    }
    trace_printf("e_localeFile");
    return e_localeFile;
}

//huangyuan1:7ujMko0admin@ftp://127.0.0.1/Log/TraceWorkerDebug.cpp
IFile *CFileManager::createFile(const std::string &path)
{   trace_worker();
    trace_printf("path.c_str()  %s", path.c_str());

    IFile *fileStream = NULL;
    FileType fileType = tranceFileType(path);
    trace_printf("fileType  %d", fileType);
    switch (fileType)
    {
        case e_ftpFile:
            fileStream = new CFtpFile(path);
            break;
        case e_localeFile:
            fileStream = new CLocaleFile(path);
            break;
        default:
            break;
    }
    return fileStream;
}


