#pragma pack(push, 1)
typedef struct
{
    char magic[4];
    u16 bom;
    u32 header_size;
    u8 tile_width;
    u8 tile_height;
    u32 clim_size;
    u32 layers;
    u32 magic_imag;
    u32 imag_header_size;
    u16 width;
    u16 height;
    u32 format;
    u32 pixel_data_size;
} clim_header;
#pragma pack(pop)
