#ifndef MARKVIEWWIDGET_H
#define MARKVIEWWIDGET_H

#include <texteditor/texteditor.h>

class QWebView;

namespace QtcMarkview {
  namespace Internal {

    class AdapterBase;
    typedef QHash<QString, AdapterBase *> Adapters;

    class MarkviewWidget : public TextEditor::TextEditorWidget {
      Q_OBJECT

      public:
        MarkviewWidget (const Adapters &adapters);
        ~MarkviewWidget ();

        void finalizeInitialization ();

      private slots:
        void currentAdapterChanged (const QString &newAdapterName);
        void changeView ();
        void reload ();

      private:
        QWebView *webView_;

        const Adapters &adapters_;
        const AdapterBase *currentAdapter_;

    };


  } // namespace Internal
} // namespace QtcMarkview


#endif // MARKVIEWWIDGET_H
