#ifndef UTIL_H
#define UTIL_H 1

#include <functional>
#include <algorithm>
#include <cctype>
#include <locale>

#define RESET         "\033[m"
#define BOLD          "\033[1m"
#define RED           "\033[31m"
#define GREEN         "\033[32m"
#define YELLOW        "\033[33m"
#define BLUE          "\033[34m"
#define MAGENTA       "\033[35m"
#define CYAN          "\033[36m"
#define BOLD_RED      "\033[1;31m"
#define BOLD_GREEN    "\033[1;32m"
#define BOLD_YELLOW   "\033[1;33m"
#define BOLD_BLUE     "\033[1;34m"
#define BOLD_MAGENTA  "\033[1;35m"
#define BOLD_CYAN     "\033[1;36m"
#define BG_RED        "\033[41m"
#define BG_GREEN      "\033[42m"
#define BG_YELLOW     "\033[43m"
#define BG_BLUE       "\033[44m"
#define BG_MAGENTA    "\033[45m"
#define BG_CYAN       "\033[46m"

// Left trim given string
static inline void ltrim( std::string &s ) {
    s.erase( s.begin(), std::find_if( s.begin(), s.end(), std::not1( std::ptr_fun<int, int>( std::isspace ) ) ) );
}

// Right trim given string
static inline void rtrim( std::string &s ) {
    s.erase( std::find_if( s.rbegin(), s.rend(), std::not1( std::ptr_fun<int, int>( std::isspace ) ) ).base(), s.end() );
}

// Left and right trim given string
static inline void trim( std::string &s ) {
    ltrim(s);
    rtrim(s);
}

// Return left-trimmed copy
static inline std::string ltrimmed( std::string s ) {
    ltrim(s);
    return s;
}

// Return right-trimmed copy
static inline std::string rtrimmed( std::string s ) {
    rtrim(s);
    return s;
}

// Return left- and right-trimmed copy
static inline std::string trimmed( std::string s ) {
    trim(s);
    return s;
}

bool errorOnDisallowedChars( const std::string & type, const std::string & invalidChars );

void PresentData(
        std::string & protocol,
        std::string & user,
        std::string & site,
        std::string & port,
        std::string & repo,
        std::string & rev,
        std::string & file
);

void PresentURL(
        std::string & protocol,
        std::string & user,
        std::string & site,
        std::string & port,
        std::string & repo,
        std::string & rev,
        std::string & file,
        bool extended = false,
        bool noansi = false
);

const char* single_to_narrow( wchar_t inchar );

const char* wide_to_narrow( wchar_t *input_buffer, int size );
#endif
