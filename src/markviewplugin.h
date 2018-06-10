#ifndef QTCMARKVIEW_H
#define QTCMARKVIEW_H

#include <extensionsystem/iplugin.h>

namespace QtcMarkview {
  namespace Internal {

    class MarkviewPlugin : public ExtensionSystem::IPlugin {
      Q_OBJECT
      Q_PLUGIN_METADATA (IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QtcMarkview.json")

      public:
        MarkviewPlugin ();
        ~MarkviewPlugin ();

        bool initialize (const QStringList &arguments, QString *errorString);
        void extensionsInitialized ();
        ShutdownFlag aboutToShutdown ();

      private:
        void initLanguage ();

    };

  } // namespace Internal
} // namespace QtcMarkview

#endif // QTCMARKVIEW_H

