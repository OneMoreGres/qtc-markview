#ifndef MARKVIEWWIDGET_H
#define MARKVIEWWIDGET_H

#include <texteditor/texteditor.h>

class QWebEngineView;
class QComboBox;

namespace QtcMarkview {
  namespace Internal {

    class AdapterBase;
    typedef QHash<QString, AdapterBase *> Adapters;

    class MarkviewWidget : public TextEditor::TextEditorWidget {
      Q_OBJECT

      public:
        MarkviewWidget (Adapters &adapters);
        ~MarkviewWidget ();

        void finalizeInitialization ();

      signals:
        void helpAvailabilityChanged (bool gotHelp);
        void isCustomEditorChanged (bool gotHelp);

      private slots:
        void currentAdapterChanged (const QString &newAdapterName);
        void changeView ();
        void init ();
        void update ();
        void showHelp ();
        void addCustomEditor ();
        void removeCustomEditor ();

      private:
        QWebEngineView *webView_;
        QComboBox *adapterCombo_;

        Adapters &adapters_;
        const AdapterBase *currentAdapter_;
        QString currentAdapterName_;

    };


  } // namespace Internal
} // namespace QtcMarkview


#endif // MARKVIEWWIDGET_H
