#ifndef WIN32_INSTAFUN_H 
#define WIN32_INSTAFUN_H

#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>

#include "basic.h"

struct read_file_output
{
    u8_array data;
    bool ok;
};

struct win32_api {
    WNDCLASSA window_class;
    HGLRC gl_context;     
};

struct win32_window {
  HDC device_context;
  HWND handle;
  s32 width, height;
};

#define INIT_SIGNATURE(name) u8 * name(win32_api *api)
#define UPDATE_SIGNATURE(name) void name(win32_api *api, u8 *init_data)

typedef INIT_SIGNATURE((*init_function));
typedef UPDATE_SIGNATURE((*update_function));


void Win32Init(win32_api *api);
void Win32CreateWindow(win32_window *window, win32_api *api, s32 width, s32 height, cstring title);
bool Win32HandleMessage(win32_api *api);
read_file_output ReadEntireFile(cstring file_path);



#endif