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

#include "math_functions.h"
#include "util.h"
#include <algorithm>
#include <regex>
#include <iostream>
#include <cstdint>

const wchar_t characters[1031]={
// Latin-1
L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9',
L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r', L's',
L't', L'u', L'v', L'w', L'x', L'y', L'z',
L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P', L'Q', L'R', L'S',
L'T', L'U', L'V', L'W', L'X', L'Y', L'Z',

// Latin-1 Supplement [µºÀ-ÖØ-áãäæ-öøú-ÿ]
L'µ', L'º', L'À', L'Á', L'Â', L'Ã', L'Ä', L'Å', L'Æ', L'Ç', L'È', L'É', L'Ê', L'Ë', L'Ì', L'Í', L'Î', L'Ï', L'Ð',
L'Ñ', L'Ò', L'Ó', L'Ô', L'Õ', L'Ö', L'Ø', L'Ù', L'Ú', L'Û', L'Ü', L'Ý', L'Þ', L'ß', L'à', L'á', L'ã', L'ä', L'æ',
L'ç', L'è', L'é', L'ê', L'ë', L'ì', L'í', L'î', L'ï', L'ð', L'ñ', L'ò', L'ó', L'ô', L'õ', L'ö', L'ø', L'ú', L'û',
L'ü', L'ý', L'þ', L'ÿ',

// Latin Extended-A [ÿ-ſ]
L'Ā', L'ā', L'Ă', L'ă', L'Ą', L'ą', L'Ć', L'ć', L'Ĉ', L'ĉ', L'Ċ', L'ċ', L'Č', L'č', L'Ď', L'ď', L'Đ', L'đ', L'Ē',
L'ē', L'Ĕ', L'ĕ', L'Ė', L'ė', L'Ę', L'ę', L'Ě', L'ě', L'Ĝ', L'ĝ', L'Ğ', L'ğ', L'Ġ', L'ġ', L'Ģ', L'ģ', L'Ĥ', L'ĥ',
L'Ħ', L'ħ', L'Ĩ', L'ĩ', L'Ī', L'ī', L'Ĭ', L'ĭ', L'Į', L'į', L'İ', L'ı', L'Ĳ', L'ĳ', L'Ĵ', L'ĵ', L'Ķ', L'ķ', L'ĸ',
L'Ĺ', L'ĺ', L'Ļ', L'ļ', L'Ľ', L'ľ', L'Ŀ', L'ŀ', L'Ł', L'ł', L'Ń', L'ń', L'Ņ', L'ņ', L'Ň', L'ň', L'ŉ', L'Ŋ', L'ŋ',
L'Ō', L'ō', L'Ŏ', L'ŏ', L'Ő', L'ő', L'Œ', L'œ', L'Ŕ', L'ŕ', L'Ŗ', L'ŗ', L'Ř', L'ř', L'Ś', L'ś', L'Ŝ', L'ŝ', L'Ş',
L'ş', L'Š', L'š', L'Ţ', L'ţ', L'Ť', L'ť', L'Ŧ', L'ŧ', L'Ũ', L'ũ', L'Ū', L'ū', L'Ŭ', L'ŭ', L'Ů', L'ů', L'Ű', L'ű',
L'Ų', L'ų', L'Ŵ', L'ŵ', L'Ŷ', L'ŷ', L'Ÿ', L'Ź', L'ź', L'Ż', L'ż', L'Ž', L'ž', L'ſ',

// Latin Extended-B [ſ-ƔƖ-ơƤ-ǃǍ-Ǳǳ-ǵǷ-Ɉɋ-ɏ]
L'ƀ', L'Ɓ', L'Ƃ', L'ƃ', L'Ƅ', L'ƅ', L'Ɔ', L'Ƈ', L'ƈ', L'Ɖ', L'Ɗ', L'Ƌ', L'ƌ', L'ƍ', L'Ǝ', L'Ə', L'Ɛ', L'Ƒ', L'ƒ',
L'Ɠ', L'Ɣ', L'Ɩ', L'Ɨ', L'Ƙ', L'ƙ', L'ƚ', L'ƛ', L'Ɯ', L'Ɲ', L'ƞ', L'Ɵ', L'Ơ', L'ơ', L'Ƥ', L'ƥ', L'Ʀ', L'Ƨ', L'ƨ',
L'Ʃ', L'ƪ', L'ƫ', L'Ƭ', L'ƭ', L'Ʈ', L'Ư', L'ư', L'Ʊ', L'Ʋ', L'Ƴ', L'ƴ', L'Ƶ', L'ƶ', L'Ʒ', L'Ƹ', L'ƹ', L'ƺ', L'ƻ',
L'Ƽ', L'ƽ', L'ƾ', L'ƿ', L'ǀ', L'ǁ', L'ǂ', L'ǃ', L'Ǎ', L'ǎ', L'Ǐ', L'ǐ', L'Ǒ', L'ǒ', L'Ǔ', L'ǔ', L'Ǖ', L'ǖ', L'Ǘ',
L'ǘ', L'Ǚ', L'ǚ', L'Ǜ', L'ǜ', L'ǝ', L'Ǟ', L'ǟ', L'Ǡ', L'ǡ', L'Ǣ', L'ǣ', L'Ǥ', L'ǥ', L'Ǧ', L'ǧ', L'Ǩ', L'ǩ', L'Ǫ',
L'ǫ', L'Ǭ', L'ǭ', L'Ǯ', L'ǯ', L'ǰ', L'Ǳ', L'ǳ', L'Ǵ', L'ǵ', L'Ƿ', L'Ǹ', L'ǹ', L'Ǻ', L'ǻ', L'Ǽ', L'ǽ', L'Ǿ', L'ǿ',
L'Ȁ', L'ȁ', L'Ȃ', L'ȃ', L'Ȅ', L'ȅ', L'Ȇ', L'ȇ', L'Ȉ', L'ȉ', L'Ȋ', L'ȋ', L'Ȍ', L'ȍ', L'Ȏ', L'ȏ', L'Ȑ', L'ȑ', L'Ȓ',
L'ȓ', L'Ȕ', L'ȕ', L'Ȗ', L'ȗ', L'Ș', L'ș', L'Ț', L'ț', L'Ȝ', L'ȝ', L'Ȟ', L'ȟ', L'Ƞ', L'ȡ', L'Ȣ', L'ȣ', L'Ȥ', L'ȥ',
L'Ȧ', L'ȧ', L'Ȩ', L'ȩ', L'Ȫ', L'ȫ', L'Ȭ', L'ȭ', L'Ȯ', L'ȯ', L'Ȱ', L'ȱ', L'Ȳ', L'ȳ', L'ȴ', L'ȵ', L'ȶ', L'ȷ', L'ȸ',
L'ȹ', L'Ⱥ', L'Ȼ', L'ȼ', L'Ƚ', L'Ⱦ', L'ȿ', L'ɀ', L'Ɂ', L'ɂ', L'Ƀ', L'Ʉ', L'Ʌ', L'Ɇ', L'ɇ', L'Ɉ', L'ɋ', L'Ɍ', L'ɍ',
L'Ɏ', L'ɏ',

// Latin Extended Additional [Ḁ-ẝẟ-ỹỻ]
L'Ḁ', L'ḁ', L'Ḃ', L'ḃ', L'Ḅ', L'ḅ', L'Ḇ', L'ḇ', L'Ḉ', L'ḉ', L'Ḋ', L'ḋ', L'Ḍ', L'ḍ', L'Ḏ', L'ḏ', L'Ḑ', L'ḑ', L'Ḓ',
L'ḓ', L'Ḕ', L'ḕ', L'Ḗ', L'ḗ', L'Ḙ', L'ḙ', L'Ḛ', L'ḛ', L'Ḝ', L'ḝ', L'Ḟ', L'ḟ', L'Ḡ', L'ḡ', L'Ḣ', L'ḣ', L'Ḥ', L'ḥ',
L'Ḧ', L'ḧ', L'Ḩ', L'ḩ', L'Ḫ', L'ḫ', L'Ḭ', L'ḭ', L'Ḯ', L'ḯ', L'Ḱ', L'ḱ', L'Ḳ', L'ḳ', L'Ḵ', L'ḵ', L'Ḷ', L'ḷ', L'Ḹ',
L'ḹ', L'Ḻ', L'ḻ', L'Ḽ', L'ḽ', L'Ḿ', L'ḿ', L'Ṁ', L'ṁ', L'Ṃ', L'ṃ', L'Ṅ', L'ṅ', L'Ṇ', L'ṇ', L'Ṉ', L'ṉ', L'Ṋ', L'ṋ',
L'Ṍ', L'ṍ', L'Ṏ', L'ṏ', L'Ṑ', L'ṑ', L'Ṓ', L'ṓ', L'Ṕ', L'ṕ', L'Ṗ', L'ṗ', L'Ṙ', L'ṙ', L'Ṛ', L'ṛ', L'Ṝ', L'ṝ', L'Ṟ',
L'ṟ', L'Ṡ', L'ṡ', L'Ṣ', L'ṣ', L'Ṥ', L'ṥ', L'Ṧ', L'ṧ', L'Ṩ', L'ṩ', L'Ṫ', L'ṫ', L'Ṭ', L'ṭ', L'Ṯ', L'ṯ', L'Ṱ', L'ṱ',
L'Ṳ', L'ṳ', L'Ṵ', L'ṵ', L'Ṷ', L'ṷ', L'Ṹ', L'ṹ', L'Ṻ', L'ṻ', L'Ṽ', L'ṽ', L'Ṿ', L'ṿ', L'Ẁ', L'ẁ', L'Ẃ', L'ẃ', L'Ẅ',
L'ẅ', L'Ẇ', L'ẇ', L'Ẉ', L'ẉ', L'Ẋ', L'ẋ', L'Ẍ', L'ẍ', L'Ẏ', L'ẏ', L'Ẑ', L'ẑ', L'Ẓ', L'ẓ', L'Ẕ', L'ẕ', L'ẖ', L'ẗ',
L'ẘ', L'ẙ', L'ẚ', L'ẛ', L'ẜ', L'ẝ', L'ẟ', L'Ạ', L'ạ', L'Ả', L'ả', L'Ấ', L'ấ', L'Ầ', L'ầ', L'Ẩ', L'ẩ', L'Ẫ', L'ẫ',
L'Ậ', L'ậ', L'Ắ', L'ắ', L'Ằ', L'ằ', L'Ẳ', L'ẳ', L'Ẵ', L'ẵ', L'Ặ', L'ặ', L'Ẹ', L'ẹ', L'Ẻ', L'ẻ', L'Ẽ', L'ẽ', L'Ế',
L'ế', L'Ề', L'ề', L'Ể', L'ể', L'Ễ', L'ễ', L'Ệ', L'ệ', L'Ỉ', L'ỉ', L'Ị', L'ị', L'Ọ', L'ọ', L'Ỏ', L'ỏ', L'Ố', L'ố',
L'Ồ', L'ồ', L'Ổ', L'ổ', L'Ỗ', L'ỗ', L'Ộ', L'ộ', L'Ớ', L'ớ', L'Ờ', L'ờ', L'Ở', L'ở', L'Ỡ', L'ỡ', L'Ợ', L'ợ', L'Ụ',
L'ụ', L'Ủ', L'ủ', L'Ứ', L'ứ', L'Ừ', L'ừ', L'Ử', L'ử', L'Ữ', L'ữ', L'Ự', L'ự', L'Ỳ', L'ỳ', L'Ỵ', L'ỵ', L'Ỷ', L'ỷ',
L'Ỹ', L'ỹ', L'ỻ',

// Greek and Coptic [ͻ-ͽΆΈΑ-ΡΣ-ΩΫ-ψϋ-ώϐ-ϒϔ-Ϡϥϧ-ϩϫ-ϲϵϷ-Ϲϻ-Ͽ]
L'ͻ', L'ͼ', L'ͽ', L'Ά', L'Έ', L'Α', L'Β', L'Γ', L'Δ', L'Ε', L'Ζ', L'Η', L'Θ', L'Ι', L'Κ', L'Λ', L'Μ', L'Ν', L'Ξ',
L'Ο', L'Π', L'Ρ', L'Σ', L'Τ', L'Υ', L'Φ', L'Χ', L'Ψ', L'Ω', L'Ϋ', L'ά', L'έ', L'ή', L'ί', L'ΰ', L'α', L'β', L'γ',
L'δ', L'ε', L'ζ', L'η', L'θ', L'ι', L'κ', L'λ', L'μ', L'ν', L'ξ', L'ο', L'π', L'ρ', L'ς', L'σ', L'τ', L'υ', L'φ',
L'χ', L'ψ', L'ϋ', L'ό', L'ύ', L'ώ', L'ϐ', L'ϑ', L'ϒ', L'ϔ', L'ϕ', L'ϖ', L'ϗ', L'Ϙ', L'ϙ', L'Ϛ', L'ϛ', L'Ϝ', L'ϝ',
L'Ϟ', L'ϟ', L'Ϡ', L'ϥ', L'ϧ', L'Ϩ', L'ϩ', L'ϫ', L'Ϭ', L'ϭ', L'Ϯ', L'ϯ', L'ϰ', L'ϱ', L'ϲ', L'ϵ', L'Ϸ', L'ϸ', L'Ϲ',
L'ϻ', L'ϼ', L'Ͻ', L'Ͼ', L'Ͽ',

// Cyrillic [Ѐ-џѡ-ѣѥѫѱ-ѳѵѷѻѽѿ-ҁҋ-ҟҡ-ҥҩ-ҳҵ-һӀ-ӌӎӐ-ӿ]
L'Ѐ', L'Ё', L'Ђ', L'Ѓ', L'Є', L'Ѕ', L'І', L'Ї', L'Ј', L'Љ', L'Њ', L'Ћ', L'Ќ', L'Ѝ', L'Ў', L'Џ', L'А', L'Б', L'В',
L'Г', L'Д', L'Е', L'Ж', L'З', L'И', L'Й', L'К', L'Л', L'М', L'Н', L'О', L'П', L'Р', L'С', L'Т', L'У', L'Ф', L'Х',
L'Ц', L'Ч', L'Ш', L'Щ', L'Ъ', L'Ы', L'Ь', L'Э', L'Ю', L'Я', L'а', L'б', L'в', L'г', L'д', L'е', L'ж', L'з', L'и',
L'й', L'к', L'л', L'м', L'н', L'о', L'п', L'р', L'с', L'т', L'у', L'ф', L'х', L'ц', L'ч', L'ш', L'щ', L'ъ', L'ы',
L'ь', L'э', L'ю', L'я', L'ѐ', L'ё', L'ђ', L'ѓ', L'є', L'ѕ', L'і', L'ї', L'ј', L'љ', L'њ', L'ћ', L'ќ', L'ѝ', L'ў',
L'џ', L'ѡ', L'Ѣ', L'ѣ', L'ѥ', L'ѫ', L'ѱ', L'Ѳ', L'ѳ', L'ѵ', L'ѷ', L'ѻ', L'ѽ', L'ѿ', L'Ҁ', L'ҁ', L'ҋ', L'Ҍ', L'ҍ',
L'Ҏ', L'ҏ', L'Ґ', L'ґ', L'Ғ', L'ғ', L'Ҕ', L'ҕ', L'Җ', L'җ', L'Ҙ', L'ҙ', L'Қ', L'қ', L'Ҝ', L'ҝ', L'Ҟ', L'ҟ', L'ҡ',
L'Ң', L'ң', L'Ҥ', L'ҥ', L'ҩ', L'Ҫ', L'ҫ', L'Ҭ', L'ҭ', L'Ү', L'ү', L'Ұ', L'ұ', L'Ҳ', L'ҳ', L'ҵ', L'Ҷ', L'ҷ', L'Ҹ',
L'ҹ', L'Һ', L'һ', L'Ӏ', L'Ӂ', L'ӂ', L'Ӄ', L'ӄ', L'Ӆ', L'ӆ', L'Ӈ', L'ӈ', L'Ӊ', L'ӊ', L'Ӌ', L'ӌ', L'ӎ', L'Ӑ', L'ӑ',
L'Ӓ', L'ӓ', L'Ӕ', L'ӕ', L'Ӗ', L'ӗ', L'Ә', L'ә', L'Ӛ', L'ӛ', L'Ӝ', L'ӝ', L'Ӟ', L'ӟ', L'Ӡ', L'ӡ', L'Ӣ', L'ӣ', L'Ӥ',
L'ӥ', L'Ӧ', L'ӧ', L'Ө', L'ө', L'Ӫ', L'ӫ', L'Ӭ', L'ӭ', L'Ӯ', L'ӯ', L'Ӱ', L'ӱ', L'Ӳ', L'ӳ', L'Ӵ', L'ӵ', L'Ӷ', L'ӷ',
L'Ӹ', L'ӹ', L'Ӻ', L'ӻ', L'Ӽ', L'ӽ', L'Ӿ', L'ӿ',

// Hebrew [א-דטך-מס-שװ] / [א-דטך-מס-פ]
L'א', L'ב', L'ג', L'ד', L'ט', L'ך', L'כ', L'ל', L'ם', L'מ', L'ס', L'ע', L'ף', L'פ', L'ץ', L'צ', L'ק', L'ר', L'ש',
L'װ',

'\0' };

