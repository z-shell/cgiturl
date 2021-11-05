#include "util.h"
#include "coding_functions.h"
#include <string>
#include <iostream>

bool errorOnDisallowedChars( const std::string & type, const std::string & invalidChars )
{
    if( !invalidChars.empty() ) {
        std::string name = getNames()[ type ];
        if( name.size() > 0 ) {
            std::cout << "Invalid characters in `" << name << "' section: " << invalidChars << ", they are skipped" << std::endl;
        } else {
            std::cout << "Invalid characters used: " << invalidChars << ", they are skipped" << std::endl;
        }
        return true;
    }
    return false;
}

void PresentData(
        std::string & protocol,
        std::string & user,
        std::string & site,
        std::string & port,
        std::string & repo,
        std::string & rev,
        std::string & file
) {
    std::cout << GREEN "Protocol:" RESET "  " << protocol << std::endl;

    if ( user.size() > 0 )
        std::cout << GREEN "User:" RESET "      " << user << std::endl;

    std::cout << GREEN "Site:" RESET "      " << site << std::endl;

    if ( port.size() > 0 )
        std::cout << GREEN "Port:" RESET "      " << port << std::endl;

    std::cout << YELLOW "Repo:" RESET "      " << repo << std::endl;

    if ( rev.size() > 0 )
        std::cout << YELLOW "Revision:" RESET "  " << rev << std::endl;

    if ( file.size() > 0 )
        std::cout << RED "File:" RESET "      " << file << std::endl;
}

void PresentURL(
        std::string & protocol,
        std::string & user,
        std::string & site,
        std::string & port,
        std::string & repo,
        std::string & rev,
        std::string & file,
        bool extended,
        bool noansi
) {
    std::string out = protocol;
    out += "://" + site;
    if ( port.size() > 0 )
        out += ":" + port;
    out += "/" + repo;

    if ( extended ) {
        if ( noansi == false )
            std::cout << MAGENTA;
        std::cout << out;
        if ( rev.size() > 0 )
            std::cout << " /  rev:" << rev;
        if ( file.size() > 0 )
            std::cout << " /  file:" << file;
        if ( noansi == false )
            std::cout << RESET;
        std::cout << std::endl;
    } else {
        if ( noansi == false ) {
            std::cout << MAGENTA << out << RESET << std::endl;
        } else {
            std::cout << out << std::endl;
        }
    }
}

const char* single_to_narrow( wchar_t inchar ) {
    wchar_t input_buffer[ 2 ] = { inchar, L'\0' };
    static char output_buffer[ 8 ];
    if( (size_t)-1 == wcstombs ( output_buffer, input_buffer, sizeof(output_buffer) ) ) {
        output_buffer[0] = '\0';
    }

    return output_buffer;
}

const char* wide_to_narrow( wchar_t *input_buffer, int size ) {
    static char output_buffer[ 65536 ];

    wchar_t newbuf[size+1];
    std::wcsncpy( newbuf, input_buffer, size );
    newbuf[ size ] = L'\0';

    if( (size_t)-1 == wcstombs ( output_buffer, newbuf, 65536 ) ) {
        output_buffer[0] = '\0';
    }

    output_buffer[65535] = '\0';

    return output_buffer;
}
