#include "trace_worker.h"
#include "IFile.h"
#include "ftp_file.h"
#include "locale_file.h"

IFile::IFile()
{

}

IFile::~IFile()
{

}

IFile::FileType IFile::tranceFileType(const std::string &path)
{   trace_worker();
    if (path.find("@ftp://") != std::string::npos)
    {   trace_printf("e_ftpFile");
        return e_ftpFile;
    }
    trace_printf("e_localeFile");
    return e_localeFile;
}

//admin:7ujMko0admin@ftp://127.0.0.1/Log/TraceWorkerDebug.cpp
IFile *IFile::CreateFile(const std::string &path)
{   trace_worker();
    trace_printf("path.c_str()  %s", path.c_str());

    IFile *fileStream = NULL;
    IFile::FileType fileType = IFile::tranceFileType(path);
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

