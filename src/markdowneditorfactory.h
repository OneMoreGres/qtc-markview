#ifndef MARKDOWNEDITORFACTORY_H
#define MARKDOWNEDITORFACTORY_H

#include <texteditor/texteditor.h>

namespace QtcMarkview {
  namespace Internal {

    class MarkdownEditorFactory : public TextEditor::TextEditorFactory {
      Q_OBJECT

      public:
        explicit MarkdownEditorFactory ();
    };

  } // namespace Internal
} // namespace QtcMarkview

#endif // MARKDOWNEDITORFACTORY_H
