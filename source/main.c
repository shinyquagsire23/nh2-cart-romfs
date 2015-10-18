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
	sf2d_texture **mon;
	char *mon_garc;
	char *box_garc;
	if (rc)
	{
		printf("romfsInit: %08lX\n", rc);
		mon_garc = "sdmc:/1";
		box_garc = "sdmc:/3_box";
	}
	else
	{
		printf("romfs Init Successful!\n");
		mon_garc = "romfs:/a/0/9/3"; //Pokemon X/Y
		box_garc = "romfs:/a/1/0/4"; //Pokemon X/Y
		
		u32 num_mon = gnumrecords(mon_garc);
		if(!num_mon)
		{
		    mon_garc = "sdmc:/1";
		    num_mon = gnumrecords(mon_garc);
		}
		
		u32 box_test = gnumrecords(box_garc);
		if(!box_test)
		{
		    box_garc = "sdmc:/3_box";
		}
	}
	
	u32 num_mon = gnumrecords(mon_garc);
	mon = (sf2d_texture**)malloc(num_mon * sizeof(sf2d_texture*));
	printf("Loading sprites...");
	
	int i;
	for(i = 0; i < 32; i++)
	{
	    gfread(mon_garc, i, &decomp_out, &decomp_size); 	
	    sf2d_texture *mon_sprite = create_texture_from_xy7_clim(decomp_out, decomp_size);
	    mon[i] = mon_sprite;
	    free(decomp_out);
	}
	printf("Pokemon sprites read.\n");
	    
	gfread(box_garc, 0, &darc_decomp, &darc_decomp_size);
	printf("Box sprites read.\n");

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
	
	printf("All read!\n");
	sf2d_texture *cursor = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "cursol01.bclim");
	sf2d_texture *wallpaper = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "box_wp02.bclim");
	sf2d_texture *name_box = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "box_name02.bclim");
	
	sf2d_texture *mode01 = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "mode01.bclim");
	sf2d_texture *mode02 = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "mode02.bclim");
	sf2d_texture *mode03 = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "mode03.bclim");
	
	sf2d_texture *left_button = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "left_button.bclim");
	sf2d_texture *right_button = load_texture_from_darc(pokedarc_get_darc(darc_decomp), "right_button.bclim");
	
	float cursor_spot = 0;
	u8 cursor_direction = 1;
	float cursor_add = 0.4f;
	u8 cursor_mon_x = 3;
	u8 cursor_mon_y = 2;

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();
		
		if(cursor_direction)
		{
		    cursor_spot += cursor_add;
		    if(cursor_spot >= 5.0f)
		        cursor_direction--;
		}
		else
		{
		    cursor_spot -= cursor_add;
		    if(cursor_spot <= 0.0f)
		        cursor_direction++;
		}
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		    draw_texture(wallpaper, 0, 0);
		    
		    draw_texture(mode01, 25, 0);
		    draw_texture(mode02, 25+50+10, 0);
		    draw_texture(mode03, 25+50+10+50+10, 0);
		    
		    draw_texture(name_box, 20, 20);
		    draw_texture(left_button, 5, 20);
		    draw_texture(right_button, 20+name_box->width, 20);
		    
		    int x, y;
		    int i = 1;
		    for(y = 0; y < 5; y++)
		    {
		        for(x = 0; x < 6; x++)
		        {
		            draw_texture(mon[i], 10+(x*32), 45+(y*30));
		            i++;
		        }
		    }
			
			draw_texture(cursor, (int)(10-5+((cursor_mon_x + 1) * 32)-cursor_spot), (int)(45+5+((cursor_mon_y - 1) * 30)+cursor_spot));
		sf2d_end_frame();

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
