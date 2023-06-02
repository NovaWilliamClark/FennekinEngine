#pragma once
#include "platform/platform.hpp"

#ifdef PLATFORM_WINDOWS
#define FG_BLACK          0x0000 // Black
#define FG_BLUE           0x0001 // Blue
#define FG_GREEN          0x0002 // Green
#define FG_CYAN           0x0003 // Cyan
#define FG_RED            0x0004 // Red
#define FG_MAGENTA        0x0005 // Magenta
#define FG_YELLOW         0x0006 // Yellow
#define FG_WHITE          0x0007 // White
#define FG_INTENSITY      0x0008 // Bright

#define FG_BRIGHT_BLACK   (FG_BLACK | FG_INTENSITY)
#define FG_BRIGHT_BLUE    (FG_BLUE | FG_INTENSITY)
#define FG_BRIGHT_GREEN   (FG_GREEN | FG_INTENSITY)
#define FG_BRIGHT_CYAN    (FG_CYAN | FG_INTENSITY)
#define FG_BRIGHT_RED     (FG_RED | FG_INTENSITY)
#define FG_BRIGHT_MAGENTA (FG_MAGENTA | FG_INTENSITY)
#define FG_BRIGHT_YELLOW  (FG_YELLOW | FG_INTENSITY)
#define FG_BRIGHT_WHITE   (FG_WHITE | FG_INTENSITY)


#define BG_BLACK          0x0000 // Black
#define BG_BLUE           0x0010 // Blue
#define BG_GREEN          0x0020 // Green
#define BG_CYAN           0x0030 // Cyan
#define BG_RED            0x0040 // Red
#define BG_MAGENTA        0x0050 // Magenta
#define BG_YELLOW         0x0060 // Yellow
#define BG_WHITE          0x0070 // White
#define BG_INTENSITY      0x0080 // Bright
#define BG_BRIGHT_BLACK   (BG_BLACK | BG_INTENSITY)
#define BG_BRIGHT_BLUE    (BG_BLUE | BG_INTENSITY)
#define BG_BRIGHT_GREEN   (BG_GREEN | BG_INTENSITY)
#define BG_BRIGHT_CYAN    (BG_CYAN | BG_INTENSITY)
#define BG_BRIGHT_RED     (BG_RED | BG_INTENSITY)
#define BG_BRIGHT_MAGENTA (BG_MAGENTA | BG_INTENSITY)
#define BG_BRIGHT_YELLOW  (BG_YELLOW | BG_INTENSITY)
#define BG_BRIGHT_WHITE   (BG_WHITE | BG_INTENSITY)

#define DEFAULT           (FG_WHITE | BG_BLACK) // Default attributes

#else

// Foreground colors ANSI
#define FG_BLACK          "\033[30m"
#define FG_RED            "\033[31m"
#define FG_GREEN          "\033[32m"
#define FG_YELLOW         "\033[33m"
#define FG_BLUE           "\033[34m"
#define FG_MAGENTA        "\033[35m"
#define FG_CYAN           "\033[36m"
#define FG_WHITE          "\033[37m"
#define FG_ORANGE         "\033[38;5;208m"
#define FG_PINK           "\033[38;5;206m"
#define FG_PURPLE         "\033[38;5;141m"
#define FG_LIME           "\033[38;5;154m"
#define FG_TEAL           "\033[38;5;30m"
#define FG_OLIVE          "\033[38;5;58m"
#define FG_LAVENDER       "\033[38;5;183m"
#define FG_LIGHT_CYAN     "\033[38;5;51m"
#define FG_MAROON         "\033[38;5;52m"
#define FG_NAVY           "\033[38;5;17m"

#define FG_BRIGHT_BLACK   "\033[90m"
#define FG_BRIGHT_RED     "\033[91m"
#define FG_BRIGHT_GREEN   "\033[92m"
#define FG_BRIGHT_YELLOW  "\033[93m"
#define FG_BRIGHT_BLUE    "\033[94m"
#define FG_BRIGHT_MAGENTA "\033[95m"
#define FG_BRIGHT_CYAN    "\033[96m"
#define FG_BRIGHT_WHITE   "\033[97m"



// Background colors ANSI
#define BG_BLACK          "\033[40m"
#define BG_RED            "\033[41m"
#define BG_GREEN          "\033[42m"
#define BG_YELLOW         "\033[43m"
#define BG_BLUE           "\033[44m"
#define BG_MAGENTA        "\033[45m"
#define BG_CYAN           "\033[46m"
#define BG_WHITE          "\033[47m"
#define BG_ORANGE         "\033[48;5;208m"
#define BG_PINK           "\033[48;5;206m"
#define BG_PURPLE         "\033[48;5;141m"
#define BG_LIME           "\033[48;5;154m"
#define BG_TEAL           "\033[48;5;30m"
#define BG_OLIVE          "\033[48;5;58m"
#define BG_LAVENDER       "\033[48;5;183m"
#define BG_MAROON         "\033[48;5;52m"
#define BG_NAVY           "\033[48;5;17m"

#define BG_BRIGHT_BLACK   "\033[100m"
#define BG_BRIGHT_RED     "\033[101m"
#define BG_BRIGHT_GREEN   "\033[102m"
#define BG_BRIGHT_YELLOW  "\033[103m"
#define BG_BRIGHT_BLUE    "\033[104m"
#define BG_BRIGHT_MAGENTA "\033[105m"
#define BG_BRIGHT_CYAN    "\033[106m"
#define BG_BRIGHT_WHITE   "\033[107m"

#define FG_DEFAULT        "\033[39m"
#define BG_DEFAULT        "\033[49m"
#define DEFAULT           "\033[0m"
#endif