const wchar_t *characters_begin = &characters[0];
const wchar_t *characters_end = &characters[1024];

// FUNCTION: convert_integer_to_base_1024 {{{
// Takes number, returns string that is the representation
// of the number in base-1024

std::tuple< std::vector<wchar_t>, int > convert_integer_to_base_1024(int number) {
    std::vector<int> digits;

    int new_number = number;
    int remainder;
    while ( new_number != 0 ) {
        remainder = new_number % 1024;
        new_number = new_number/1024;

        digits.push_back( remainder );
    }

    if ( digits.size() == 0 )
        digits.push_back(0);
    if ( digits.size() == 1 )
        digits.push_back(0);
    if ( digits.size() == 2 )
        digits.push_back(0);

    std::reverse(digits.begin(), digits.end());

    std::tuple<std::vector<wchar_t>, int> letters_error = numbers_to_letters(digits);

    return letters_error;
}
// }}}

// FUNCTION: numbers_to_letters {{{
// Converts array of numbers into string [a-z0-9]+
std::tuple< std::vector<wchar_t>, int> numbers_to_letters(const std::vector<int> & digits) {
    std::vector<wchar_t> REPLY;
    for( std::vector<int>::const_iterator it = digits.begin(); it != digits.end(); it ++ ) {
        int i = *it;
        if ( i < 0 || i > 1024 ) {
            std::cout << "Incorrect number during character conversion: " << i << std::endl;
            return std::tuple<std::vector<wchar_t>, int>( REPLY, 1 );
        }
        REPLY.push_back( characters[i] );
    }

    return std::tuple<std::vector<wchar_t>, int>( REPLY, 0 );
}

