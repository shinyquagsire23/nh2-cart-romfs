#pragma pack(push, 1)
typedef struct
{
    u32 entries;
    char names[0xFFFF][0x40];
} darc_init_header;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    u32 entries;
    u8 wats[0x20];
} darc_init_wat;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    u32 magic;
    u16 bom;
    u16 header_size;
    u32 version;
    u32 file_length;
    u32 file_table_offset;
    u32 file_table_length;
    u32 file_data_offset;
} darc_header;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    u16 name_offset;
    u8 parent;
    u8 is_folder;
    u32 offset;
    u32 length;
} darc_file;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    darc_file files[0xFFFF];
} darc_file_table;
#pragma pack(pop)

void *pokedarc_get_darc(void *pokedarc_data);
void *dfget(void *darc_data, char *path, u32 *out_size);
