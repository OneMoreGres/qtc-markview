#ifndef MARKVIEWEDITORFACTORY_H
#define MARKVIEWEDITORFACTORY_H

#include <texteditor/texteditor.h>

namespace QtcMarkview {
  namespace Internal {

    class MarkviewEditorFactory : public TextEditor::TextEditorFactory {
      Q_OBJECT

      public:
        MarkviewEditorFactory ();
        ~MarkviewEditorFactory ();
    };

  } // namespace Internal
} // namespace QtcMarkview


#endif // MARKVIEWEDITORFACTORY_H