// FUNCTION: letters_to_numbers {{{
// Converts each character of input string into number 0..35
std::tuple< std::vector<int>, int> letters_to_numbers( const std::vector<wchar_t> & letters ) {
    std::vector<int> reply;

    std::wstring str( &letters[0], (&letters[0]) + letters.size() );

    std::wregex bits_regex(L"^[0-9a-zA-ZµºÀ-ÖØ-áãäæ-öøú-ƔƖ-ơƤ-ǃǍ-Ǳǳ-ǵǷ-Ɉɋ-ɏḀ-ẝẟ-ỹỻͻ-ͽΆΈΑ-ΡΣ-ΩΫ-ψϋ-ώϐ-ϒϔ-Ϡϥϧ-ϩϫ-ϲϵϷ-Ϲϻ-ϿЀ-џѡ-ѣѥѫѱ-ѳѵѷѻѽѿ-ҁҋ-ҟҡ-ҥҩ-ҳҵ-һӀ-ӌӎӐ-ӿא-דטך-מס-פ]+$");
    if ( !std::regex_search( str, bits_regex ) )
        std::cout << "Incorrect character in gcode detected by range testing" << std::endl;

    int number;
    for( std::vector<wchar_t>::const_iterator it = letters.begin(); it != letters.end(); it ++ ) {
        const wchar_t *number_it = std::find( characters_begin, characters_end, *it );
        if ( number_it == characters_end ) {
            std::cout << "Incorrect character: " << single_to_narrow( *it ) << std::endl;
            return std::make_tuple( reply, 100 );
        } else {
            number = std::distance( characters_begin, number_it );
            reply.push_back( number );
        }
    }

    return std::make_tuple( reply, 0 );
}

