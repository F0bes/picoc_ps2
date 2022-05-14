#include "picoc_ps2.h"

printf("Hi there, running PICOC_VERSION %s.\n",PICOC_VERSION);

printf("This should be visible on both the console and the screen.\n");


toggle_prints(STDIO_OUT_SCREEN);

printf("This should only be visible on the screen.\n");

toggle_prints(STDIO_OUT_CONSOLE);

printf("This should only be visible in the console.\n");

toggle_prints(STDIO_OUT_CONSOLE | STDIO_OUT_SCREEN);

printf("This should be visible on both the console and screen again!\n");
