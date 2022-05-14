#include "../interpreter.h"

#include <tamtypes.h>
#include <debug.h>
 
void PS2VsyncWait()
{
	volatile u64* GS_REG_CSR = (u64*)0x12001000;
	*GS_REG_CSR |= *GS_REG_CSR & 8;
	while (!(*GS_REG_CSR & 8));
}

void PS2SetupFunc()
{
	*(u32*)0x10003000 = 1; // RESET GIF
	init_scr();
	PS2VsyncWait();
}

void Ctest(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
	printf("test(%d)\n", Param[0]->Val->Integer);
	Param[0]->Val->Integer = 1234;
}

void Clineno(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
	ReturnValue->Val->Integer = Parser->Line;
}


/* list of all library functions and their prototypes */
struct LibraryFunction PS2Functions[] =
	{
		{Ctest, "void test(int);"},
		{Clineno, "int lineno();"},
		{NULL, NULL}};


void PlatformLibraryInit(Picoc* pc)
{
	IncludeRegister(pc, "picoc_unix.h", &PS2SetupFunc, &PS2Functions[0], NULL);
	PS2STDIOLibraryInit(pc);
}
