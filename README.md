# `CGITURL`

---

> **Note:** 
> - This repository is outdated and may not work properly.
> - Please consider opening a pull request or an issue, if would like to speed up development.

## What is CGiturl?

It's a tool that allows you to pack Git repository data into single URL. For example:

```shell
% cgiturl https://github.com/z-shell/giturl -r devel -p lib/coding_functions.cpp

Protocol: https
Site:     github.com
Repo:     z-shell/giturl
Revision: devel
File:     lib/coding_functions.cpp

gitu://ŬṽǚǫoŒẗ6ẏȅcЭÑẩőn4ầŘїệαЃȣϟṈӛŀї

% cgiturl -d gitu://ŬṽǚǫoŒẗ6ẏȅcЭÑẩőn4ầŘїệαЃȣϟṈӛŀї
Protocol: https
Site:     github.com
Repo:     z-shell/giturl
Revision: devel
File:     lib/coding_functions.cpp
```

This is `C++11` implementation. There is [the main Zsh implementation too](https://github.com/z-shell/giturl).
You don't have to use Zsh as your shell, just have it installed, like Ruby.

The resulting URL is highly compressed thanks to use of Huffman codes and base-1024 encoding.
Compare length of the above data to the `gcode` in URL:

```shell
ŬṽǚǫoŒẗ6ẏȅcЭÑẩőn4ầŘїệαЃȣϟṈӛŀї
https://github.com/z-shell/giturldevellib/coding_functions.cpp
```

It is `29` vs `62` characters, and you would normally need additional description like "the branch
is: ", etc. `Giturl` allows to easily grab repository data from web page and terminal.

## Limitations

Only following characters can appear in input data – in the server, repository path, revision, etc.: `[a-zA-Z0-9._~:-]`

## Characters used in base-1024 encoding

The characters are carefully chosen so that double clicking GitURL selects the whole `gcode`.
Asian scripts are not used because characters from them are double-width and they would consume
the space saved by the compressions.

```
0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
µºÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞßàáãäæçèéêëìíîïðñòóôõöøúûüýþÿĀ
āĂăĄąĆćĈĉĊċČčĎďĐđĒēĔĕĖėĘęĚěĜĝĞğĠġĢģĤĥĦħĨĩĪīĬĭĮįİıĲĳĴĵĶķĸĹĺĻļĽľ
ĿŀŁłŃńŅņŇňŉŊŋŌōŎŏŐőŒœŔŕŖŗŘřŚśŜŝŞşŠšŢţŤťŦŧŨũŪūŬŭŮůŰűŲųŴŵŶŷŸŹźŻż
ŽžſƀƁƂƃƄƅƆƇƈƉƊƋƌƍƎƏƐƑƒƓƔƖƗƘƙƚƛƜƝƞƟƠơƤƥƦƧƨƩƪƫƬƭƮƯưƱƲƳƴƵƶƷƸƹƺƻƼƽ
ƾƿǀǁǂǃǍǎǏǐǑǒǓǔǕǖǗǘǙǚǛǜǝǞǟǠǡǢǣǤǥǦǧǨǩǪǫǬǭǮǯǰǱǳǴǵǷǸǹǺǻǼǽǾǿȀȁȂȃȄȅȆ
ȇȈȉȊȋȌȍȎȏȐȑȒȓȔȕȖȗȘșȚțȜȝȞȟȠȡȢȣȤȥȦȧȨȩȪȫȬȭȮȯȰȱȲȳȴȵȶȷȸȹȺȻȼȽȾȿɀɁɂɃɄ
ɅɆɇɈɋɌɍɎɏḀḁḂḃḄḅḆḇḈḉḊḋḌḍḎḏḐḑḒḓḔḕḖḗḘḙḚḛḜḝḞḟḠḡḢḣḤḥḦḧḨḩḪḫḬḭḮḯḰḱḲḳḴ
ḵḶḷḸḹḺḻḼḽḾḿṀṁṂṃṄṅṆṇṈṉṊṋṌṍṎṏṐṑṒṓṔṕṖṗṘṙṚṛṜṝṞṟṠṡṢṣṤṥṦṧṨṩṪṫṬṭṮṯṰṱṲ
ṳṴṵṶṷṸṹṺṻṼṽṾṿẀẁẂẃẄẅẆẇẈẉẊẋẌẍẎẏẐẑẒẓẔẕẖẗẘẙẚẛẜẝẟẠạẢảẤấẦầẨẩẪẫẬậẮắẰằ
ẲẳẴẵẶặẸẹẺẻẼẽẾếỀềỂểỄễỆệỈỉỊịỌọỎỏỐốỒồỔổỖỗỘộỚớỜờỞởỠỡỢợỤụỦủỨứỪừỬửỮữ
ỰựỲỳỴỵỶỷỸỹỻͻͼͽΆΈΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩΫάέήίΰαβγδεζηθικλμνξοπ
ρςστυφχψϋόύώϐϑϒϔϕϖϗϘϙϚϛϜϝϞϟϠϥϧϨϩϫϬϭϮϯϰϱϲϵϷϸϹϻϼϽϾϿЀЁЂЃЄЅІЇЈЉЊЋЌ
ЍЎЏАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъ
ыьэюяѐёђѓєѕіїјљњћќѝўџѡѢѣѥѫѱѲѳѵѷѻѽѿҀҁҋҌҍҎҏҐґҒғҔҕҖҗҘҙҚқҜҝҞҟҡҢңҤҥ
ҩҪҫҬҭҮүҰұҲҳҵҶҷҸҹҺһӀӁӂӃӄӅӆӇӈӉӊӋӌӎӐӑӒӓӔӕӖӗӘәӚӛӜӝӞӟӠӡӢӣӤӥӦӧӨөӪӫӬӭ
ӮӯӰӱӲӳӴӵӶӷӸӹӺӻӼӽӾӿאבגדטךכלםמסעףפץצקרשװ
```
