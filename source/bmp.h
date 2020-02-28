
// #pragma pack(push, 1) sets data alignment to 1, removes padding from structs
#pragma pack(push, 1)
struct bmp_header
{
    u8 signature[2]; // e.g. BM
    u32 byte_count;
    u32 reserved;
    u32 pixels_byte_offset;
};

struct bmp_dip_header
{
    u32 dip_header_byte_count;
    u32 width, height;
    u32 color_plane_count; // must be 1
    u16 bits_per_pixel;
    
    u32 compression_method;
    u32 image_byte_count;
    s32 width_pixels_per_meter;
    s32 hight_pixels_per_meter;
    u32 color_table_count;
    u32 important_color_count;
    
    u32 red_channel_bitmask;
    u32 green_channel_bitmask;
    u32 blue_channel_bitmask;
    u32 alpha_channel_bitmask;
    
    u32 color_space_type;
    u32 red_gamma;
    u32 blue_gamma;
    u32 green_gamma;
    
    u32 intent;
    u32 icc_profile_data;
    u32 icc_profile_size;
    u32 reserved;
};

#pragma pack(pop)

#if 0
enum
{
    BI_RGB = 0,
};
#endif

#define NextItem(iterator, type) ((type *)Advance(iterator, sizeof(type)).base)
#define PeekItem(iterator, type) ((type *)(iterator).base)

struct texture
{
    u8_array data;
    u32 width, height;
};

texture
LoadBMP(u8_array source)
{
    auto it = source;
    
    auto header = NextItem(&it, bmp_header);
    
    auto dip_header_byte_count = *PeekItem(it, u32);
    
    assert(dip_header_byte_count == 124);
    
    auto dip_header = NextItem(&it, bmp_dip_header);
    
    // no compression
    assert(dip_header->compression_method == BI_RGB);
    
    Advance(&it, dip_header->dip_header_byte_count - sizeof(bmp_dip_header));
    
    texture result;
    result.width = dip_header->width;
    result.height = dip_header->height;
    
    result.data = it;
    
    return result;
}