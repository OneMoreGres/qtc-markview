#include "markviewplugin.h"
#include "constants.h"
#include "markdowneditorfactory.h"

#include <QCoreApplication>
#include <QTranslator>

#include <QtPlugin>

#include <coreplugin/icore.h>

using namespace QtcMarkview::Internal;

MarkviewPlugin::MarkviewPlugin () :
  IPlugin () {
  // Create your members
}

MarkviewPlugin::~MarkviewPlugin () {
  // Unregister objects from the plugin manager's object pool
  // Delete members
}

bool MarkviewPlugin::initialize (const QStringList &arguments, QString *errorString) {
  // Register objects in the plugin manager's object pool
  // Load settings
  // Add actions to menus
  // Connect to other plugins' signals
  // In the initialize function, a plugin can be sure that the plugins it
  // depends on have initialized their members.

  Q_UNUSED (arguments)
  Q_UNUSED (errorString)

  initLanguage ();

  new MarkdownEditorFactory (this);

  return true;
}

void MarkviewPlugin::initLanguage () {
  const QString &language = Core::ICore::userInterfaceLanguage ();
  if (!language.isEmpty ()) {
    QStringList paths;
    paths << Core::ICore::resourcePath () << Core::ICore::userResourcePath ();
    const QString &trFile = QLatin1String ("QtcMarkview_") + language;
    QTranslator *translator = new QTranslator (this);
    foreach (const QString &path, paths) {
      if (translator->load (trFile, path + QLatin1String ("/translations"))) {
        qApp->installTranslator (translator);
        break;
      }
    }
  }
}

void MarkviewPlugin::extensionsInitialized () {
  // Retrieve objects from the plugin manager's object pool
  // In the extensionsInitialized function, a plugin can be sure that all
  // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag MarkviewPlugin::aboutToShutdown () {
  // Save settings
  // Disconnect from signals that are not needed during shutdown
  // Hide UI (if you add UI that is not in the main window directly)
  return SynchronousShutdown;
}
