#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "garc.h"

//#define DEBUG_GARC

int gnumrecords(const char* archive)
{
    FILE *handle = fopen(archive, "r");
    if(!handle)
        return handle;
        
    garc_header *garc_h = malloc(0x1C);
    fato_header *fato_h_init = malloc(0xC);
    
    fread(garc_h, 0x1C, 1, handle);
    fread(fato_h_init, 0xC, 1, handle);
    
    int return_val = fato_h_init->num_records;
    free(garc_h);
    free(fato_h_init);
    
    return return_val;
}

void *gfread(const char* archive, u32 index, void **out, u32 *size_out)
{
    FILE *handle = fopen(archive, "r");
    if(!handle)
        return handle;
        
    garc_header *garc_h = malloc(0x1C);
    fato_header *fato_h_init = malloc(0xC);
    
    fread(garc_h, 0x1C, 1, handle);
    fread(fato_h_init, 0xC, 1, handle);
    fseek(handle, -0xC, SEEK_CUR); //Go back to read the FATO header again after sizing the number of elements
#ifdef DEBUG_GARC
    printf("%.4s\n", garc_h->magic);
    printf("%.4s %x\n", fato_h_init->magic, fato_h_init->num_records);
#endif
    
    fato_header *fato_h = malloc(0xC + (fato_h_init->num_records * sizeof(u32)));
    fread(fato_h, 0xC + (fato_h_init->num_records * sizeof(u32)), 1, handle);
    free(fato_h_init);
#ifdef DEBUG_GARC
    printf("%.4s\n", fato_h->magic);
#endif
    
    fatb_header *fatb_h_init = malloc(0xC);
    fread(fatb_h_init, 0xC, 1, handle);
    fseek(handle, -0xC, SEEK_CUR); //Go back to read the FATB header again
    
    fatb_header *fatb_h = malloc(0xC + (fatb_h_init->file_count * sizeof(fatb_file)));
    fread(fatb_h, 0xC + (fatb_h_init->file_count * sizeof(fatb_file)), 1, handle);
    free(fatb_h_init);
    
    //All headers init'd, now we can start looking for our index
#ifdef DEBUG_GARC
    printf("%.4s\n", fatb_h->magic);
    
    printf("Seek %x, size %x\n", garc_h->data_offset + fatb_h->files[index].start_offset, fatb_h->files[index].size);
#endif
    fseek(handle, garc_h->data_offset + fatb_h->files[index].start_offset, SEEK_SET); //Go to our compressed file
    u8 *file_comp = malloc(fatb_h->files[index].size);
    fread(file_comp, fatb_h->files[index].size, 1, handle);
    
    u32 output_size;
    u8 *file_decomp;
    decompressLZ77content(file_comp, fatb_h->files[index].size, &file_decomp, &output_size);
    *size_out = output_size;
    
    free(file_comp);
    
    free(fatb_h);
    free(fato_h);
    free(garc_h);
    
    *out = file_decomp;
}
