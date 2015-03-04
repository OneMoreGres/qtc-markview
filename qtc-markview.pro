DEFINES += QTCMARKVIEW_LIBRARY

VERSION_SUFFIX = ""

include(paths.pri)

QT += webkitwidgets

# QtcMarkview files

SOURCES += \
    src/QtcMarkviewPlugin.cpp \
    src/MarkviewEditorFactory.cpp \
    src/AdapterBase.cpp \
    src/AdapterMarkdown.cpp \
    src/MarkviewWidget.cpp

HEADERS += \
    src/Constants.h \
    src/PluginGlobal.h \
    src/QtcMarkviewPlugin.h \
    src/MarkviewEditorFactory.h \
    src/AdapterBase.h \
    src/AdapterMarkdown.h \
    src/MarkviewWidget.h

TRANSLATIONS += \
    translation/QtcMarkview_ru.ts

OTHER_FILES += \
    LICENSE.md \
    README.md \
    uncrustify.cfg

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

