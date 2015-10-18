#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <3ds.h>

#include "draw.h"
#include "clim.h"
#include "garc.h"
#include "darc.h"
#include <sf2d.h>

Handle romfsRealHandle;

// bypass handle list
Result _srvGetServiceHandle(Handle* out, const char* name)
{
	Result rc = 0;

	u32* cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = 0x50100;
	strcpy((char*) &cmdbuf[1], name);
	cmdbuf[3] = strlen(name);
	cmdbuf[4] = 0x0;
	
	if((rc = svcSendSyncRequest(*srvGetSessionHandle())))return rc;

	*out = cmdbuf[3];
	return cmdbuf[1];
}

void printfile(const char* path)
{
	FILE* f = fopen(path, "r");
	if (f)
	{
		char mystring[100];
		while (fgets(mystring, sizeof(mystring), f))
		{
			int a = strlen(mystring);
			if (mystring[a-1] == '\n')
			{
				mystring[a-1] = 0;
				if (mystring[a-2] == '\r')
					mystring[a-2] = 0;
			}
			puts(mystring);
			break;
		}
		printf(">>EOF<<\n");
		fclose(f);
	}
}

void *clim_tex_xy7(u8 *decomp_out, u32 decomp_size, u16 *out_data);
sf2d_texture *create_texture_from_clim(u8 *decomp_out, u32 decomp_size);
sf2d_texture *create_texture_from_xy7_clim(u8 *decomp_out, u32 decomp_size);
sf2d_texture *load_texture_from_darc(void *darc_data, char *path);

int main()
{
	//gfxInitDefault();
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));
	consoleInit(GFX_TOP, NULL);
	
	Result rc = _srvGetServiceHandle(&romfsRealHandle, "fs:USER");
	printf("realHandle: %08lX\n", rc);
	
	rc = FSUSER_Initialize(&romfsRealHandle);
	printf("initReal: %08lX\n", rc);

    // Regular RomFS
	u8 zeros[0xC];
	memset(zeros, 0, sizeof(zeros));

	FS_archive arch = { ARCH_ROMFS, { PATH_EMPTY, 1, (u8*)"" }, 0, 0 };
	FS_path path = { PATH_BINARY, sizeof(zeros), zeros };
	Handle romFS_file;

	rc = FSUSER_OpenFileDirectly(&romfsRealHandle, &romFS_file, arch, path, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
	printf("romfsOpen: %08lX\n", rc);
    
	rc = romfsInitFromFile(romFS_file, 0);
	u8 *decomp_out;
	u8 *darc_decomp;
	u32 decomp_size;
	u32 darc_decomp_size;
	if (rc)
	{
		printf("romfsInit: %08lX\n", rc);
		gfread("sdmc:/1", 7, &decomp_out, &decomp_size);
		gfread("sdmc:/3_box", 0, &darc_decomp, &darc_decomp_size);
	}
	else
	{
		printf("romfs Init Successful!\n");
		
		//Pokemon X/Y
		u32 ret = gfread("romfs:/a/0/9/3", 7, &decomp_out, &decomp_size); 	
		if(!ret)
		    gfread("sdmc:/1", 7, &decomp_out, &decomp_size);
		printf("Pokemon sprites read.\n");
		    
		ret = gfread("romfs:/a/1/0/4", 0, &darc_decomp, &darc_decomp_size); 
		if(!ret)
		    gfread("sdmc:/3_box", 0, &darc_decomp, &darc_decomp_size);
		printf("Box sprites read.\n");
	}

	if(decomp_out)
	    clim_print(decomp_out, decomp_size);
	u32 box_clim_size;
	u32 box_bg_size;
	u32 box_name_size;
	void *box_clim;
	void *box_bg_clim;
	void *box_name;
	    
	if(darc_decomp)
	{
	    printf("darc_file: %x\n", box_clim);
	}
	
	printf("All read!");
	sf2d_texture *cursor = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "cursol02.bclim");
	sf2d_texture *mon = create_texture_from_xy7_clim(decomp_out, decomp_size);
	sf2d_texture *wallpaper = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "box_wp02.bclim");
	sf2d_texture *name_box = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "box_name02.bclim");

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		    draw_texture(wallpaper, 0, 0);
		    draw_texture(name_box, 20, 20);
			draw_texture(mon, 100, 100);
			draw_texture(cursor, 120, 80);
		sf2d_end_frame();
		
		//if(decomp_out)
		    //clim_draw_xy7(decomp_out, decomp_size, 0, 0);
		//clim_draw_xy7(box_clim, box_clim_size, 32, 32);

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
			
		sf2d_swapbuffers();
	}

    sf2d_fini();
	romfsExit();
	gfxExit();
	return 0;
}

void draw_texture(const sf2d_texture *texture, int x, int y)
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
	for(i = 0; i < (next_pow2(clim_h->width) * next_pow2(clim_h->height)) / 2; i++)
	{
	    *(u16*)(tex->data + (i * sizeof(u16) * sizeof(u16))) = palette[(pixel_data[i] & 0xF0) >> 4];
	    *(u16*)(tex->data + (i * sizeof(u16) * sizeof(u16)) + sizeof(u16)) = palette[pixel_data[i] & 0xF];
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
