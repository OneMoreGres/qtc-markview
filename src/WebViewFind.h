#ifndef WEBVIEWFIND_H
#define WEBVIEWFIND_H

#include <QPointer>

#include <coreplugin/find/ifindsupport.h>

class QWebView;

class WebViewFind : public Core::IFindSupport {
  Q_OBJECT

  public:
    WebViewFind (QWebView *view);

    bool supportsReplace () const;
    Core::FindFlags supportedFindFlags () const;
    void resetIncrementalSearch ();
    void clearHighlights ();
    QString currentFindString () const;
    QString completedFindString () const;
    Result findIncremental (const QString &txt, Core::FindFlags findFlags);
    Result findStep (const QString &txt, Core::FindFlags findFlags);

  private:
    QPointer<QWebView> view_;
};

#endif // WEBVIEWFIND_H
