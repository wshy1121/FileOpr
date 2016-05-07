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
	const int sleepTime = 3* 1000 * 1000;
	LOG_FILE *pLogFile = NULL;
	while(1)
	{
		base::usleep(sleepTime);
		CGuardMutex guardMutex(m_logFileMutex);
		for (LogFileMap::iterator iter = m_logFileMap.begin(); iter != m_logFileMap.end(); ++iter)
		{
			pLogFile = iter->second;
			toFile(pLogFile, pLogFile->content);
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
	LOG_FILE *pLogFile = createLogFile(fileName, clientIpAddr);
	m_logFileMap.insert(std::make_pair(fileKey, pLogFile));
	
	return &pLogFile->traceFileInf;
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
	LOG_FILE *pLogFile = iter->second;
	toFile(pLogFile, pLogFile->content);

	m_logFileMap.erase(iter);
	
	destroyLogFile(pLogFile);
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
    IFileHander fileAddTime = iter->second->traceFileInf.m_fileAddTime;
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
	LOG_FILE *pLogFile = iter->second;
	(pLogFile->content)->append(content);
	TraceFileInf *&traceFileInf = traceInfoId.clientInf->m_traceFileInf;
	traceFileInf->m_fileSize += strlen(content);
}
void CLogOprManager::toFile(LOG_FILE *logFile, CString *pString)
{   trace_worker();
	if (pString->size() == 0)
	{
		return ;
	}

    
	IFileHander fileAddTime = logFile->traceFileInf.m_fileAddTime;
    
	if (fileAddTime == NULL || fileAddTime->open() == false)
	{
	    printf("toFile  fopen  %s failed\n", fileAddTime->getPath().c_str());
	    pString->clear();
		return ;
	}
    fileAddTime->write(pString->c_str(), pString->size());
    pString->clear();
	fileAddTime->close();

	TraceFileInf &traceFileInf = logFile->traceFileInf;
    traceFileInf.m_fileSize = fileAddTime->size();
    if (traceFileInf.m_fileSize > 67108864) //large than 64M
    {
        traceFileInf.m_fileAddTime = CFileManager::instance()->getFileHander(logFile->fileName, logFile->clientIpAddr);
    }

	return ;
}

LOG_FILE *CLogOprManager::createLogFile(char *fileName, std::string &clientIpAddr)
{
	LOG_FILE *pLogFile = new LOG_FILE;
	pLogFile->content = CString::createCString();

    pLogFile->fileName = fileName;   
    pLogFile->clientIpAddr = clientIpAddr;
    
    TraceFileInf &traceFileInf = pLogFile->traceFileInf;
    
    initTraceFileInf(&traceFileInf, fileName, clientIpAddr);
    
    
	return pLogFile;
}

void CLogOprManager::destroyLogFile(LOG_FILE *pLogFile)
{
	CString::destroyCString(pLogFile->content);
	delete pLogFile;
}

bool CLogOprManager::isAvailable()
{	trace_worker();
	bool bRet = CUserManager::instance()->isVerified();
	trace_printf("bRet  %d", bRet);
	return bRet;
}

void CLogOprManager::initTraceFileInf(TraceFileInf *traceFileInf, char *fileName, std::string &clientIpAddr)
{	trace_worker();
	traceFileInf->m_fileName = fileName;
	traceFileInf->m_count = 0;
	traceFileInf->m_fileSize = 0;
	traceFileInf->m_candyCount = 0;
	traceFileInf->m_traceCount = 0;
	
	struct stat statbuf; 
	if (stat(fileName,&statbuf) == 0)
	{
		traceFileInf->m_fileSize = statbuf.st_size;
	}

    traceFileInf->m_fileAddTime = CFileManager::instance()->getFileHander(fileName, clientIpAddr);       

    return ;	
}

void CLogOprManager::getTraceFileList(TraceFileInfMap &traceFileInfMap)
{
    CGuardMutex guardMutex(m_logFileMutex);
    LogFileMap &logFileMap = m_logFileMap;
    
    LogFileMap::iterator iter = logFileMap.begin();
    for (; iter != logFileMap.end(); ++iter)
    {
        LOG_FILE *logFile = iter->second;
        traceFileInfMap[logFile->fileName+logFile->clientIpAddr] = &logFile->traceFileInf;
    }
}


