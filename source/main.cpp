#include "win32_instafun.cpp"

const cstring INSTAFUN_DLL_SOURCE = "instafun.dll";
const cstring INSTAFUN_DLL_COPY = "instafun_copy.dll";

bool CopyDll() {
  if (!CopyFile(INSTAFUN_DLL_SOURCE, INSTAFUN_DLL_COPY, false)) {
    printf("Could not copy %s. Error Code: %u", INSTAFUN_DLL_SOURCE, GetLastError());
    return false;
  }
  return true;
}

u64 getLastWriteTime(cstring file) {
  WIN32_FILE_ATTRIBUTE_DATA attributes;
  if (GetFileAttributesExA(file, GetFileExInfoStandard, &attributes)){
    return (((u64) attributes.ftLastWriteTime.dwHighDateTime) << 32) | (attributes.ftLastWriteTime.dwLowDateTime);
  }
  return 0;
}

//will not change parameters if LoadLibraryA fails
void LoadLib(HMODULE *dll_handle, init_function *init, update_function *update){
  HMODULE new_handle = LoadLibraryA(INSTAFUN_DLL_COPY);
  
  if (!new_handle) {
    printf("Could not load library. Error Code: %u", GetLastError());
    return;
  }

  *dll_handle = new_handle;
  *init = (init_function) GetProcAddress(new_handle, "Init");
  *update = (update_function) GetProcAddress(new_handle, "Update");
}

void main() {
  win32_api api;
  Win32Init(&api); 

  if(!CopyDll()) {
    return;
  }

  HMODULE dll_handle = {};
  init_function init = {};
  update_function update = {};
  LoadLib(&dll_handle, &init, &update);  
  
  auto init_data = init(&api);
  u64 last_write_time = getLastWriteTime(INSTAFUN_DLL_SOURCE);  

  while(Win32HandleMessage(&api)) {      

    if(getLastWriteTime(INSTAFUN_DLL_SOURCE) > last_write_time) {
      if (FreeLibrary(dll_handle)){
        CopyDll();
        LoadLib(&dll_handle, &init, &update);
        last_write_time = getLastWriteTime(INSTAFUN_DLL_SOURCE);
      } else {          
        printf("Could not free library. Error Code: %u", GetLastError());
      }
    }    

    update(&api, init_data);
  }   
    
	printf(":D\n");
}


