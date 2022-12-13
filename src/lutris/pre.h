// do some leg work ahead of time

// linux is already a macro
// but we want it as a keyword
#ifdef linux
#define _linux linux
#undef linux
#endif
