#include "MarkviewEditorFactory.h"
#include "Constants.h"
#include "MarkviewWidget.h"
#include "AdapterMarkdown.h"

#include <QCoreApplication>

#include <texteditor/normalindenter.h>

using namespace QtcMarkview::Internal;

namespace {
  Adapters adapters;
}

MarkviewEditorFactory::MarkviewEditorFactory () {
  setId (Constants::QTCMARKVIEW_ID);
  setDisplayName (QCoreApplication::translate ("OpenWith::Editors", Constants::QTCMARKVIEW_DISPLAY_NAME));
  addMimeType ("text/plain");

  adapters.insert (tr ("Plain"), nullptr);
  adapters.insert (tr ("Markdown"), new AdapterMarkdown);

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
