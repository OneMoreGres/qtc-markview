#ifndef ADAPTERBASE_H
#define ADAPTERBASE_H

#include <QString>

class QWebView;

namespace QtcMarkview {
  namespace Internal {

    class AdapterBase {
      public:
        AdapterBase (const QString helpFileName = QString (),
                     const QString htmlFileName = QString ());
        virtual ~AdapterBase ();

        virtual void initView (const QString &plainText, QWebView *view) const;
        virtual void updateView (const QString &plainText, QWebView *view) const;

        QString helpMessage () const;
        bool isHelpAvailable () const;

      protected:
        QString helpFileName_;
        QString htmlFileName_;
    };

  } // namespace Internal
} // namespace QtcMarkview

#endif // ADAPTERBASE_H
