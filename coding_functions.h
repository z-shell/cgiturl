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

#ifndef CODING_FUNCTIONS_H
#define CODING_FUNCTIONS_H
#include <tuple>
#include <vector>
#include <string>
#include <map>
#include <list>

void create_codes_map();
void create_rcodes_map();
void create_sites_maps();
void create_helper_maps();
void create_server_maps();
void create_protocol_maps();

std::map< std::string, std::string > & getCodes();
std::map< std::string, std::string > & getRCodes();
std::map< std::string, std::string > & getSites();
std::map< std::string, std::string > & getRSites();
std::map< std::string, std::string > & getNames();
std::map< std::string, std::string > & getServerSite();
std::map< std::string, std::string > & getRServerSite();

std::tuple< int, std::map< std::string, std::string >, int >
process_meta_data( const std::vector<int> & _bits );

int BitsStart( std::vector<int> & dest );
int BitsStop( std::vector<int> & dest );
int BitsRemoveIfStartStop( std::vector<int> & bits );

std::tuple< int, std::string >
BitsWithPreamble( std::vector<int> & dest, const std::string & type, const std::string & data, bool force = false );

std::tuple< bool, int >
BitsCompareSuffix( const std::vector<int> & bits, const std::u32string & strBits );

std::tuple< bool, int >
BitsCompareSuffix( const std::vector<int> & bits, const std::u32string & strBits );

int
insertBitsFromStrBits( std::vector<int> & dest, const std::string & str );

std::wstring
build_gcode(
        std::string & protocol,
        std::string & user,
        std::string & site,
        std::string & port,
        std::string & repo,
        std::string & rev,
        std::string & file,
        std::vector<int> & selectors );

#endif // CODING_FUNCTIONS_H
