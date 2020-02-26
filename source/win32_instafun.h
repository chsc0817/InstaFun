#include <stdio.h>
#include <windows.h>
#include <stdint.h>
#include <gl/gl.h>

#include <assert.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "opengl32.lib")

#define null NULL

#define s8  int8_t
#define s32 int32_t

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

// maximal addressable size (32bit or 64bit), unsigned memory (umm)
#define umm size_t 

// is 0 terminated c string
typedef char * cstring;

// c string "hello" => { 'h', 'e', 'l', 'l', 'o', 0 }

struct u8_array
{
    u8 *base;
    umm count;
};

u8_array Advance(u8_array *iterator, umm count = 1)
{
    assert(iterator->count >= count);
    
    u8_array skipped_data;
    skipped_data.base = iterator->base;
    skipped_data.count = count;
    
    iterator->base  += count;
    iterator->count -= count;
    
    return skipped_data;
}

struct read_file_output
{
    u8_array data;
    bool ok;
};

read_file_output 
ReadEntireFile(cstring file_path)
{
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