#ifndef WIN32_INSTAFUN_H 
#define WIN32_INSTAFUN_H

#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>

#include "basic.h"

struct win32_api;
struct win32_window;

#define INIT_SIGNATURE(name) u8 * name(win32_api *api)
#define UPDATE_SIGNATURE(name) void name(win32_api *api, u8 *init_data)

typedef INIT_SIGNATURE((*init_function));
typedef UPDATE_SIGNATURE((*update_function));


#define CREATE_WINDOW_SIGNATURE(name) void name(win32_window *window, win32_api *api, s32 width, s32 height, cstring title)
typedef CREATE_WINDOW_SIGNATURE((*create_window_function));

struct read_file_output
{
    u8_array data;
    bool ok;
};

#define READ_ENTIRE_FILE_SIGNATURE(name) read_file_output name(cstring file_path)
typedef READ_ENTIRE_FILE_SIGNATURE((*read_entire_file_function));

#define FREE_FILE_DATA_SIGNATURE(name) void name(u8_array data)
typedef FREE_FILE_DATA_SIGNATURE((*free_file_data_function));

#define DISPLAY_WINDOW_SIGNATURE(name) void name(win32_window *window)
typedef DISPLAY_WINDOW_SIGNATURE((*display_window_function));

struct code_info
{
    init_function   init;
    update_function update;
    u8 *init_data;
    HMODULE dll_handle;
    u64 last_write_time;
};

struct win32_api {
    create_window_function  create_window;
    display_window_function display_window;
    read_entire_file_function read_entire_file;
    free_file_data_function   free_file_data;
    
    WNDCLASSA window_class;
    HGLRC gl_context;
    code_info application;
};

struct win32_window {
    HDC device_context;
    HWND handle;
    s32 width, height;
};

void Win32Init(win32_api *api);
bool Win32HandleMessage(win32_api *api);

#endif