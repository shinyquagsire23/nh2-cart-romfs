#include <sf2d.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef enum {
	CLIM_I8     =  0,
	CLIM_A8     =  1,
	CLIM_IA4    =  2,
	CLIM_IA8    =  3,
	CLIM_HILO8  =  4,
	CLIM_RGB565 =  5,
	CLIM_RGB8   =  6,
	CLIM_RGB5A1 =  7,
	CLIM_RGBA4  =  8,
	CLIM_RGBA8  =  9,
	CLIM_ETC1   = 10,
	CLIM_ETC1A4 = 11,
	CLIM_I4     = 12,
	CLIM_A4     = 13,
} clim_texfmt;

const u8 clim_to_sf2d[14] = { TEXFMT_I8, TEXFMT_A8, TEXFMT_IA4, TEXFMT_IA8, -1, TEXFMT_RGB565, TEXFMT_RGB8, TEXFMT_RGB5A1, TEXFMT_RGBA4, TEXFMT_RGBA8, TEXFMT_ETC1, TEXFMT_ETC1A4, TEXFMT_I4, TEXFMT_A4 };

const u8 sf2d_to_clim[14] = { CLIM_RGBA8, CLIM_RGB8, CLIM_RGB5A1, CLIM_RGB565, CLIM_RGBA4, CLIM_IA8, -1, CLIM_I8, CLIM_A8, CLIM_IA4, CLIM_I4, CLIM_A4, CLIM_ETC1, CLIM_ETC1A4 };

#ifdef __cplusplus
}
#endif
