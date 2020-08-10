#ifndef LUTRIS_H
#define LUTRIS_H

enum keywords {
	MOVE = 0,
	MERGE,
	EXTRACT,
	COPY,
	CHMODX,
	EXECUTE,
	WRITE_FILE,
	WRITE_CONFIG,
	WRITE_JSON,
	INPUT_MENU,
	INSERT_DISC,
	TASK,

	KEYWORDMAX
};

static const char keywordstr[KEYWORDMAX][0xF] = {
	"move",
	"merge",
	"extract",
	"copy",
	"chmodx",
	"execute",
	"write_file",
	"write_config",
	"write_json",
	"input_menu",
	"insert-disc",
	"task",
};

enum taskKeywords {
	WINEEXEC = 0,
	WINETRICKS,
	CREATE_PREFIX,
	SET_REGEDIT,
	WINEKILL,

	TASKKEYWORDMAX
};

static const char taskKeywordstr[TASKKEYWORDMAX][0xF] = 
{
	"wineexec",
	"winetricks",
	"create_prefix",
	"set_regedit",
	"winekill"
};

int lutris(int, char**);
int lutris_install(int, char**);
int lutris_info(int, char**);
int lutris_help(int, char**);
void lutris_getInstallerURL(char*, char*);

#endif