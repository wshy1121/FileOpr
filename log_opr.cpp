#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <time.h>

#include "stdafx.h"
#include "string_base.h"
#include "log_opr.h"
#include "mem_calc.h"
#include "time_calc.h"
#include "trace_server.h"

using namespace base;
extern CPthreadMutex g_insMutexCalc;

CLogOprManager *CLogOprManager::_instance = NULL;
CLogOprManager::CLogOprManager() : m_logName("./Debug.cpp")
{
	CBase::pthread_create(&m_threadId, NULL,threadFunc,NULL);
}
CLogOprManager *CLogOprManager::instance()
{
	if (NULL == _instance)
	{
		CGuardMutex guardMutex(g_insMutexCalc);
		if (NULL == _instance)
		{
			_instance = new CLogOprManager;
		}
	}
	return _instance;
}

void CLogOprManager::threadProc()
{	
	const int sleepTime = 4 * 1000 * 1000;
	std::shared_ptr<CLogFile> logFile;
	while(1)
	{
		base::usleep(sleepTime);
		CGuardMutex guardMutex(m_logFileMutex);
		for (LogFileMap::iterator iter = m_logFileMap.begin(); iter != m_logFileMap.end(); ++iter)
		{
			logFile = iter->second;
			toFile(logFile.get(), logFile->m_content);
		}
	}
}

void* CLogOprManager::threadFunc(void *pArg)
{
	CLogOprManager::instance()->threadProc();
	return NULL;
}

TraceFileInf *CLogOprManager::openFile(int fileKey, char *fileName, std::string &clientIpAddr)
{
	CGuardMutex guardMutex(m_logFileMutex);
	LogFileMap::iterator iter = m_logFileMap.find(fileKey);
	if (iter != m_logFileMap.end())
	{
		m_logFileMutex.Leave();
		closeFile(fileKey);
		m_logFileMutex.Enter();
	}
	printf("openFile  fileKey, fileName  %d  %s\n", fileKey, fileName);
    std::shared_ptr<CLogFile> logFile(new CLogFile(fileName, clientIpAddr));
	m_logFileMap.insert(std::make_pair(fileKey, logFile));
	
	return &logFile->m_traceFileInf;
}

bool CLogOprManager::closeFile(int fileKey)
{
	CGuardMutex guardMutex(m_logFileMutex);
	printf("closeFile  fileKey  %d\n", fileKey);
	LogFileMap::iterator iter = m_logFileMap.find(fileKey);
	if (iter == m_logFileMap.end())
	{
		return false;
	}
	std::shared_ptr<CLogFile> logFile = iter->second;
	toFile(logFile.get(), logFile->m_content);

	m_logFileMap.erase(iter);
	return true;
}

bool CLogOprManager::cleanFile(int fileKey)
{	trace_worker();
	CGuardMutex guardMutex(m_logFileMutex);
	LogFileMap::iterator iter = m_logFileMap.find(fileKey);
	if (iter == m_logFileMap.end())
    {
        return false;
    }
    IFileHander fileAddTime = iter->second->m_traceFileInf.m_fileAddTime;
    if (fileAddTime == NULL)
    {
        return false;
    }
    fileAddTime->clean();
	return true;
}


void CLogOprManager::writeFile(TraceInfoId &traceInfoId, char *content)
{
	if (!isAvailable())
	{
		return;
	}

	CGuardMutex guardMutex(m_logFileMutex);
	LogFileMap::iterator iter = m_logFileMap.find(traceInfoId.clientId);
	if (iter == m_logFileMap.end())
	{
		printf("writeFile failed! no file opened\n");
		return ;
	}
	std::shared_ptr<CLogFile> logFile = iter->second;
	(logFile->m_content)->append(content);
	TraceFileInf *&traceFileInf = traceInfoId.clientInf->m_traceFileInf;
	traceFileInf->m_fileSize += strlen(content);
}

void CLogOprManager::toFile(CLogFile *logFile, CString *pString)
{   trace_worker();
	if (pString->size() == 0)
	{
		return ;
	}

    
	IFileHander fileAddTime = logFile->m_traceFileInf.m_fileAddTime;
    
	if (fileAddTime == NULL || fileAddTime->open() == false)
	{
	    printf("toFile  fopen  %s failed\n", fileAddTime->getPath().c_str());
	    pString->clear();
		return ;
	}

    TraceFileInf &traceFileInf = logFile->m_traceFileInf;
    traceFileInf.m_rxbps = pString->size() / 4;
    
    fileAddTime->write(pString->c_str(), pString->size());
    pString->clear();
	fileAddTime->close();

    traceFileInf.m_fileSize = fileAddTime->size();
    if (traceFileInf.m_fileSize > 67108864) //large than 64M
    {
        traceFileInf.m_fileAddTime = CFileManager::instance()->getFileHander(logFile->m_fileName, logFile->m_clientIpAddr);
    }

	return ;
}


bool CLogOprManager::isAvailable()
{	trace_worker();
	bool bRet = CUserManager::instance()->isVerified();
	trace_printf("bRet  %d", bRet);
	return bRet;
}



void CLogOprManager::getTraceFileList(TraceFileInfMap &traceFileInfMap)
{
    CGuardMutex guardMutex(m_logFileMutex);
    LogFileMap &logFileMap = m_logFileMap;
    
    LogFileMap::iterator iter = logFileMap.begin();
    for (; iter != logFileMap.end(); ++iter)
    {
        std::shared_ptr<CLogFile> logFile = iter->second;
        traceFileInfMap[logFile->m_fileName+logFile->m_clientIpAddr] = &logFile->m_traceFileInf;
    }
}

CLogFile::CLogFile(char *fileName, std::string &clientIpAddr)
{
    m_content = CString::createCString();

    m_fileName = fileName;   
    m_clientIpAddr = clientIpAddr;
    
    TraceFileInf &traceFileInf = m_traceFileInf;
    
    initTraceFileInf(&traceFileInf, fileName, clientIpAddr);  
}


CLogFile::~CLogFile()
{
    CString::destroyCString(m_content);
}


void CLogFile::initTraceFileInf(TraceFileInf *traceFileInf, char *fileName, std::string &clientIpAddr)
{	trace_worker();
	traceFileInf->m_fileName = fileName;
	traceFileInf->m_count = 0;
	traceFileInf->m_fileSize = 0;
	traceFileInf->m_candyCount = 0;
	traceFileInf->m_traceCount = 0;
    traceFileInf->m_rxbps = 0;
	
	struct stat statbuf; 
	if (stat(fileName,&statbuf) == 0)
	{
		traceFileInf->m_fileSize = statbuf.st_size;
	}

    traceFileInf->m_fileAddTime = CFileManager::instance()->getFileHander(fileName, clientIpAddr);       

    return ;	
}


