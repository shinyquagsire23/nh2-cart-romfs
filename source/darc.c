#include <3ds.h>

#include "darc.h"

char *wide_to_char(u16 *data);

void *pokedarc_get_darc(void *pokedarc_data)
{
    darc_init_header *darc_init_h = pokedarc_data;
	darc_init_wat *darc_init_w = pokedarc_data + sizeof(u32) + (darc_init_h->entries * 0x40);
	printf("%x wats\n", darc_init_w->entries);
	
	darc_header *darc_h = pokedarc_data + ((sizeof(u32) + (darc_init_h->entries * 0x40) + sizeof(u32) + (darc_init_w->entries * 0x20) + 0x80) & 0xFFFFFF80); //Go past darc_init_wat to the nearest 0x80
	printf("%.5s\n", &darc_h->magic);
	return darc_h;
}

void *dfget(void *darc_data, char *path, u32 *out_size)
{
    darc_header *darc_h = darc_data;
    darc_file_table *darc_file_t = (u32)darc_h + (u32)darc_h->file_table_offset;
	void *darc_name_table = (u32)darc_file_t + (u32)(darc_file_t->files[0].length * 0xC);
	
	int i;
	void *out_ptr;
	for(i = 0; i < darc_file_t->files[0].length; i++)
	{
	    char *dfile = wide_to_char(darc_name_table + darc_file_t->files[i].name_offset);
	    if(strcmp(dfile, path) == 0)
	    {
	        out_ptr = (u32)darc_h + (u32)darc_file_t->files[i].offset;
	        if(out_size != NULL)
	            *out_size = darc_file_t->files[i].length;
	        free(dfile);
	        break;
	    }
	    free(dfile);
	}
	    
	return out_ptr;
}

char *wide_to_char(u16 *data)
{
    char *out = malloc(0x200);
    int i;
    for(i = 0;;i++)
    {
        out[i] = (char)(data[i] & 0xFF);
        if(out[i] == 0)
            break;
    }
    return out;
}
