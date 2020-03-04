#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>

#include "win32_instafun.cpp"
#include "bmp.cpp"
#include "render.cpp"

s32 global_width = 500;
s32 global_height = 300;

LRESULT CALLBACK WindowProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param) {
	switch(message) {
		case WM_DESTROY: {
			PostQuitMessage(0);
		} break;
        
		case WM_SIZE: {
			RECT rectangle;
			GetClientRect(window_handle, &rectangle);  
			global_width = rectangle.right - rectangle.left;
			global_height = rectangle.bottom - rectangle.top;
		} break;
	}
    
	return DefWindowProc(window_handle, message, w_param, l_param);
}

struct State
{
    
};

void main() {
	WNDCLASSA window_class = {};
	window_class.hInstance = GetModuleHandle(null);
	window_class.hbrBackground = (HBRUSH) (COLOR_BACKGROUND);
	window_class.lpszClassName = "InstaName";
	window_class.style = CS_OWNDC;
	window_class.hCursor = LoadCursor(null, IDC_ARROW);
	window_class.lpfnWndProc = WindowProc;
    
	if (!RegisterClassA(&window_class)) {
		printf("RegisterClassA failed with error %d.\n", GetLastError());
	}	
    
	HWND window_handle = CreateWindowA(
        window_class.lpszClassName,
        "InstaFun",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        global_width,
        global_height,
        null,
        null,
        window_class.hInstance,
        0
        );
    
	HDC device_context = GetDC(window_handle);
    
	PIXELFORMATDESCRIPTOR pixel_format_descriptor = {};
	pixel_format_descriptor.nSize = sizeof(pixel_format_descriptor);
    pixel_format_descriptor.nVersion = 1;
    pixel_format_descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixel_format_descriptor.iPixelType = PFD_TYPE_RGBA;
    pixel_format_descriptor.cColorBits = 32;
	pixel_format_descriptor.cDepthBits = 24;
    pixel_format_descriptor.cStencilBits = 8;
    pixel_format_descriptor.iLayerType = PFD_MAIN_PLANE;
    
    //TODO: check for error
    s32 pixel_format = ChoosePixelFormat(device_context, &pixel_format_descriptor);
    SetPixelFormat(device_context, pixel_format, &pixel_format_descriptor);
    HGLRC gl_context = wglCreateContext(device_context);
    
    wglMakeCurrent(device_context, gl_context);
    
	ShowWindow(window_handle, SW_SHOW);
	
  auto sprites = LoadBMPTexture("data/sprites.bmp");
  auto background = LoadBMPTexture("data/stage.bmp");
    
	bool do_continue = true;
    
    while(do_continue) {
        MSG msg;
        
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {            
            switch (msg.message) {
                case WM_QUIT: {
                    do_continue = false;
                } break;
                
                default:{
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
    	glViewport(0, 0, global_width, global_height);
    	glClearColor(0, 0.5f, 0.5f, 1.0f);
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GEQUAL, 1/255.0f);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glBindTexture(GL_TEXTURE_2D, background.id);

      glBegin(GL_TRIANGLES);
        glTexCoord2f(0.01f, 0);
        glVertex2f(-1, -1);

        glTexCoord2f(1, 0);
        glVertex2f(1, -1);
        
        glTexCoord2f(1, 1);
        glVertex2f(1, 1);

        glTexCoord2f(0.01f, 1);
        glVertex2f(-1, 1);

        glTexCoord2f(0.01f, 0);
        glVertex2f(-1, -1);

        glTexCoord2f(1, 1);
        glVertex2f(1, 1);
      glEnd();

      glBindTexture(GL_TEXTURE_2D, sprites.id);

    	glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0);
      	glVertex2f(-0.5f, -0.5f);

        glTexCoord2f(1, 0);
      	glVertex2f(0.5f, -0.5f);
      	
        glTexCoord2f(1, 1);
        glVertex2f(0.5f, 0.5f);

        glTexCoord2f(0, 1);
        glVertex2f(-0.5f, 0.5f);

        glTexCoord2f(0, 0);
        glVertex2f(-0.5f, -0.5f);

        glTexCoord2f(1, 1);
        glVertex2f(0.5f, 0.5f);
    	glEnd();
        
    	SwapBuffers(device_context);
    }
    
    
    
	printf(":D\n");
}
