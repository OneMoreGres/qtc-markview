#ifndef WEBVIEWFIND_H
#define WEBVIEWFIND_H

#include <QPointer>

#include <coreplugin/find/ifindsupport.h>

class QWebEngineView;

class WebViewFind : public Core::IFindSupport {
  Q_OBJECT

  public:
    WebViewFind (QWebEngineView *view);

    bool supportsReplace () const;
    Core::FindFlags supportedFindFlags () const;
    void resetIncrementalSearch ();
    void clearHighlights ();
    QString currentFindString () const;
    QString completedFindString () const;
    Result findIncremental (const QString &txt, Core::FindFlags findFlags);
    Result findStep (const QString &txt, Core::FindFlags findFlags);

  private:
    QPointer<QWebEngineView> view_;
};

#endif // WEBVIEWFIND_H
