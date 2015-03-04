#ifndef ADAPTERBASE_H
#define ADAPTERBASE_H

#include <QString>

namespace QtcMarkview {
  namespace Internal {

    class AdapterBase {
      public:
        AdapterBase ();
        virtual ~AdapterBase ();

        virtual QString toHtml (const QString &plainText) const = 0;

        const QString &helpMessage () const;
        bool isHelpAvailable () const;

      protected:
        QString helpMessage_;
    };

  } // namespace Internal
} // namespace QtcMarkview

#endif // ADAPTERBASE_H
