#ifndef ADAPTERMARKDOWN_H
#define ADAPTERMARKDOWN_H

#include "AdapterBase.h"

namespace QtcMarkview {
  namespace Internal {

    class AdapterMarkdown : public AdapterBase {
      public:
        AdapterMarkdown ();
        ~AdapterMarkdown ();

        QString toHtml (const QString &plainText) const Q_DECL_OVERRIDE;
    };

  } // namespace Internal
} // namespace QtcMarkview

#endif // ADAPTERMARKDOWN_H
