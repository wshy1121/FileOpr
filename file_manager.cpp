#include "file_manager.h"
#include "ftp_file.h"
#include "locale_file.h"
#include "trace_base.h"

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
{   trace_worker();
    m_cleanHanderThread = WorkThread(new boost::thread(boost::bind(&CFileManager::cleanHanderThread,this)));
    m_checkOnlineThread = WorkThread(new boost::thread(boost::bind(&CFileManager::checkOnlineThread,this)));
}

CFileManager::~CFileManager()
{   trace_worker();
    if(m_cleanHanderThread != NULL)
    {
        m_cleanHanderThread->interrupt();
        m_cleanHanderThread->join();
    }

    if(m_checkOnlineThread != NULL)
    {
        m_checkOnlineThread->interrupt();
        m_checkOnlineThread->join();
    }

    {
        boost::unique_lock<boost::mutex> lock(m_fileMapMutex);
        m_fileMap.clear();
    }
    
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

IFileHander CFileManager::getFileHander(const std::string &path, std::string &clientIpAddr)
{   trace_worker();
    trace_printf("path.c_str()  %s", path.c_str());
    std::string filePath = path;
    
    filePath = addFileAddr(filePath, clientIpAddr);

    IFile::FileKey fileKey;
    if (getFileKey(filePath, fileKey) == false)
    {   trace_printf("NULL");
        return NULL;
    }

    
    IFileHander fileHander;
    {
        boost::unique_lock<boost::mutex> lock(m_fileMapMutex);
        
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
    }
    
    std::string &fileNameAddTime = fileKey.path;
    fileNameAddTime = addFileTime(fileNameAddTime);
    fileHander->setPath(fileNameAddTime);

    printf("fileNameAddTime.c_str()  %s\n", fileNameAddTime.c_str());
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

void CFileManager::cleanHanderThread()
{
	FileMap::iterator iter;	
    int sleepTime = 3*1000*1000;
    while (1)
    {   trace_worker();
        boost::this_thread::interruption_point();
        {
            boost::unique_lock<boost::mutex> lock(m_fileMapMutex);   
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
        
        CBase::usleep(sleepTime);
    }
    
}

void CFileManager::checkOnlineThread()
{
    FileMap::iterator iter; 
    int sleepTime = 3*1000*1000;
    FileMap fileMap;
    while (1)
    {   trace_worker();
        boost::this_thread::interruption_point();
        {
            boost::unique_lock<boost::mutex> lock(m_fileMapMutex);   
            fileMap = m_fileMap;
        }
        
        for(iter = fileMap.begin(); iter!=fileMap.end(); ++iter)
        {
            IFileHander &fileHander = iter->second;
            trace_printf("fileHander.use_count  %ld", fileHander.use_count());
            if (fileHander->isOnline() == false)
            {   trace_printf("NULL");
                fileHander->reConnect();
            }
        }
        fileMap.clear();
        
        CBase::usleep(sleepTime);
    }
    
}

void CFileManager::dispFileMap()
{   trace_worker();
    boost::unique_lock<boost::mutex> lock(m_fileMapMutex);
    FileMap::iterator iter;	
    for(iter = m_fileMap.begin(); iter!=m_fileMap.end(); ++iter)
    {
        IFile::FileKey fileKey = iter->first; 
        trace_printf("fileKey.type, fileKey.serInf  %d  %s", fileKey.type, fileKey.serInf.c_str());
        
    }
}


std::string CFileManager::nowTime()
{
    char nowTime[64];
    time_t now;
    struct tm  *w;
    time(&now);  
    w=localtime(&now);
    CBase::snprintf(nowTime, sizeof(nowTime), "%04d%02d%02d-%02d%02d%02d",w->tm_year+1900,w->tm_mon+1,w->tm_mday,w->tm_hour,w->tm_min,w->tm_sec);
    return nowTime;
}


std::string &CFileManager::addFileAddr(std::string &fileName, std::string &clientIpAddr)
{
    int nameIndex = fileName.find_last_of('/');
    if (nameIndex > 0)
    {
        fileName = fileName.insert(nameIndex, '/' + clientIpAddr);  
    }
    
    return fileName;
}

std::string &CFileManager::addFileTime(std::string &fileName)
{
    int nameIndex = fileName.find_last_of('.');
    if (nameIndex > 0)
    {
        fileName = fileName.insert(nameIndex, '_' + nowTime());  
    }
    
    return fileName;
}



