#include "../../interpreter.h"

#include <debug.h>
struct StdVararg
{
	struct Value** Param;
	int NumArgs;
};

extern int StdioBasePrintf(struct ParseState* Parser, FILE* Stream, char* StrOut, int StrOutLen, char* Format, struct StdVararg* Args);

static int STDIO_OUT_SCREEN =  1;
static int STDIO_OUT_CONSOLE = 2;

int STDIO_OUTPUT = 3;

void PS2_StdioSetup()
{
	scr_setCursor(0);
}

void PS2_TogglePrints(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
	STDIO_OUTPUT = Param[0]->Val->Integer;
	ReturnValue->Val->Integer = STDIO_OUTPUT;
}

void PS2_StdioPrintf(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
	struct StdVararg PrintfArgs;
	
	PrintfArgs.Param = Param;
	PrintfArgs.NumArgs = NumArgs - 1;
	if(STDIO_OUTPUT & STDIO_OUT_CONSOLE)
	{
		ReturnValue->Val->Integer = StdioBasePrintf(Parser, stdout, NULL, 0, Param[0]->Val->Pointer, &PrintfArgs);
	}
	if(STDIO_OUTPUT & STDIO_OUT_SCREEN)
	{
		char* strOut = malloc(255);
		StdioBasePrintf(Parser, NULL, strOut, 255, Param[0]->Val->Pointer, &PrintfArgs);
		scr_printf(strOut);
		free(strOut);
	}
}

struct LibraryFunction PS2StdioFunctions[] =
	{
		{PS2_StdioPrintf, "int printf(char *, ...);"},
		{PS2_TogglePrints, "int toggle_prints(int);"},
		{NULL, NULL}};


void PS2STDIOLibraryInit(Picoc* pc)
{
	printf("PS2STDIOLibraryInit\n");
	IncludeRegister(pc, "picoc_ps2.h", &PS2_StdioSetup, &PS2StdioFunctions[0], NULL);
	VariableDefinePlatformVar(pc, NULL, "STDIO_OUT_SCREEN", &pc->IntType, (union AnyValue*)&STDIO_OUT_SCREEN, FALSE);
	VariableDefinePlatformVar(pc, NULL, "STDIO_OUT_CONSOLE", &pc->IntType, (union AnyValue*)&STDIO_OUT_CONSOLE, FALSE);
}