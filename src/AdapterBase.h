#ifndef ADAPTERBASE_H
#define ADAPTERBASE_H

#include <QString>

class QWebEngineView;

namespace QtcMarkview {
  namespace Internal {

    class AdapterBase {
      public:
        AdapterBase (const QString helpFileName = QString (),
                     const QString htmlFileName = QString (),
                     bool isCustom = false);
        virtual ~AdapterBase ();

        virtual void initView (const QString &plainText, const QString &baseDir,
                               QWebEngineView *view) const;
        virtual void updateView (const QString &plainText, const QString &baseDir,
                                 QWebEngineView *view) const;

        QString helpMessage () const;
        bool isHelpAvailable () const;

        bool isCustom () const;
        QString htmlFileName () const;

      protected:
        QString helpFileName_;
        QString htmlFileName_;
        bool isCustom_;
    };

  } // namespace Internal
} // namespace QtcMarkview

#endif // ADAPTERBASE_H
