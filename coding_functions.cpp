/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "coding_functions.h"
#include "math_functions.h"
#include "util.h"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <list>
#include <regex>
#include <bitset>

const char input_characters[67]={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                      'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                      '.', '_', '~', '-', '\0' };

const char *incharacters_begin = &input_characters[0];
const char *incharacters_end = &input_characters[66];

std::map<std::string, std::string> codes;
std::map<std::string, std::string> rcodes;
std::map<std::string, std::string> sites_flags;
std::map<std::string, std::string> rsites_flags;
std::map<std::string, std::string> names;
std::map<std::string, std::string> server_site;
std::map<std::string, std::string> rserver_site;
std::map<std::string, std::string> proto_code;
std::map<std::string, std::string> rproto_code;

std::tuple< int, std::map< std::string, std::string >, int >
process_meta_data( const std::vector<int> & _bits ) {
    int size = _bits.size();
    std::vector<int> bits = _bits;

    // Pop two version bits
    int ver2 = bits.back();
    bits.pop_back();
    int ver1 = bits.back();
    bits.pop_back();

    std::reverse( bits.begin(), bits.end() );

    std::stringstream ss;
    std::copy( bits.begin(), bits.end(), std::ostream_iterator<int>(ss,""));
    std::string strbits = ss.str().c_str();

    std::regex bits_regex("^[01]+$");
    if ( !std::regex_search( strbits, bits_regex ) ) {
        return std::make_tuple( 0, std::map<std::string, std::string>(), 120 );
    }

    // We will use this value to compute how much bits were skipped
    int init_len = strbits.size();

    int REPLY, inlne=0, error = 0;
    std::map< std::string, std::string > decoded;
    decoded["file"]         = "";
    decoded["rev"]          = "";
    decoded["repo"]         = "";
    decoded["wordrev"]      = "";
    decoded["proto"]        = "";
    decoded["user"]         = "";
    decoded["site"]         = "";
    decoded["site_inline"]  = "";
    decoded["flags"]        = "";
    decoded["port"]         = "";
    decoded["unused1"]      = "";
    decoded["unused2"]      = "";
    decoded["unused3"]      = "";
    decoded["unused4"]      = "";
    decoded["error"]        = "";

    std::string current_selector = "error";
    bool first = true;
    int trylen;
    std::string mat, trystr;
    while ( 1 ) {
        if ( first ) {
            mat = "site_flags";
            first = false;
        } else {
            mat="";
            for ( trylen = 6; trylen <= 7; trylen ++ ) {
                // Take substring of len $trylen and check if
                // it matches any Huffman code
                trystr = strbits.substr( 0, trylen );
                if( rcodes.count( trystr ) ) {
                    mat = rcodes[ trystr ];
                    break;
                }
            }

            if( mat == "" ) {
                return std::make_tuple( 0, decoded, 121 );
            }

            // Skip decoded bits
            strbits = strbits.substr( trylen, std::string::npos );
        }

        // Handle what has been matched, either selector or data
        if ( mat == "ss" ) {
            break;
        } else if ( mat == "repo_rev_usr" ) {
            if ( current_selector == "repo" ) {
                current_selector = "rev";
            } else if ( current_selector == "rev" ) {
                current_selector = "user";
            } else {
                current_selector = "repo";
            }
        } else if ( mat == "site_flags" ) {
            if ( current_selector == "site_flags" ) {
                inlne = 1;
            } else {
                inlne = 0;
                current_selector = "site_flags";

                // Short path for port number - conditional constant 16 bits
                if ( strbits[0] == '1' ) {
                    std::bitset<16> bin_port( strbits.substr( 1, 16 ) );
                    decoded["port"] = std::to_string( bin_port.to_ulong() );
                    strbits = strbits.substr( 17, std::string::npos );
                } else {
                    strbits = strbits.substr( 1, std::string::npos );
                }

                // Short path for protocol - always 3 bits
                std::string proto_bits = strbits.substr( 0, 3 );
                decoded[ "proto" ] = rproto_code[ proto_bits ];
                strbits = strbits.substr( 3, std::string::npos );
            }
        } else if ( decoded.count( mat ) ) {
            inlne = 0;
            current_selector = mat;
        } else {
            if ( current_selector == "site_flags" ) {
                if ( inlne ) {
                    decoded[ "site_inline" ].append( mat );
                } else {
                    std::ptrdiff_t pos1 = std::find( incharacters_begin, incharacters_end, mat.c_str()[0] ) - incharacters_begin;
                    std::ptrdiff_t pos2 = std::find( incharacters_begin, incharacters_end, '3') - incharacters_begin;
                    if ( pos1 > pos2 ) {
                        mat = rsites_flags[ mat ];
                        if ( decoded[ "flags" ].size() > 0 )
                            decoded[ "flags" ].append( "," );
                        decoded[ "flags" ].append( mat );
                    } else {
                        mat = rsites_flags[ mat ];
                        decoded[ "site" ] = mat;
                    }
                }
            } else {
                decoded[ current_selector ].append( mat );
            }
        }
    }

    REPLY = init_len - strbits.size();

    return std::make_tuple( REPLY, decoded, error );
}

int insertBitsFromStrBits( std::vector<int> & dest, const std::string & str ) {
    int error = 0;
    bool was = false;
    for ( std::string::const_iterator it = str.begin() ; it < str.end(); it++ ) {
        was = true;
        if( *it == '1' ) {
            dest.push_back( 1 );
        } else if( *it == '0' ) {
            dest.push_back( 0 );
        } else {
            error += 157;
            dest.push_back( 1 );
        }
    }

    if( !was ) {
        error += 257;
    }
    return error;
}

int BitsStart( std::vector<int> & dest ) {
    int error = insertBitsFromStrBits( dest, codes["ss"] );
    if( error ) {
        error += 1570000;
    }
    return error;
}

std::tuple< std::vector<int>, int, std::string > BitsForString( const std::string & data ) {
    std::string invalidChars;
    int error = 0;
    std::vector<int> out;
    for ( std::string::const_iterator it = data.begin() ; it < data.end(); it++ ) {
        std::string strbits = codes[ std::string( 1,*it ) ];
        if( strbits.size() == 0 ) {
            invalidChars.append( 1, *it );
            error += 163;
        } else {
            error += insertBitsFromStrBits( out, strbits );
        }
    }

    return std::make_tuple( out, error, invalidChars );
}

std::tuple<int, std::string> BitsWithPreamble( std::vector<int> & dest, const std::string & type, const std::string & data, bool force ) {
    std::string invalidChars;
    int error;
    std::vector<int> bits;
    std::tie( bits, error, invalidChars ) = BitsForString( data );
    if( error ) {
        error += 1630000;
    }

    if( bits.size() > 0 || force ) {
        std::string preambleStrBits = codes[ type ];
        if( preambleStrBits.size() > 0 ) {
            // Preamble
            int newerror = insertBitsFromStrBits( dest, preambleStrBits );
            if( newerror ) {
                newerror += 1930000;
            }
            error += newerror;

            // Data
            dest.insert( dest.end(), bits.begin(), bits.end() );
        } else {
            error += 2330000;
        }
    }

    return std::make_tuple( error, invalidChars );
}

std::tuple<int, std::string> BitsWithoutPreamble( std::vector<int> & dest, const std::string & data ) {
    std::string invalidChars;
    int error;
    std::vector<int> bits;
    std::tie( bits, error, invalidChars ) = BitsForString( data );
    if( error ) {
        error += 1630000;
    }

    // Data
    dest.insert( dest.end(), bits.begin(), bits.end() );

    return std::make_tuple( error, invalidChars );
}

std::tuple<int, std::string> BitsProtoSitePort( std::vector<int> & dest, const std::string & proto, std::string server, const std::string & port ) {
    int error = 0;
    int newerror;
    std::string invalidChars, newInvalidChars;

    // No preamble - protocol-etc. it is the default data section

    //
    // Port
    //

    std::vector<int> bits;
    if ( port.size() > 0 ) {
        bits.push_back( 1 );

        std::bitset< 16 > bin_port( std::stoi( port ) );
        for ( int i = 15; i >= 0; i -- )
            bits.push_back( bin_port[ i ] );

        dest.insert( dest.end(), bits.begin(), bits.end() );
    } else {
        bits.push_back( 0 );
        dest.insert( dest.end(), bits.begin(), bits.end() );
    }

    //
    // Protocol
    //

    std::string proto_bits = proto_code[ proto ];
    if ( proto_bits.size() != 3 )
        proto_bits = "111";
    error += insertBitsFromStrBits( dest, proto_bits );

    //
    // Site
    //

    bool skip=false;
    std::string site;

    std::transform( server.begin(), server.end(), server.begin(), ::tolower );
    if ( server_site.count( server ) > 0 ) {
        site = server_site[ server ];
    } else {
        skip = true;
    }

    if ( skip ) {
        // Inline server, with the repeating preamble
        std::tie( newerror, newInvalidChars ) = BitsWithPreamble( dest, "site_flags", server );
        invalidChars += newInvalidChars;
        error += newerror;
    } else {
        // Github is the default site
        if ( site == "gh" )
            skip = true;

        std::string site_lt = sites_flags[site];
        if ( site_lt.size() == 0 )
            skip = true;

        if ( ! skip ) {
            std::tie( newerror, newInvalidChars ) = BitsWithoutPreamble( dest, site_lt );
            invalidChars += newInvalidChars;
            error += newerror;
        }
    }

    return std::make_tuple( error, invalidChars );
}

int BitsStop( std::vector<int> & dest ) {
    int error = insertBitsFromStrBits( dest, codes["ss"] );
    if( error ) {
        error += 2570000;
    }
    return error;
}

// FUNCTION: BitsCompareSuffix {{{2
// Compares suffix of the longer "$1" with whole "$2"
std::tuple< bool, int > BitsCompareSuffix( const std::vector<int> & bits, const std::string & strBits ) {
    int error = 0;

    std::string tstrBits = trimmed( strBits );
    if( tstrBits.size() == 0 ) {
        error += 193;
        return std::make_tuple( true, error );
    }

    std::vector<int> bits2;
    for ( std::string::const_iterator it = tstrBits.begin(); it < tstrBits.end(); it ++ ) {
        if( *it == '1' ) {
            bits2.push_back( 1 );
        } else if ( *it == '0' ) {
            bits2.push_back( 0 );
        } else {
            error += 233;
            bits2.push_back( 1 );
        }
    }

    if( bits.size() < bits2.size() ) {
        return std::make_tuple( 0, error );
    }

    // Check if short_bits occur at the end of long_bits
    int beg_idx = bits.size() - bits2.size();
    int end_idx = bits.size();
    bool not_equal = false;
    int s = 0;
    for ( int l = beg_idx; l < end_idx; l++ ) {
        if( bits[l] != bits2[s] ) {
            not_equal = true;
            break;
        }

        if( (bits[l] != 0 && bits[l] != 1) || (bits2[s] != 0 && bits2[s] != 1) ) {
            error += 277;
        }

        s += 1;
    }

    return std::make_tuple( !not_equal, error );
}

// FUNCTION: BitsRemoveIfStartStop() {{{2
// This function removes any SS bits if meta-data is empty
int BitsRemoveIfStartStop( std::vector<int> & bits ) {
    bool result;
    int error;

    std::tie( result, error ) = BitsCompareSuffix( bits, codes[ "ss" ] );
    error += error ? 2770000 : 0;

    if( result ) {
        bits.erase( bits.end() - codes["ss"].size(), bits.end() );

        int error2;
        std::tie( result, error2 ) = BitsCompareSuffix( bits, codes[ "ss" ] );
        error += error2 ? error2 + 3110000 : 0;

        if( result ) {
            bits.erase( bits.end() - codes["ss"].size(), bits.end() );
            // Two consecutive SS bits occured, correct removal
        } else {
            // We couldn't remove second SS bits, so it means
            // that there is some meta data, and we should
            // restore already removed last SS bits
            int error3 = insertBitsFromStrBits( bits, codes["ss"] );
            error += error3 ? error3 + 3370000 : 0;
        }
    } else {
        // This shouldn't happen, this function must be
        // called after adding SS bits
        error += 3730000;
    }

    return error;
}

std::wstring build_gcode(
        std::string & protocol,
        std::string & user,
        std::string & site,
        std::string & port,
        std::string & repo,
        std::string & rev,
        std::string & file,
        std::vector<int> & selectors )
{
    std::vector<int> bits;

    bits.insert( bits.end(), selectors.begin(), selectors.end() );
    // Reverse, so that trailing zeros are eradicated from gcode
    std::reverse( bits.begin(), bits.end() );

    std::vector<int> appendix;
    int error=0, newerror;
    std::string invalidChars;

    // Site
    std::tie( newerror, invalidChars ) = BitsProtoSitePort( appendix, protocol, site, port );
    error += newerror;
    errorOnDisallowedChars( "protocol-site-port", invalidChars );

    // Repo is ensured to be always appended, even if somehow empty (should not happen)
    std::tie( newerror, invalidChars ) = BitsWithPreamble( appendix, "repo_rev_usr", repo, true );
    error += newerror;
    errorOnDisallowedChars( "repository-path", invalidChars );

    // Revision is always appended even if empty. The rationale
    // is that without revision, URL will be probably (...) short
    // anyway. Also, the main or at least serious motivation to
    // use giturl is to embed revision
    std::tie( newerror, invalidChars ) = BitsWithPreamble( appendix, "repo_rev_usr", rev, true );
    error += newerror;
    errorOnDisallowedChars( "revision", invalidChars );

    // User is normal, optional
    std::tie( newerror, invalidChars ) = BitsWithPreamble( appendix, "repo_rev_usr", user );
    error += newerror;
    errorOnDisallowedChars( "user-name", invalidChars );

    // File name
    std::tie( newerror, invalidChars ) = BitsWithPreamble( appendix, "file", file );
    error += newerror;
    errorOnDisallowedChars( "file", invalidChars );

    // Meta-data end
    error += BitsStop( appendix );

    // Append meta-data bits
    // They go to end, and are reversed, so to decode, one can
    // first reverse whole sequence, to have meta-data waiting
    // in the beginning, in order
    std::reverse( appendix.begin(), appendix.end() );
    bits.insert( bits.end(), appendix.begin(), appendix.end() );

    // Version
    bits.push_back( 0 );
    bits.push_back( 0 );

    // std::copy( bits.begin(), bits.end(), std::ostream_iterator<int>(std::cout,""));
    // std::cout << std::endl;

    // Create Zcode
    std::vector<wchar_t> gcode;
    std::vector<int> numbers;
    int error2;
    std::tie( gcode, numbers, error2 ) = encode_gcode_arr01( bits );
    error += error2;

    // Convert Zcode to QString
    std::wstring gcode2( gcode.begin(), gcode.end() );

    if( error ) {
        int exam = error - 1630000;
        // Display if there is other error besides use of invalid characters
        if( exam % 163 ) {
            std::cout << "Warning: Computation ended with code: " << error << std::endl;
        } else {
            std::cout << "Allowed characters are: a-z, A-Z, 0-9, /, ~, -, _, ., space" << std::endl;
        }

        return std::wstring();
    } else {
        return gcode2;
    }
}

void create_codes_map() {
    codes["ss"]         = "100111";
    codes["file"]       = "101000";
    codes["repo_rev_usr"] = "101111";
    codes["wordrev"]    = "101100";
    codes["site_flags"] = "100010";
    codes["unused1"]    = "100011";
    codes["unused2"]    = "101110";
    codes["unused3"]    = "1100110";
    codes["unused4"]    = "101101";

    codes[":"] = "100000";
    codes["b"] = "110001";
    codes["a"] = "110000";
    codes["9"] = "101011";
    codes["8"] = "101010";
    codes["."] = "101001";
    codes["/"] = "100110";
    codes["_"] = "100101";
    codes["-"] = "100100";
    codes["~"] = "100001";
    codes["x"] = "011111";
    codes["w"] = "011110";
    codes["z"] = "011101";
    codes["y"] = "011100";
    codes["t"] = "011011";
    codes["s"] = "011010";
    codes["v"] = "011001";
    codes["u"] = "011000";
    codes["5"] = "010111";
    codes["4"] = "010110";
    codes["7"] = "010101";
    codes["6"] = "010100";
    codes["1"] = "010011";
    codes["0"] = "010010";
    codes["3"] = "010001";
    codes["2"] = "010000";
    codes["h"] = "001111";
    codes["g"] = "001110";
    codes["j"] = "001101";
    codes["i"] = "001100";
    codes["d"] = "001011";
    codes["c"] = "001010";
    codes["f"] = "001001";
    codes["e"] = "001000";
    codes["p"] = "000111";
    codes["o"] = "000110";
    codes["r"] = "000101";
    codes["q"] = "000100";
    codes["l"] = "000011";
    codes["k"] = "000010";
    codes["n"] = "000001";
    codes["m"] = "000000";
    codes["J"] = "1111111";
    codes["I"] = "1111110";
    codes["L"] = "1111101";
    codes["K"] = "1111100";
    codes["F"] = "1111011";
    codes["E"] = "1111010";
    codes["H"] = "1111001";
    codes["G"] = "1111000";
    codes["R"] = "1110111";
    codes["Q"] = "1110110";
    codes["T"] = "1110101";
    codes["S"] = "1110100";
    codes["N"] = "1110011";
    codes["M"] = "1110010";
    codes["P"] = "1110001";
    codes["O"] = "1110000";
    codes["Z"] = "1101111";
    codes["Y"] = "1101110";
    codes[" "] = "1101101";
    codes["A"] = "1101100";
    codes["V"] = "1101011";
    codes["U"] = "1101010";
    codes["X"] = "1101001";
    codes["W"] = "1101000";
    codes["B"] = "1100111";
    codes["D"] = "1100101";
    codes["C"] = "1100100";
}

void create_rcodes_map() {
    rcodes["100111"]  = "ss";
    rcodes["101000"]  = "file";
    rcodes["101111"]  = "repo_rev_usr";
    rcodes["101100"]  = "wordrev";
    rcodes["100010"]  = "site_flags";
    rcodes["100011"]  = "unused1";
    rcodes["101110"]  = "unused2";
    rcodes["1100110"] = "unused3";
    rcodes["101101"]  = "unused4";

    rcodes["100000"] = ":";
    rcodes["110001"] = "b";
    rcodes["110000"] = "a";
    rcodes["101011"] = "9";
    rcodes["101010"] = "8";
    rcodes["101001"] = ".";
    rcodes["100110"] = "/";
    rcodes["100101"] = "_";
    rcodes["100100"] = "-";
    rcodes["100001"] = "~";
    rcodes["011111"] = "x";
    rcodes["011110"] = "w";
    rcodes["011101"] = "z";
    rcodes["011100"] = "y";
    rcodes["011011"] = "t";
    rcodes["011010"] = "s";
    rcodes["011001"] = "v";
    rcodes["011000"] = "u";
    rcodes["010111"] = "5";
    rcodes["010110"] = "4";
    rcodes["010101"] = "7";
    rcodes["010100"] = "6";
    rcodes["010011"] = "1";
    rcodes["010010"] = "0";
    rcodes["010001"] = "3";
    rcodes["010000"] = "2";
    rcodes["001111"] = "h";
    rcodes["001110"] = "g";
    rcodes["001101"] = "j";
    rcodes["001100"] = "i";
    rcodes["001011"] = "d";
    rcodes["001010"] = "c";
    rcodes["001001"] = "f";
    rcodes["001000"] = "e";
    rcodes["000111"] = "p";
    rcodes["000110"] = "o";
    rcodes["000101"] = "r";
    rcodes["000100"] = "q";
    rcodes["000011"] = "l";
    rcodes["000010"] = "k";
    rcodes["000001"] = "n";
    rcodes["000000"] = "m";
    rcodes["1111111"] = "J";
    rcodes["1111110"] = "I";
    rcodes["1111101"] = "L";
    rcodes["1111100"] = "K";
    rcodes["1111011"] = "F";
    rcodes["1111010"] = "E";
    rcodes["1111001"] = "H";
    rcodes["1111000"] = "G";
    rcodes["1110111"] = "R";
    rcodes["1110110"] = "Q";
    rcodes["1110101"] = "T";
    rcodes["1110100"] = "S";
    rcodes["1110011"] = "N";
    rcodes["1110010"] = "M";
    rcodes["1110001"] = "P";
    rcodes["1110000"] = "O";
    rcodes["1101111"] = "Z";
    rcodes["1101110"] = "Y";
    rcodes["1101101"] = " ";
    rcodes["1101100"] = "A";
    rcodes["1101011"] = "V";
    rcodes["1101010"] = "U";
    rcodes["1101001"] = "X";
    rcodes["1101000"] = "W";
    rcodes["1100111"] = "B";
    rcodes["1100101"] = "D";
    rcodes["1100100"] = "C";
}

void create_sites_maps() {
        sites_flags["gh"] = "1";
        sites_flags["bb"] = "2";
        sites_flags["gl"] = "3";

        rsites_flags["1"] = "gh";
        rsites_flags["2"] = "bb";
        rsites_flags["3"] = "gl";
}

void create_helper_maps() {
    names["rev"]  = "revision";
    names["file"] = "file name";
    names["repo"] = "user/repo";
    names["site"] = "site";
}

void create_server_maps() {
    server_site["github.com"]    = "gh";
    server_site["bitbucket.org"] = "bb";
    server_site["gitlab.com"]    = "gl";

    rserver_site["gh"] = "github.com";
    rserver_site["bb"] = "bitbucket.org";
    rserver_site["gl"] = "gitlab.com";
}

void create_protocol_maps() {
    proto_code["ssh"]   = "000";
    proto_code["git"]   = "001";
    proto_code["http"]  = "010";
    proto_code["https"] = "011";
    proto_code["ftp"]   = "100";
    proto_code["ftps"]  = "101";
    proto_code["rsync"] = "110";

    rproto_code["000"] =    "ssh";
    rproto_code["001"] =    "git";
    rproto_code["010"] =    "http";
    rproto_code["011"] =    "https";
    rproto_code["100"] =    "ftp";
    rproto_code["101"] =    "ftps";
    rproto_code["110"] =    "rsync";
}

std::map< std::string, std::string > & getCodes() { return codes; }
std::map< std::string, std::string > & getRCodes() { return rcodes; }
std::map< std::string, std::string > & getSitesFlags() { return sites_flags; }
std::map< std::string, std::string > & getRSitesFlags() { return rsites_flags; }
std::map< std::string, std::string > & getNames() { return names; }
std::map< std::string, std::string > & getServerSite() { return server_site; }
std::map< std::string, std::string > & getRServerSite() { return rserver_site; }
