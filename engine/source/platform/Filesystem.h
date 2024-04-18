#pragma once

#include "Defines.h"

// holds handle to a file
typedef struct FileHandle {
    // opaque handle to internal file
    void* handle;
    b8 isValid;
} FileHandle;

typedef enum FileModes {
    FILE_MODE_READ = 0x1,
    FILE_MODE_WRITE = 0x2
} FileModes;

/**
 * Checks if a file with the given path exists.
 * @param path The path of the file to be checked.
 * @returns True if exists, false otherwise.
 */
CAPI b8 FilesystemExists(const char* _path);

/**
 * Attempt to open file located at the path.
 * @param path The path of the file to be opened.
 * @param mode Mode flags for the file when opened (read/write). See FileModes enum in Filesystem.h
 * @param binary Indicates if the file should be opened in binary mode.
 * @param outHandle A pointer to a FileHandle struct which holds the handle information
 * @returns True if opened successfully, otherwise false.
 */
CAPI b8 FilesystemOpen(const char* _path, FileModes _mode, b8 _binary, FileHandle* _outHandle);

/**
 * Closes the provided handle to a file.
 * @param handle A pointer to a FileHandle struct which holds the handle to be closed.
 */
CAPI void FilesystemClose(FileHandle* _handle);

/**
 * Reads up to a newline or EOF. Allocates *lineBuf, which must be freed by the caller.
 * @param handle A pointer to a FileHandle struct.
 * @param lineBuf A pointer to a character arary which will be allocated and populated by this method.
 * @returns True if successful, otherwise false.
 */
CAPI b8 FilesystemReadLine(FileHandle* _handle, char** _lineBuf);

/**
 * Writes text to the provided file, appending a '\n' at the end.
 * @param handle A pointer to a FileHandle struct.
 * @param text The text to be written.
 * @returns True if successful, otherwise false.
 */
CAPI b8 FilesystemWriteLine(FileHandle* _handle, const char* _text);

/**
 * Reads up to dataSize bytes of data into outBytesRead.
 * Allocates &outData, which must be freed by the caller.
 * @param handle A pointer to a FileHandle struct.
 * @param dataSize The number of bytes to read.
 * @param outData A pointer to a block of memory to be populated by this method.
 * @param outBytesRead A pointer to a number which will be populated with the number of bytes actually read from the file.
 * @returns True if successful, otherwise false.
 */
CAPI b8 FilesystemRead(FileHandle* _handle, u64 _dataSize, void* _outData, u64* _outBytesRead);

/**
 * Reads all bytes of the file into outBytesRead.
 * Allocates outBytes, which must be freed by the caller.
 * @param handle A pointer to a FileHandle struct.
 * @param outBytes A pointer to a byte array which will be allocated and populated by this method.
 * @param outBytesRead A pointer to a number which will be populated with the number of bytes actually read from the file.
 * @returns True if successful, otherwise false.
 */
CAPI b8 FilesystemReadAllBytes(FileHandle* _handle, u8** _outBytes, u64* _outBytesRead);

/**
 * Writes provided data to the file.
 * @param handle A pointer to a FileHandle struct.
 * @param dataSize The size of the data in bytes.
 * @param data The to be written.
 * @param outBytesWritten A pointer to a number which will be populated with the number of bytes actually written to the file.
 * @returns True if successful, otherwise false.
 */
CAPI b8 FilesystemWrite(FileHandle* _handle, u64 _dataSize, const void* _data, u64* _outBytesWritten);