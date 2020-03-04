#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>

#include "win32_instafun.h"


#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "opengl32.lib")

read_file_output ReadEntireFile(cstring file_path) {
    HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null);
    
    assert(file_handle);
    
    WIN32_FILE_ATTRIBUTE_DATA attribute_data;
    if (!GetFileAttributesEx(file_path, GetFileExInfoStandard, &attribute_data))
        return {};
    
    u64 byte_count = (((u64)attribute_data.nFileSizeHigh) << 32) | attribute_data.nFileSizeLow;
    
    u8_array data;
    data.count = byte_count;
    data.base = new u8[data.count];
    
    DWORD bytes_read_count;
    if (!ReadFile(file_handle, data.base, data.count, &bytes_read_count, null))
        return {};
    
    return { data, true };
}