std::tuple< std::vector<int>, int> letters_to_numbers( const std::wstring & letters ) {
    std::vector<wchar_t> letters2( letters.c_str(), letters.c_str() + letters.size() );
    return letters_to_numbers( letters2 );
}

// FUNCTION: decode_gcode {{{
// Takes zekyl code, i.e. 1/someletters
// and decodes it to sequence of bits
std::tuple< std::vector<int>, int > decode_gcode( const std::wstring & letters ) {
    std::vector<int> bits;
    std::wstring workingvar = letters;
    std::wregex rx(L"^0*$");
    while( !std::regex_search( workingvar, rx ) ) {
        int subtracted, error;
        std::tie( workingvar, subtracted, error ) = div2( workingvar );
        if( error != 0 ) {
            return std::make_tuple( std::vector<int>(), 103 );
        }
        bits.push_back( subtracted );
        // print "After div $workingvar/${reply[2]}"
    }
    std::reverse( bits.begin(), bits.end() );
    // print "Bits of the letters $letters are: ${(j::)bits[@]}"

    return std::make_tuple( bits, 0 );
}
// }}}

// FUNCTION: get_integer_from_base_1024 {{{
// Converts given base-1024 string into integer
// Warning: it tagets integer (signed), so
// the size of number is limited here (while
// decode_gcode generates series of bits of
// arbitrary length)
std::tuple<int, int> get_integer_from_base_1024( const std::wstring & letters ) {
    std::vector<int> bits;
    std::wstring workingvar = letters;

    // Get base-2 generated array consisting of 1 and 0
    std::wregex zeros_regex(L"0*$$");
    while ( !std::regex_search( workingvar, zeros_regex ) ) {
        int subtracted, error;
        std::tie( workingvar, subtracted, error ) = div2( workingvar );
        if( error != 0 ) {
            return std::make_tuple( 0, error + 1000 );
        }
        bits.push_back( subtracted );
    }

    std::reverse( bits.begin(), bits.end() );

    // Now sum up the obtained 0 and 1
    int i, mul = 1, size = bits.size();
    int REPLY=0;
    for ( i = size - 1; i >= 0; i -- ) {
        REPLY = REPLY + bits[i]*mul;
        mul = mul * 2;
    }

    // TODO: detect overflow and other problems
    return std::make_tuple( REPLY, 0 );
}
// }}}

