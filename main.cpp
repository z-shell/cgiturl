#include "math_functions.h"
#include "coding_functions.h"
#include "util.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <locale>
#include <regex>
#include <cstdlib>
#include <cstring>

#include "optionparser.h"

struct Arg: public option::Arg
{
    static void printError(const char* msg1, const option::Option& opt, const char* msg2)
    {
        fprintf(stderr, "ERROR: %s", msg1);
        fwrite(opt.name, opt.namelen, 1, stderr);
        fprintf(stderr, "%s", msg2);
    }

    static option::ArgStatus Unknown(const option::Option& option, bool msg)
    {
        if (msg) printError("Unknown option '", option, "'\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus Required(const option::Option& option, bool msg)
    {
        if (option.arg != 0)
            return option::ARG_OK;

        if (msg) printError("Option '", option, "' requires an argument\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
    {
        if (option.arg != 0 && option.arg[0] != 0)
            return option::ARG_OK;

        if (msg) printError("Option '", option, "' requires a non-empty argument\n");
        return option::ARG_ILLEGAL;
    }
};


enum  optionIndex { UNKNOWN, HELP, QUIET, NOANSI, REV, PATH, DECODE };
const option::Descriptor usage[] =
{
 { UNKNOWN, 0, "", "", option::Arg::None, "USAGE: cgiturl <URL> [options]\n"
                                          "       cgiturl -d <GitURL|gcode>\n\n"
                                          "Options:" },
 { HELP,    0, "h", "help",     option::Arg::None, " -h, --help              Print help and exit" },
 { QUIET,   0, "q", "quiet",    option::Arg::None, " -q, --quiet             Less verbose output" },
 { NOANSI,  0, "n", "noansi",   option::Arg::None, " -n, --noansi            No color for gitu://..." },
 { REV,     0, "r", "revision", Arg::NonEmpty,     " -r <r>, --revision <r>  Which revision to encode in GitURL" },
 { PATH,    0, "p", "path",     Arg::NonEmpty,     " -p <p>, --path <p>      A path to encode in GitURL" },
 { DECODE,  0, "d", "decode",   Arg::NonEmpty,     " -d <c>, --decode <c>    GitURL or gcode to decode" },

 { UNKNOWN, 0, "", "",          option::Arg::None, "\nExamples:\n"
                               "  cgiturl https://github.com/zdharma/giturl -p lib/codes_huffman\n"
                               "  cgiturl -d ŬṽǚǫoŒẗ6ẏȅcЭÑẩőn4ầŘїệαЃȣϟṈӛŀї\n"
                               "  cgiturl -d gitu://ŬṽǚǫoŒẗ6ẏȅcЭÑẩőn4ầŘїệαЃȣϟṈӛŀї\n" },
 {0,0,0,0,0,0}
};

int main( int argc, char * argv[]) {
    std::locale initial_locale;
    std::string sys_locale_str;

    if ( std::getenv( "LANG" ) )
        sys_locale_str = std::string( std::getenv( "LANG" ) );

    if ( sys_locale_str.size() > 0 ) {
        std::locale::global ( std::locale ("") ); // use system environment to set locale
    } else {
        std::locale::global ( std::locale ("en_US.UTF-8") );
    }

    create_codes_map();
    create_rcodes_map();
    create_sites_maps();
    create_helper_maps();
    create_server_maps();
    create_protocol_maps();

    argc -= ( argc>0 ); argv += ( argc>0 ); // skip program name argv[0] if present
    option::Stats  stats( usage, argc, argv );
    std::vector<option::Option> options( stats.options_max + 1 );
    std::vector<option::Option> buffer( 128 );
    option::Parser parse( true, usage, argc, argv, &options[0], &buffer[0], 0, false, 127 );

    if ( parse.error() ) {
        fprintf( stderr, "Error when parsing options, aborting\n" );
        fflush( stderr );
        return 1;
    }

    if ( options[HELP] ) {
        option::printUsage(std::cout, usage);
        return 0;
    }

    if ( options[DECODE].count() == 0 ) {
        if ( parse.nonOptionsCount() == 0 ) {
            fprintf( stderr, "URL (like https://github.com/...) is needed as argument, aborting\n" );
            fflush( stderr );
            return 2;
        }

        std::string protocol;
        std::string user;
        std::string site;
        std::string port;
        std::string upath;
        std::string rev;
        std::string file;
        bool result;
        std::string arg( parse.nonOption(0) );
        std::wstring warg( std::strlen( parse.nonOption(0) ), L' ' );
        warg.resize( std::mbstowcs( &warg[0], parse.nonOption(0), std::strlen( parse.nonOption(0) ) ) );

        //
        // Correct matches
        //

        std::smatch m;
        std::regex r( "^(git|http|https|ftp|ftps)://([a-zA-Z0-9._~-]+)(:[0-9]+)?/([a-zA-Z0-9./_~:-]+)$" );
        result = regex_match( arg, m, r );
        if ( result ) {
            protocol=m[1]; site=m[2]; upath=m[4];
            if ( m[3].str().size() > 0 ) {
                port = m[3].str().substr( 1, std::string::npos );
            }
        }

        if ( ! result ) {
            r = std::regex( "^rsync://([a-zA-Z0-9._~-]+)/([a-zA-Z0-9./_~:-]+)$" );
            m = std::smatch();
            result = regex_match( arg, m, r );
            if ( result ) {
                protocol="rsync"; site=m[1]; upath=m[2];
            }
        }

        if ( ! result ) {
            r = std::regex( "^ssh://([a-zA-Z0-9._~-]+@)?([a-zA-Z0-9._~-]+)(:[0-9]+)?/([a-zA-Z0-9./_~:-]+)$" );
            m = std::smatch();
            result = regex_match( arg, m, r );
            if ( result ) {
                protocol="ssh"; site=m[2]; upath=m[4];
                if ( m[1].str().size() > 0 ) {
                    user = m[1].str().substr( 0, m[1].str().size() - 1 );
                }
                if ( m[3].str().size() > 0 ) {
                    port = m[3].str().substr( 1, std::string::npos );
                }
            }
        }

        if ( ! result ) {
            r = std::regex( "^([a-zA-Z0-9._~-]+@)?([a-zA-Z0-9._~-]+):([a-zA-Z0-9./_~:-]?[a-zA-Z0-9._~:-][a-zA-Z0-9./_~:-]*)$" );
            m = std::smatch();
            result = regex_match( arg, m, r );
            if ( result ) {
                protocol="ssh"; site=m[2]; upath=m[3];
                if ( m[1].str().size() > 0 ) {
                    user = m[1].str().substr( 0, m[1].str().size() - 1 );
                }
            }
        }

        //
        // Error matches
        //

        std::wsmatch wm;
        std::wregex wr( L"^(git|http|https|ftp|ftps)://([^/:]+)(:[^/]+)?/(.+)$" );

        if ( ! result ) {
            result = regex_match( warg, wm, wr );
            if ( result ) {
                bool result2 = true;
                if ( wm[3].str().size() > 0 ) {
                    std::wsmatch wm2;
                    std::wregex wr2( L"^:[0-9]+$" );
                    result2 = regex_match( wm[3].str(), wm2, wr2 );
                    if ( ! result2 ) {
                        std::wcout << L"Incorrect port number: " << wm[3].str().substr( 1, std::string::npos ) << std::endl;
                    }
                }

                if ( result2 ) {
                    std::cout << "Forbidden characters used in URL, allowed are [a-zA-Z0-9._~-]" << std::endl;
                    std::wsmatch wm2;
                    std::wregex wr2( L"[a-zA-Z0-9._~-]+" );
                    result2 = regex_match( wm[2].str(), wm2, wr2 );
                    if ( ! result2 ) {
                        std::wcout << L"Url part: " << wm[2] << std::endl;
                    } else {
                        std::wcout << L"Url part: " << wm[4] << std::endl;
                    }
                }

                return 3;
            }
        }

        if ( ! result ) {
            wm = std::wsmatch();
            wr = std::wregex( L"^ssh://([^@/]+@)?([^/:]+)(:[^/]+)?/(.*)$" );
            result = regex_match( warg, wm, wr );
            if ( result ) {
                bool result2 = true;

                if ( wm[1].str().size() > 0 ) {
                    std::wsmatch wm2;
                    std::wregex wr2( L"^[a-zA-Z0-9._~-]+@$" );
                    result2 = regex_match( wm[1].str(), wm2, wr2 );
                    if ( ! result2 ) {
                        std::cout << "Forbidden characters used in user name, allowed are [a-zA-Z0-9._~-]" << std::endl;
                        std::wcout << L"Obtained user name: " << wm[1].str().substr( 0, wm[1].str().size() - 1 ) << std::endl;
                    }
                }

                if ( result2 && wm[3].str().size() > 0 ) {
                    std::wsmatch wm2;
                    std::wregex wr2( L"^:[0-9]+$" );
                    result2 = regex_match( wm[3].str(), wm2, wr2 );
                    if ( ! result2 ) {
                        std::wcout << L"Incorrect port number: " << wm[3].str().substr( 1, std::string::npos ) << std::endl;
                    }
                }

                if ( result2 ) {
                    std::cout << "Forbidden characters used in URL, allowed are [a-zA-Z0-9._~-]" << std::endl;
                    std::wsmatch wm2;
                    std::wregex wr2( L"^[a-zA-Z0-9._~-]+$" );
                    result2 = regex_match( wm[2].str(), wm2, wr2 );
                    if ( ! result2 ) {
                        std::wcout << L"Url part: " << wm[2] << std::endl;
                    } else {
                        std::wcout << L"Url part: " << wm[4] << std::endl;
                    }
                }

                return 4;
            }
        }

        if ( ! result ) {
            wm = std::wsmatch();
            wr = std::wregex( L"^rsync://([^/]+)/(.*)$" );
            result = regex_match( warg, wm, wr );
            if ( result ) {
                std::cout << "Forbidden characters used in URL, allowed are [a-zA-Z0-9._~-]" << std::endl;
                std::wsmatch wm2;
                std::wregex wr2( L"^[a-zA-Z0-9._~-]+$" );
                bool result2 = regex_match( wm[1].str(), wm2, wr2 );
                if ( ! result2 ) {
                    std::wcout << L"Url part: " << wm[1] << std::endl;
                } else {
                    std::wcout << L"Url part: " << wm[2] << std::endl;
                }

                return 5;
            }
        }

        if ( ! result ) {
            std::wregex proto_r(L"^(rsync|git|http|https|ftp|ftps|ssh)://.*$");
            if ( !std::regex_search( warg, proto_r ) ) {
                wm = std::wsmatch();
                wr = std::wregex( L"^([^/:]+)://.*$" );
                result = regex_match( warg, wm, wr );
                if ( result ) {
                    std::wcout << L"Unsupported protocol: " << wm[1] << std::endl;
                    return 6;
                }
            }
        }

        if ( ! result ) {
            std::wregex proto_r(L"^(rsync|git|http|https|ftp|ftps|ssh)://.*$");
            if ( !std::regex_search( warg, proto_r ) ) {
                wm = std::wsmatch();
                wr = std::wregex( L"^([^@/]+@)?([^:]+):(.*.)$" );
                result = regex_match( warg, wm, wr );
                if ( result ) {
                    bool result2 = true;

                    if ( wm[1].str().size() > 0 ) {
                        std::wsmatch wm2;
                        std::wregex wr2( L"^[a-zA-Z0-9._~-]+@$" );
                        result2 = regex_match( wm[1].str(), wm2, wr2 );
                        if ( ! result2 ) {
                            std::cout << "Forbidden characters used in user name, allowed are [a-zA-Z0-9._~-]" << std::endl;
                            std::wcout << L"Obtained user name: " << wm[1].str().substr( 0, wm[1].str().size() - 1 ) << std::endl;
                        }
                    }

                    if ( result2 ) {
                        std::cout << "Forbidden characters used in URL, allowed are [a-zA-Z0-9._~-]" << std::endl;
                        std::wsmatch wm2;
                        std::wregex wr2( L"^[a-zA-Z0-9._~-]+$" );
                        result2 = regex_match( wm[2].str(), wm2, wr2 );
                        if ( ! result2 ) {
                            std::wcout << L"Url part: " << wm[2] << std::endl;
                        } else {
                            std::wcout << L"Url part: " << wm[3] << std::endl;
                        }
                    }

                    return 7;
                }
            }
        }

        if ( options[REV].count() > 0 ) {
            rev = std::string( options[REV].last()->arg );
        }
        if ( options[PATH].count() > 0 ) {
            file = std::string( options[PATH].last()->arg );
        }
        std::vector<int> selectors;

        if ( options[QUIET].count() == 0 ) {
            std::cout << std::endl;
            PresentData( protocol, user, site, port, upath, rev, file );
        }

        std::wstring gcode = build_gcode( protocol, user, site, port, upath, rev, file, selectors );

        if ( options[QUIET].count() == 0 )
            std::cout << std::endl;

        if ( options[NOANSI].count() == 0 ) {
            std::cout << MAGENTA << "gitu://" << wide_to_narrow( &gcode[0], gcode.size() ) << RESET << std::endl;
        } else {
            std::cout << "gitu://" << wide_to_narrow( &gcode[0], gcode.size() ) << std::endl;
        }
    } else {
        std::wstring gcode( std::strlen( options[DECODE].last()->arg ), L' ' );
        gcode.resize( std::mbstowcs( &gcode[0], options[DECODE].last()->arg, std::strlen( options[DECODE].last()->arg ) ) );

        std::wsmatch wm;
        std::wregex wr( L"^gitu://(.*)$" );

        bool result = regex_match( gcode, wm, wr );
        if ( result ) {
            gcode = std::wstring( wm[1] );
        } else {
            wm = std::wsmatch();
            wr = std::wregex( L"^//(.*)$" );
            result = regex_match( gcode, wm, wr );
            if ( result ) {
                gcode = std::wstring( wm[1] );
            }
        }

        std::vector<int> bits;
        int error;
        tie( bits, error ) = decode_gcode( gcode );
        if( error != 0 ) {
            std::cout << "Error during decoding the entered Zcode: " << error << std::endl;
        } else {
            int to_skip;
            std::map< std::string, std::string > decoded;
            tie( to_skip, decoded, error ) = process_meta_data( bits );

            if ( decoded[ "site" ].size() == 0 ) {
                decoded[ "site" ] = decoded[ "site_inline" ];
                if ( decoded[ "site" ].size() == 0 ) {
                    decoded[ "site" ] = "github.com";
                }
            } else {
                decoded[ "site" ] = getRServerSite()[ decoded[ "site" ] ];
            }

            if ( options[QUIET].count() > 0 ) {
                PresentURL( decoded["proto"], decoded["user"], decoded["site"], decoded["port"],
                            decoded["repo"], decoded["rev"], decoded["file"],
                            true, options[NOANSI].count() != 0 );
            } else {
                PresentData( decoded["proto"], decoded["user"], decoded["site"], decoded["port"],
                             decoded["repo"], decoded["rev"], decoded["file"] );
                std::cout << std::endl;
                PresentURL( decoded["proto"], decoded["user"], decoded["site"], decoded["port"],
                            decoded["repo"], decoded["rev"], decoded["file"],
                            false, options[NOANSI].count() != 0 );
            }
        }
    }
    return 0;
}
