#ifndef INSTAFUN_H 
#define INSTAFUN_H

#include "basic.h"



struct read_file_output
{
    u8_array data;
    bool ok;
};

read_file_output ReadEntireFile(cstring file_path);


#endif