// FUNCTION: encode_gcode_str01 {{{
// Takes string of 0 and 1 that mark which zekylls are active
// and encodes it to base-1024 number
std::tuple< std::vector<wchar_t>, std::vector<int>, int > encode_gcode_str01( const std::string & sbits ) {
    std::vector<int> numbers;
    int error;
    std::tie( numbers, error ) = str_01_to_24_bit_pack_numbers( sbits );
    if( error != 0 ) {
        return make_tuple( std::vector<wchar_t>(), std::vector<int>(), error + 2000 );
    }

    std::vector<wchar_t> code;
    std::tie( code, numbers, error ) = encode_gcode_24_bit_pack_numbers( numbers );
    if( error != 0 ) {
        return make_tuple( code, numbers, error + 3000 );
    }

    return make_tuple( code, numbers, 0 );
}
// }}}

// FUNCTION: encode_gcode_arr01 {{{
// Takes array of 0 and 1 that mark which zekylls are
// active and encodes it to base-1024 number
std::tuple< std::vector<wchar_t>, std::vector<int>, int > encode_gcode_arr01( const std::vector<int> & bits ) {
    std::vector<int> numbers;
    int error;
    std::tie( numbers, error ) = arr_01_to_24_bit_pack_numbers( bits );
    if( error != 0 ) {
        return make_tuple( std::vector<wchar_t>(), std::vector<int>(), error + 8000 );
    }

    std::vector<wchar_t> code;
    std::tie( code, numbers, error ) = encode_gcode_24_bit_pack_numbers( numbers );
    if( error != 0 ) {
        return make_tuple( code, numbers, error + 9000 );
    }
    return make_tuple( code, numbers, 0 );
}
// }}}

