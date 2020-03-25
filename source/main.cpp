#include "win32_platform.cpp"

u64 GetLastWriteTime(cstring file) {
    WIN32_FILE_ATTRIBUTE_DATA attributes;
    if (GetFileAttributesExA(file, GetFileExInfoStandard, &attributes)){
        return (((u64) attributes.ftLastWriteTime.dwHighDateTime) << 32) | (attributes.ftLastWriteTime.dwLowDateTime);
    }
    return 0;
}

#if defined LIVE_CODE_RELOADING

#include "input.cpp"

bool LoadCode(code_info *code) {
    bool do_reload = false;
    
    u64 last_write_time = GetLastWriteTime("build/instafun.dll");
    if(last_write_time > code->last_write_time) {
        
        WIN32_FILE_ATTRIBUTE_DATA attributes;
        if (GetFileAttributesExA("build/compile.lock", GetFileExInfoStandard, &attributes))
            return false;
        
        do_reload = true;
        
        if (code->dll_handle)
            FreeLibrary(code->dll_handle);
        
        code->dll_handle = LoadLibraryA("instafun.dll");
        assert(code->dll_handle);
        code->init   =   (init_function)GetProcAddress(code->dll_handle, "Init");
        code->update = (update_function)GetProcAddress(code->dll_handle, "Update");
        
        if (!code->init || !code->update)
            do_reload = false;
        
        FreeLibrary(code->dll_handle);
        
        if (do_reload) {
            bool ok = CopyFile("build/instafun.dll", "build/instafun_live.dll", false);
            assert(ok);
        }
        
        code->dll_handle = LoadLibraryA("instafun_live.dll");
        assert(code->dll_handle);
        
        code->init   =   (init_function)GetProcAddress(code->dll_handle, "Init");
        code->update = (update_function)GetProcAddress(code->dll_handle, "Update");
        code->last_write_time = last_write_time;
    }
    
    return do_reload;
}

void InitCode(code_info *code) {
    bool ok = LoadCode(code);
    assert(ok);
}

#else

#include "instafun.cpp"

#define LoadCode(...) 

void InitCode(code_info *code) {
    code->init = Init;
    code->update = Update;
}

#endif

void main() {
    win32_api api;
    Win32Init(&api); 
    
    InitCode(&api.application);
    
    api.application.init_data = api.application.init(&api);
    
    while(Win32HandleMessage(&api)) {
        LoadCode(&api.application);
        
        api.application.update(&api, api.application.init_data);
    }   
    
	printf(":D\n");
}


