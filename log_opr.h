#ifndef _LOG_OPR_H_
#define _LOG_OPR_H_
#include "link_tool.h"
#include "mem_calc.h"
#include "file_manager.h"

class CLogOprManager;

typedef struct TraceFileInf
{
	std::string m_fileName;
	long m_fileSize;
	int m_count;
	int m_candyCount;
	int m_traceCount;
	std::string m_lastCandy;
    IFileHander m_fileAddTime;
    size_t m_rxbps;
}TraceFileInf;

typedef struct LogDataInf
{
	typedef enum
	{
		e_none,
		e_writeFile,
		e_openFile,
		e_closeFile
	}LogDataOpr;
	LogDataOpr m_opr;
	TraceInfoId m_traceInfoId;
	char *m_content;
}LogDataInf;

typedef struct LOG_DATA
{
	LogDataInf logDataInf;
	struct node node;
}LOG_DATA;	

#define logDataContain(ptr)  container_of(ptr, LOG_DATA, node)

class CLogFile
{
public:
    friend class CLogOprManager;
    CLogFile(char *fileName, std::string &clientIpAddr);
    ~CLogFile();
private:
    void initTraceFileInf(TraceFileInf *traceFileInf, char *fileName, std::string &clientIpAddr);
private:
	std::string m_fileName;
    std::string m_clientIpAddr;
	base::CString *m_content;
	TraceFileInf m_traceFileInf;
	
};
class  CLogOprManager
{
public:
	typedef std::map<std::string, TraceFileInf *> TraceFileInfMap;
	typedef std::map<int, std::shared_ptr<CLogFile>> LogFileMap;	
	static CLogOprManager *instance();
	TraceFileInf *openFile(int fileKey, char *fileName, std::string &clientIpAddr);
	bool closeFile(int fileKey);
	bool cleanFile(int fileKey);
	void writeFile(TraceInfoId &traceInfoId,char *content);	
	void getTraceFileList(TraceFileInfMap &traceFileInfMap);
private:
	CLogOprManager();
private:
	static void* threadFunc(void *pArg);
	void threadProc();	
	void toFile(CLogFile *logFile, base::CString *pString);
	bool isAvailable();
private:
	static CLogOprManager *_instance;
	base::CPthreadMutex m_logFileMutex;
	CBase::pthread_t m_threadId;
	const char *m_logName;
	LogFileMap m_logFileMap;
};

#endif


