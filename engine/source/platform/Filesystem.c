#include "Filesystem.h"

#include "core/Logger.h"
#include "core/CMemory.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

b8 FilesystemExists(const char* _path) 
{
    struct stat buffer;
    return stat(_path, &buffer) == 0;
}

b8 FilesystemOpen(const char* _path, FileModes _mode, b8 _binary, FileHandle* _outHandle) 
{
    _outHandle->isValid = false;
    _outHandle->handle = 0;
    const char* modeStr;

    if((_mode & FILE_MODE_READ) != 0 && (_mode & FILE_MODE_WRITE) != 0)
        modeStr = _binary ? "w+b" : "w+";
    else if((_mode & FILE_MODE_READ) != 0 && (_mode & FILE_MODE_WRITE) == 0)
        modeStr = _binary ? "rb" : "r";
    else if((_mode & FILE_MODE_READ) == 0 && (_mode & FILE_MODE_WRITE) != 0)
        modeStr = _binary ? "wb" : "w";
    else
    {
        LOG_ERROR("Invalid mode passed while trying to open file: '%s'", _path);
        return false;
    }

    //attempt to open file
    FILE* file = fopen(_path, modeStr);
    if(!file)
    {
        LOG_ERROR("Error opening file: '%s'", _path);
        return false;
    }

    _outHandle->handle = file;
    _outHandle->isValid = true;

    return true;
}

void FilesystemClose(FileHandle* _handle) 
{
    if(_handle->handle)
    {
        fclose((FILE*)_handle->handle);
        _handle->handle = 0;
        _handle->isValid = false;
    }
}

CAPI b8 FilesystemReadLine(FileHandle* _handle, char** _lineBuf) 
{
    if(_handle->handle)
    {
        //since we are reading a single line, it should be safe to assume this is enough chars
        char buffer[32000];
        if(fgets(buffer, 3200, (FILE*)_handle->handle) != 0)
        {
            u64 length = strlen(buffer);
            *_lineBuf = cAllocate((sizeof(char) * length) + 1, MEMORY_TAG_STRING);
            strcpy(*_lineBuf, buffer);
            return true;
        }
    }

    return false;
}

b8 FilesystemWriteLine(FileHandle* _handle, const char* _text) 
{
    if(_handle->handle)
    {
        i32 result = fputs(_text, (FILE*)_handle->handle);
        if(result != EOF)
            result = fputc('\n', (FILE*)_handle->handle);

        //make sure to flush the stram so it is written to the file immediately.
        //this prevents data loss in the event of a crash.
        fflush((FILE*)_handle->handle);
        return result != EOF;
    }

    return false;
}

CAPI b8 FilesystemRead(FileHandle* _handle, u64 _dataSize, void* _outData, u64* _outBytesRead) 
{
    if(_handle->handle && _outData)
    {
        *_outBytesRead = fread(_outData, 1, _dataSize, (FILE*)_handle->handle);
        if(*_outBytesRead != _dataSize)
            return false;

        return true;
    }

    return false;
}

CAPI b8 FilesystemReadAllBytes(FileHandle* _handle, u8** _outBytes, u64* _outBytesRead) 
{
    if(_handle->handle)
    {
        //file size
        fseek((FILE*)_handle->handle, 0, SEEK_END);
        u64 size = ftell((FILE*)_handle->handle);
        rewind((FILE*)_handle->handle);

        *_outBytes = cAllocate(sizeof(u8) * size, MEMORY_TAG_STRING);
        *_outBytesRead = fread(*_outBytes, 1, size, (FILE*)_handle->handle);
        if(*_outBytesRead != size)
            return false;
        
        return true;
    }

    return false;
}

b8 FileSystemWrite(FileHandle* _handle, u64 _dataSize, const void* _data, u64* _outBytesWritten) 
{
    if(_handle->handle)
    {
        *_outBytesWritten = fwrite(_data, 1, _dataSize, (FILE*)_handle->handle);
        if(*_outBytesWritten != _dataSize)
            return false;
        
        fflush((FILE*)_handle->handle);
        return true;
    }

    return false;
}