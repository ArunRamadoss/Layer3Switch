#include "cli.h"

#define MAX_CMD_NAME        64
#define MAX_HELP_STRING     64
#define MAX_PMP_LEN   64
#define MAX_USERS         8
#define MAX_USER_NAME       32
#define MAX_USER_PASSWORD   32


#define IS_HELP_CHAR(c)   (c == '?')
#define IS_BACK_SPACE(c)  (c == 0x7f)
#define IS_TAB(c)         (c == '\t')
#define PAGE_UP     73
#define HOME        71
#define END         79
#define PAGE_DOWN   81
#define UP_ARROW    72
#define LEFT_ARROW  75
#define DOWN_ARROW  80
#define RIGHT_ARROW 77
#define F1          59
#define F2          60
#define F3          61
#define F4          62
#define F5          63
#define F6          64
#define F7          65
#define F8          66
#define F9          67
#define F10         68


