#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <3ds.h>

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

int main()
{
	gfxInitDefault();
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
	if (rc)
		printf("romfsInit: %08lX\n", rc);
	else
	{
		printf("romfs Init Successful!\n");
		printfile("a/0/0/0"); //Should print out CRAG
	}

	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
	}

	romfsExit();
	gfxExit();
	return 0;
}
