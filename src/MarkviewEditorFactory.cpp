#include "MarkviewEditorFactory.h"
#include "Constants.h"
#include "MarkviewWidget.h"
#include "AdapterBase.h"

#include <QCoreApplication>
#include <QSettings>

#include <texteditor/normalindenter.h>
#include <texteditor/textdocument.h>
#include <coreplugin/icore.h>

using namespace QtcMarkview::Internal;

namespace {
  Adapters adapters;
  const QString group = "QtcMarkview";
  const QString field = "adapters";
}

MarkviewEditorFactory::MarkviewEditorFactory () {
  setId (Constants::QTCMARKVIEW_ID);
  setDisplayName (QCoreApplication::translate ("OpenWith::Editors",
                                               Constants::QTCMARKVIEW_DISPLAY_NAME));
  addMimeType ("text/plain");

  adapters.insert (tr ("Plain"), nullptr);
  adapters.insert (tr ("Markdown"), new AdapterBase (QStringLiteral (":/help/markdown.md"),
                                                     QStringLiteral (":/html/markdown.html")));
  adapters.insert (tr ("Asciidoctor"), new AdapterBase (QStringLiteral (":/help/asciidoctor.adoc"),
                                                        QStringLiteral (":/html/asciidoctor.html")));
  adapters.insert (tr ("Textile"), new AdapterBase (QStringLiteral (":/help/textile.textile"),
                                                    QStringLiteral (":/html/textile.html")));
  adapters.insert (tr ("Html"), new AdapterBase (QStringLiteral (":/help/html.html")));

  loadCustomAdapters ();

  setDocumentCreator ([]() {
    return new TextEditor::TextDocument (Constants::QTCMARKVIEW_ID);
  });
  setEditorCreator ([]() {
    return new TextEditor::BaseTextEditor;
  });
  setEditorWidgetCreator ([]() {
    return new MarkviewWidget (adapters);
  });
  setIndenterCreator ([]() {
    return new TextEditor::NormalIndenter;
  });
}

MarkviewEditorFactory::~MarkviewEditorFactory () {
  saveCustomAdapters ();
  qDeleteAll (adapters.values ());
  adapters.clear ();
}

void MarkviewEditorFactory::loadCustomAdapters () {
  Q_ASSERT (Core::ICore::settings () != NULL);
  QSettings &settings = *(Core::ICore::settings ());
  settings.beginGroup (group);
  auto custom = settings.value (field).toString ().split (",");
  for (const auto &i: custom) {
    auto parts = i.split (":::");
    if (parts.size () < 2) {
      continue;
    }
    adapters.insert (parts[0], new AdapterBase ({}, parts[1], true));
  }
}

void MarkviewEditorFactory::saveCustomAdapters () {
  Q_ASSERT (Core::ICore::settings () != NULL);
  QSettings &settings = *(Core::ICore::settings ());
  settings.beginGroup (group);
  QStringList custom;
  for (const auto &name: adapters.keys ()) {
    auto adapter = adapters[name];
    if (adapter->isCustom ()) {
      custom << QString ("%1:::%2").arg (name, adapter->htmlFileName ());
    }
  }
  settings.setValue (field, custom.join (","));
}
