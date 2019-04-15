#include "markdowneditorfactory.h"
#include "constants.h"

#include "markdowntextedit.h"

#include <texteditor/normalindenter.h>
#include <texteditor/textdocument.h>

namespace QtcMarkview {
  namespace Internal {

    MarkdownEditorFactory::MarkdownEditorFactory (QObject *parent) :
      TextEditor::TextEditorFactory (parent) {
      setId (Constants::MARKDOWN_EDITOR_ID);
      setDisplayName (QCoreApplication::translate ("OpenWith::Editors",
                                                   Constants::MARKDOWN_EDITOR_NAME));
      addMimeType ("text/plain");
      addMimeType ("text/markdown");

      setDocumentCreator ([]() {
        return new TextEditor::TextDocument (Constants::MARKDOWN_EDITOR_ID);
      });
      setEditorCreator ([]() {
        return new TextEditor::BaseTextEditor;
      });
      setEditorWidgetCreator ([]() {
        return new QMarkdownTextEdit;
      });
      setIndenterCreator ([](QTextDocument *doc) {
        return new TextEditor::NormalIndenter (doc);
      });
      //      setSyntaxHighlighterCreator()
    }

  } // namespace Internal
} // namespace QtcMarkview
