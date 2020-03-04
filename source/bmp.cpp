#include "bmp.h"
#include "win32_instafun.h"

image LoadBMP(u8_array source)
{
    auto it = source;
    
    auto header = NextItem(&it, bmp_header);
    
    auto dip_header_byte_count = *PeekItem(it, u32);
    
    assert(dip_header_byte_count == 124);
    
    auto dip_header = NextItem(&it, bmp_dip_header);
    
    // no compression
    assert(dip_header->compression_method == BMP_NO_COMPRESSION);
    
    Advance(&it, dip_header->dip_header_byte_count - sizeof(bmp_dip_header));
    
    image result;
    result.width = dip_header->width;
    result.height = dip_header->height;
    
    result.data = it;
    
    //bmp format is BGRA. convert to RGBA
    for(u32 i = 0; i < result.width * result.height; i++) {
        Swap(result.data[i * 4], result.data[i * 4 + 2]); 
    }

    return result;
}

texture LoadBMPTexture(cstring file_path) {
    auto result = ReadEntireFile(file_path);
    assert(result.ok);
    
    auto data = result.data;
    auto image = LoadBMP(data);
    auto texture = MakeTexture(image.width, image.height, image.data);
    
    delete [] data.base;

    return texture;

}