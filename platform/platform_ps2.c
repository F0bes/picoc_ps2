#include "../picoc.h"
#include "../interpreter.h"
#include "ps2lib/stdio_ps2.h"

#include <kernel.h>
#include <libkbd.h>
#include <loadfile.h>
#include <debug.h>

extern int STDIO_OUT_SCREEN;
extern int STDIO_OUT_CONSOLE;

extern int STDIO_OUTPUT;

/* mark where to end the program for platforms which require this */
jmp_buf PicocExitBuf;

void PlatformInit(Picoc* pc)
{
	// Initialize the keyboard
	// Currently relies on irx modules to be in the same directory as the elf
	printf("Loading usbd.irx TODO: ERROR HANDLING\n");
	SifLoadModule("host:usbd.irx", 0, NULL);
	printf("Loading ps2kbd.irx TODO: ERROR HANDLING\n");
	SifLoadModule("host:ps2kbd.irx", 0, NULL);

	printf("Initialising ps2kbd\n");
	PS2KbdInit();
	PS2KbdSetReadmode(PS2KBD_READMODE_NORMAL);
	PS2KbdSetBlockingMode(PS2KBD_BLOCKING);
}

void PlatformCleanup(Picoc* pc)
{
}

void ps2printf(const char* Format, ...)
{
	char buffer[1024];
	va_list ap;
	va_start(ap, Format);
	vsprintf(buffer, Format, ap);
	va_end(ap);
	if (STDIO_OUTPUT & 2)
	{
		printf(buffer);
	}
	if (STDIO_OUTPUT & 1)
	{
		if (buffer[0] == '\b') // Backspace
		{
			// Clear the previous char
			scr_setXY(scr_getX() - 1, scr_getY());
			scr_printf(" ");
			scr_setXY(scr_getX() - 1, scr_getY());
		}
		else
		{
			scr_printf(buffer);
		}
	}
}

void ps2putchar(char c)
{
	if (STDIO_OUTPUT & 2)
	{
		putchar(c);
	}
	if (STDIO_OUTPUT & 1)
	{
		if (c == '\b') // Backspace
		{
			// Clear the previous char
			scr_setXY(scr_getX() - 1, scr_getY());
			scr_printf(" ");
			scr_setXY(scr_getX() - 1, scr_getY());
		}
		else
		{
			scr_printf("%c", c);
		}
	}
}
/* get a line of interactive input */
char* PlatformGetLine(char* Buf, int MaxLen, const char* Prompt)
{

	if (Prompt != NULL)
		ps2printf("%s", Prompt);

	fflush(stdout);

	while (1)
	{
		int i;
		char c;

		for (i = 0; i < MaxLen - 1; i++)
		{
			PS2KbdRead(&c);
			if (c == -1)
				continue;

			if (c == '\n')
			{
				Buf[i] = '\0';
				ps2printf("\n");
				return Buf;
			}
			else if (c == 7) // Backspace (ps2kbd sends 7 for some reason)
			{
				if (i > 0)
				{
					ps2printf("\b");
					i -= 2;
				}
			}
			else
			{
				Buf[i] = c;
				ps2putchar(c);
				fflush(stdout);
			}
		}

		Buf[i] = '\0';
	}
}

/* get a character of interactive input */
int PlatformGetCharacter()
{
	fflush(stdout);
	return getchar();
}

/* write a character to the console */
void PlatformPutc(unsigned char OutCh, union OutputStreamInfo* Stream)
{
	putchar(OutCh);
}

/* read a file into memory */
char* PlatformReadFile(Picoc* pc, const char* FileName)
{
	struct stat FileInfo;
	char* ReadText;
	FILE* InFile;
	int BytesRead;
	char* p;

	if (stat(FileName, &FileInfo))
		ProgramFailNoParser(pc, "can't read file %s\n", FileName);

	ReadText = malloc(FileInfo.st_size + 1);
	if (ReadText == NULL)
		ProgramFailNoParser(pc, "out of memory\n");

	InFile = fopen(FileName, "r");
	if (InFile == NULL)
		ProgramFailNoParser(pc, "can't read file %s\n", FileName);

	BytesRead = fread(ReadText, 1, FileInfo.st_size, InFile);
	if (BytesRead == 0)
		ProgramFailNoParser(pc, "can't read file %s\n", FileName);

	ReadText[BytesRead] = '\0';
	fclose(InFile);

	if ((ReadText[0] == '#') && (ReadText[1] == '!'))
	{
		for (p = ReadText; (*p != '\r') && (*p != '\n'); ++p)
		{
			*p = ' ';
		}
	}

	return ReadText;
}

/* read and scan a file for definitions */
void PicocPlatformScanFile(Picoc* pc, const char* FileName)
{
	char* SourceStr = PlatformReadFile(pc, FileName);

	/* ignore "#!/path/to/picoc" .. by replacing the "#!" with "//" */
	if (SourceStr != NULL && SourceStr[0] == '#' && SourceStr[1] == '!')
	{
		SourceStr[0] = '/';
		SourceStr[1] = '/';
	}

	PicocParse(pc, FileName, SourceStr, strlen(SourceStr), TRUE, FALSE, TRUE, TRUE);
}

/* exit the program */
void PlatformExit(Picoc* pc, int RetVal)
{
	printf("PicoC sleeping\n");
	SleepThread();
}
