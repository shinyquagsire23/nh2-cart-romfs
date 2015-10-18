#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char magic[4]; //CRAG
    u32 header_size;
    u32 bom;
    u32 chunk_count;
    u32 data_offset;
    u32 size;
    u32 last_size;
} garc_header;

typedef struct
{
    char magic[4]; //OTAF
    u32 size;
    u16 num_records;
    u16 padding;
    u32 offsets[0xFFFF];
} fato_header;

typedef struct
{
    u32 bit_vector;
    u32 start_offset;
    u32 end_offset;
    u32 size;
} fatb_file;

typedef struct
{
    char magic[4]; //BTAF
    u32 size;
    u32 file_count;
    fatb_file files[0xFFFF];
} fatb_header;

typedef struct
{
    char magic[4];
    u32 header_size;
    u32 data_size;
    u8 *data;
} fimb_entry;

void *gfread(const char* archive, u32 index, void **out, u32 *size_out);

#ifdef __cplusplus
}
#endif
