#ifndef DEFINES_H
#define DEFINES_H

#define ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])

#define GITHUB_API "https://api.github.com"
#define LUTRIS_API "https://lutris.net/api"
#define GITHUB_RELEASE(repo) GITHUB_API"/repos/" repo "/releases"

#define WINE_API LUTRIS_API "/runners/wine"
#define DXVK_API GITHUB_RELEASE("lutris/dxvk")
#define INSTALLER_API LUTRIS_API "/installers/"
#define LUTRIS_GAME_API LUTRIS_API "/games"
#define LUTRIS_GAME_SEARCH_API LUTRIS_GAME_API "?search=%s"
#define LUTRIS_GAME_INSTALLER_API LUTRIS_GAME_API "/%s/installers"


#ifndef NAME
#warning "no name specified, setting it to \"polecat\""
#define NAME "polecat"
#endif

#ifndef VERSION
#warning "no version specified, setting it to \"0.0.0\""
#define VERSION "0.0.0"
#endif

#define USER_AGENT NAME "/" VERSION

#define USAGE_STR "Usage: " NAME

#ifndef NDEBUG
#define UNREACHABLE printf("!!!UNREACHABLE CODE REACHED!!!\n" __FILE__ ":L%i\n", __LINE__); exit(0);
#else
#define UNREACHABLE
#endif

#ifdef __GNUC__
#define UNUSED __attribute__((__unused__))
#else
#define UNUSED
#endif

// since json-c 0.13 json_object_array_length returns a size_t
#if defined(JSON_C_MINOR_VERSION) && JSON_C_MINOR_VERSION >= 13
#define JSON_LENGTH_TYPE size_t
#else
#define JSON_LENGTH_TYPE int
#endif

#ifdef _WIN32
#define mkdir(path, perm) mkdir(path)
#endif

#endif