// FUNCTION: encode_gcode_24-bit_pack_numbers {{{
// Takes 24-bit pack numbers whose bits mark which zekylls are active
// and encodes them to base-1024 number
std::tuple< std::vector<wchar_t>, std::vector<int>, int > encode_gcode_24_bit_pack_numbers( const std::vector<int> & numbers ) {
    std::vector<int> nums_base1024, workingvar;
    workingvar = numbers;

    bool all_zero = true;
    for( unsigned int i = 0; i < workingvar.size(); i ++ ) {
        if( workingvar[i] != 0 ) {
            all_zero = false;
            break;
        }
    }

    while ( !all_zero ) {
        int error, subtracted;
        std::tie( workingvar, subtracted, error ) = div_24_bit_pack_numbers_1024( workingvar );
        nums_base1024.push_back( subtracted );

        all_zero = true;
        for( unsigned int i = 0; i < workingvar.size(); i ++ ) {
            if( workingvar[i] != 0 ) {
                all_zero = false;
                break;
            }
        }
    }

    std::reverse( nums_base1024.begin(), nums_base1024.end() );

    std::vector<wchar_t> letters;
    int error;
    tie( letters, error ) = numbers_to_letters( nums_base1024 );

    if( error != 0 ) {
        error += 4000;
    }

    return make_tuple( letters, nums_base1024, error );
}
// }}}

