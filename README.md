# Qt Creator markup plugin

## Introduction
Plugin performs preview of documents written in some markup languages.

![Screen](util/screen.png?raw=true)

## Supported languages
* [Asciidoctor](http://asciidoctor.org/)
* Html
* [Markdown](http://daringfireball.net/projects/markdown/)
* [Textile](http://txstyle.org/)

## Usage
1. Right click on document (via project explorer or editor) and choose `Open with->Markview`.
2. Open same document in another split.
3. Change view format in editor's menu.
4. Click on `?` icon near format selector to view some syntax help for selected format.

## Downloads
Binaries can be downloaded [here](https://sourceforge.net/projects/qtc-markview/files/ "Sourceforge")
or from github releases.

## Installation
IMPORTANT: plugin's version must match Qt Creator's version (difference in last digit is acceptable)

### From source
1. Change paths.pri:

 - set `QTCREATOR_SOURCES` = path to Qt Creator source dir (with qtcreator.pro)
 - set `IDE_BUILD_TREE` = path to compiled Qt Creator dir (with bin,lib,libexec,...)

2. Compile plugin.

### From binaries
1. Extract/copy files from two archives (with plugin and dependencies) into Qt Creator's dir (archives already contains proper paths).
2. On linux launch Qt Creator via `qtcreator.sh` file.
3. Enable plugin in Help->Modules menu.

## More info
Additional information can be obtained [here](http://gres.biz/qtc-markview/ "Homepage") (in russian)
