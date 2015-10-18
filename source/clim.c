#include <3ds.h>
#include <sf2d.h>

#include "clim.h"
#include "garc.h"
#include "darc.h"

const u8 clim_to_sf2d[14] = { TEXFMT_I8, TEXFMT_A8, TEXFMT_IA4, TEXFMT_IA8, -1, TEXFMT_RGB565, TEXFMT_RGB8, TEXFMT_RGB5A1, TEXFMT_RGBA4, TEXFMT_RGBA8, TEXFMT_ETC1, TEXFMT_ETC1A4, TEXFMT_I4, TEXFMT_A4 };
const u8 sf2d_to_clim[14] = { CLIM_RGBA8, CLIM_RGB8, CLIM_RGB5A1, CLIM_RGB565, CLIM_RGBA4, CLIM_IA8, -1, CLIM_I8, CLIM_A8, CLIM_IA4, CLIM_I4, CLIM_A4, CLIM_ETC1, CLIM_ETC1A4 };

void clim_draw_texture(const sf2d_texture *texture, int x, int y)
{
    sf2d_draw_texture_part(texture, x, y, 0, next_pow2(texture->height) - texture->height, texture->width, texture->height);
}

sf2d_texture *load_texture_from_darc(void *darc_data, char *path)
{
    u32 size;
    void *data;
    data = dfget(darc_data, path, &size);
    return create_texture_from_clim(data, size);
}

sf2d_texture *create_texture_from_clim(u8 *decomp_out, u32 decomp_size)
{
    clim_header *clim_h = decomp_out + (decomp_size - 0x28);
	sf2d_texture *tex = sf2d_create_texture(clim_h->width, clim_h->height, clim_to_sf2d[clim_h->format], SF2D_PLACE_RAM);
	memcpy(tex->data, decomp_out, clim_h->pixel_data_size);
	
	tex->flip_h = 0;
	tex->flip_v = 1;
	return tex;
}

sf2d_texture *create_texture_from_xy7_clim(u8 *decomp_out, u32 decomp_size)
{
    clim_header *clim_h = decomp_out + (decomp_size - 0x28);
	sf2d_texture *tex = sf2d_create_texture(clim_h->width, clim_h->height, TEXFMT_RGB5A1, SF2D_PLACE_RAM);
	
	u16 *palette = decomp_out+(sizeof(u16)*2);
	u16 num_colors = *(u16*)(decomp_out + sizeof(u16));
	u8 *pixel_data = decomp_out + (sizeof(u16)*2) + (sizeof(u16) * num_colors);
	
	//Un-palette the data
	int i;
	for(i = 0; i < (next_pow2(clim_h->width) * next_pow2(clim_h->height)) / (num_colors > 0x10 ? 1 : 2); i++)
	{
	    if(num_colors > 0x10)
	    {
	        *(u16*)(tex->data + (i * sizeof(u16))) = palette[pixel_data[i]];
	    }
	    else
	    {
	        *(u16*)(tex->data + (i * sizeof(u16) * sizeof(u16))) = palette[(pixel_data[i] & 0xF0) >> 4];
	        *(u16*)(tex->data + (i * sizeof(u16) * sizeof(u16)) + sizeof(u16)) = palette[pixel_data[i] & 0xF];
	    }
	}
	
	tex->flip_h = 0;
	tex->flip_v = 1;
	return tex;
}

void clim_print(u8 *decomp_out, u32 decomp_size)
{
    clim_header *clim_h = decomp_out + (decomp_size - 0x28);
		
	printf("gfread: %x %.5s\n", decomp_size, clim_h->magic);
	u16 num_colors = *(u16*)(decomp_out + sizeof(u16));
	u32 squared_width = (clim_h->pixel_data_size / 2) >> 5; //sqrt(pixel_data_size / bpp)
	
	printf("%x colors, %u x %u, data %u x %u\n", num_colors, clim_h->width, clim_h->height, squared_width, squared_width);
}
