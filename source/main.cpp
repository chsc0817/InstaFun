#include "win32_instafun.cpp"

void main() {
  win32_api api;
  Win32Init(&api); 

  //TODO: copy dll and load copy
  HMODULE dll_handle = LoadLibraryA("instafun.dll");

  auto init = (init_function) GetProcAddress(dll_handle, "Init");
  auto update = (update_function) GetProcAddress(dll_handle, "Update");
  
  auto init_data = init(&api);
  u64 last_write_time = 0;  

  while(Win32HandleMessage(&api)) {
    WIN32_FILE_ATTRIBUTE_DATA attributes;
    if (GetFileAttributesExA("instafun.dll", GetFileExInfoStandard, &attributes)){
      u64 write_time = (((u64) attributes.ftLastWriteTime.dwHighDateTime) << 32) | (attributes.ftLastWriteTime.dwLowDateTime);   
      if(write_time > last_write_time) {

      }
    }

    update(&api, init_data);

  }
    
    
    
	printf(":D\n");
}
