#ifndef MARKDOWNEDITORFACTORY_H
#define MARKDOWNEDITORFACTORY_H

#include <texteditor/texteditor.h>

namespace QtcMarkview {
  namespace Internal {

    class MarkdownEditorFactory : public TextEditor::TextEditorFactory {
      Q_OBJECT

      public:
        explicit MarkdownEditorFactory (QObject *parent = nullptr);
    };

  } // namespace Internal
} // namespace QtcMarkview

#endif // MARKDOWNEDITORFACTORY_H
