// Dont think we really need the wrapper ifndef bc we don't have any files that
// would include this twice, nor do we include this (A) in B and C and then
// also include B in C resulting in double-def (if no wrapper)
// I assume the preprocessor works on .c each file independently, so including
// this file in both i2c.c and main.c wont result in double-def bc CPP doesnt
// have memory b/w input files (wrt to macros)
#define _XTAL_FREQ 4000000
