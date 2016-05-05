#include <unistd.h>
#include <sys/stat.h>

#include "locale_file.h"
#include "trace_worker.h"
#include "platform_base.h"

CLocaleFile::CLocaleFile(const std::string &path)
:m_fp(NULL)
{
    m_path = path;
}

bool CLocaleFile::open()
{   trace_worker();
    const char *fileName = m_path.c_str();
    
	m_fp = base::fopen (fileName, "a+");
	if (m_fp == NULL)
	{
	    printf("toFile  fopen  %s failed\n", fileName);
		return false;
	}
    
    return true;
}

int CLocaleFile::write(const char *data, int dataLen)
{   trace_worker();
    return fwrite(data, dataLen, 1, m_fp);
}


bool CLocaleFile::close()
{   trace_worker();
    int ret = fclose(m_fp);
    if (ret == 0)
    {        
        m_fp = NULL;
        return true;
    }
    return false;
}

int CLocaleFile::size()
{   trace_worker();
    struct stat statbuf; 
    if (stat(m_path.c_str(),&statbuf) == 0)
    {
        return statbuf.st_size;
    }
    return 0;
}

bool CLocaleFile::clean()
{   trace_worker();
    FILE *fp = NULL;
    trace_printf("fileName  %s", m_path.c_str());
    fp = base::fopen(m_path.c_str(), "w");
    if (fp == NULL)
    {   trace_printf("NULL");
        return true;
    }
    fclose(fp);
    trace_printf("NULL");   
    return true;
} 