// FUNCTION: div2 {{{
// input - gcode's letters
// Result – ( "gcode's letters after division" "remainder 0 or 1" )
std::tuple< std::vector<wchar_t>, int, int > div2( const std::vector<wchar_t> & letters ) {
    // First translate the letters to numbers and put them into array
    std::tuple< std::vector<int>, int > res = letters_to_numbers( letters );
    int error = std::get<1>( res );
    if( error != 0 ) {
        return std::make_tuple( std::vector<wchar_t>(), 0, error + 5000 );
    }

    std::vector<int> numbers = std::get<0>( res );

    // Now operate on the array performing long-division
    int cur = 0, last = numbers.size() - 1;

    std::vector<int> result;

    int prepared_for_division = numbers[cur];
    int subtracted;
    while (( 1 )) {
        int quotient = prepared_for_division / 2;

        result.push_back( quotient );

        int recovered = quotient*2;
        subtracted = prepared_for_division - recovered;

        cur ++;
        if ( cur > last ) {
            break;
        }

        prepared_for_division = 1024 * subtracted + numbers[cur];
    }

    // Now convert the result to letters

    std::tuple< std::vector<wchar_t>, int > res2 = numbers_to_letters( result );
    error = std::get<1>(res2);
    if( error != 0 ) {
        return std::make_tuple( std::vector<wchar_t>(), 0, error + 6000 );
    }

    return std::make_tuple( std::get<0>( res2 ), subtracted, 0 );
}
// }}}

