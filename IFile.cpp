#include "trace_worker.h"
#include "IFile.h"

IFile::IFile()
{   trace_worker();

}

IFile::~IFile()
{   trace_worker();

}


IFile::FileType IFile::tranceFileType(const std::string &path)
{   trace_worker();
    if (path.find("@ftp://") != std::string::npos)
    {   trace_printf("e_ftpFile");
        return IFile::e_ftpFile;
    }
    trace_printf("e_localeFile");
    return IFile::e_localeFile;
}


