CREATE_ENUM(unknown_runner)
CREATE_ENUM(wine)
#ifdef linux
#define _linux linux
#undef linux
#endif
CREATE_ENUM(linux)
#ifdef _linux
#define linux _linux
#undef _linux
#endif