// FUNCTION: div2 {{{
std::tuple< std::wstring, int, int > div2( const std::wstring & letters ) {
    std::vector<wchar_t> out_letters;
    int subtracted, error;
    tie( out_letters, subtracted, error ) = div2( std::vector<wchar_t>( letters.c_str(), letters.c_str() + letters.size() ) );
    if( error != 0 ) {
        error += 7000;
    }
    return std::make_tuple( std::wstring( out_letters.begin(), out_letters.end() ), subtracted, error );
}
// }}}

// FUNCTION: str_01_to_24-bit_pack_numbers {{{
// Takes STRING of 0 and 1 and converts it to array of numbers
// that are 24-bit packs taken from right to left, from the string
std::tuple< std::vector<int>, int > str_01_to_24_bit_pack_numbers( const std::string & sbits ) {
    std::vector<int> bits;
    for ( std::string::const_iterator it = sbits.begin() ; it < sbits.end(); it++ ) {
        bool ok = false;
        int bit = -1;
        if ( *it == '0' )
            bit = 0;
        else if ( *it == '1' )
            bit = 1;

        if( bit != 0 && bit != 1 ) {
            return make_tuple( std::vector<int>(), 105 );
        }

        bits.push_back( bit );
    }

    return arr_01_to_24_bit_pack_numbers( bits );
}
// }}}

// FUNCTION: arr_01_to_24-bit_pack_numbers {{{
// Takes ARRAY (@) of 0 and 1 and converts it to array of numbers
// that are 24-bit packs taken from right to left, from the string
std::tuple< std::vector<int>, int > arr_01_to_24_bit_pack_numbers( const std::vector<int> & bits ) {
    std::vector<int> pack, numbers;
    int count=0, i, size = bits.size();

    // Take packs of 24 bits, convert each to number and store in array
    for ( i = size - 1; i >= 0; i-- ) {
        if( bits[i] != 0 && bits[i] != 1 ) {
            return std::make_tuple( std::vector<int>(), 106 );
        }

        pack.push_back( bits[i] );
        count+=1;
        if ( count < 24 && i != 0 ) {
                continue;
        }

        count = 0;
        std::reverse( pack.begin(), pack.end() );

        // Convert the max. 24 bit pack to number
        int result = 0;
        for( unsigned int p = 0; p < pack.size(); p ++ ) {
            result = result*2 + pack[p];
        }

        numbers.push_back( result );

        pack.clear();
    }

    std::reverse( numbers.begin(), numbers.end() );

    return std::make_tuple( numbers, 0 );
}
// }}}

// FUNCTION: div_24-bit_pack_numbers_1024 {{{
// input - series of 0 and 1 (bits marking which zekyll is active)
// output - result of division and remainder 0 ... 35, also error
std::tuple< std::vector<int>, int, int > div_24_bit_pack_numbers_1024( const std::vector<int> & numbers ) {
    // Now operate on the array performing long-division
    int cur = 0, last = numbers.size() - 1;

    std::vector<int> result;

    int64_t subtracted;
    int64_t prepared_for_division = numbers[cur];
    while ( 1 ) {
        int quotient = prepared_for_division / 1024;

        result.push_back( quotient );

        int64_t recovered = quotient * 1024;
        subtracted = prepared_for_division - recovered;


        cur ++;
        if ( cur > last ) {
            break;
        }

        // std::cout << "Div loop run, prepared_for_division: " << prepared_for_division << ", quotient: " << quotient << ", recovered: " << recovered << ", subtracted: " << subtracted << ", new numbers[cur]: " << numbers[cur] << std::endl;

        prepared_for_division = 16777216 * subtracted + numbers[cur];

        if( numbers[cur] > prepared_for_division ) {
            return make_tuple( std::vector<int>(), 0, 107);
        }
    }

    return std::make_tuple( result, subtracted, 0 );
}
// }}}
