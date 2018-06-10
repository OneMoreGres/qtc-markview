DEFINES += QTCMARKVIEW_LIBRARY

VERSION_SUFFIX = ""

include(paths.pri)

# QtcMarkview files

SOURCES += \
    src/markdownhighlighter.cpp \
    src/markdowntextedit.cpp \
    src/markdowneditorfactory.cpp \
    src/markviewplugin.cpp

HEADERS += \
    src/constants.h \
    src/markdownhighlighter.h \
    src/markdowntextedit.h \
    src/markdowneditorfactory.h \
    src/markviewplugin.h

TRANSLATIONS += \
    translation/QtcMarkview_ru.ts

OTHER_FILES += \
    LICENSE.md \
    README.md \
    uncrustify.cfg \
    dist/README.md \
    dist/make_dist.bat \
    dist/make_dist.sh

PROVIDER = Gres

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = QtcMarkview
QTC_LIB_DEPENDS += \
    # nothing here at this time

QTC_PLUGIN_DEPENDS += \
    coreplugin \
    texteditor

QTC_PLUGIN_RECOMMENDS += \
    # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
