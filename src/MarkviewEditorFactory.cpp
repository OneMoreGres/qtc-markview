#include "MarkviewEditorFactory.h"
#include "Constants.h"
#include "MarkviewWidget.h"
#include "AdapterBase.h"

#include <QCoreApplication>

#include <texteditor/normalindenter.h>
#include <texteditor/textdocument.h>

using namespace QtcMarkview::Internal;

namespace {
  Adapters adapters;
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
  qDeleteAll (adapters.values ());
  adapters.clear ();
}
