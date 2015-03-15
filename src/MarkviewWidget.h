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

      signals:
        void helpAvailabilityChanged (bool gotHelp);

      private slots:
        void currentAdapterChanged (const QString &newAdapterName);
        void changeView ();
        void init ();
        void update ();
        void showHelp ();

      private:
        QWebView *webView_;

        const Adapters &adapters_;
        const AdapterBase *currentAdapter_;
        QString currentAdapterName_;

    };


  } // namespace Internal
} // namespace QtcMarkview


#endif // MARKVIEWWIDGET_H
