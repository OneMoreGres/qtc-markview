#include <QDesktopServices>

#include "PreviewPage.h"

PreviewPage::PreviewPage (QObject *parent) : QWebEnginePage (parent) {
}

bool PreviewPage::acceptNavigationRequest (const QUrl &url, QWebEnginePage::NavigationType /*type*/,
                                           bool /*isMainFrame*/) {
  QDesktopServices::openUrl (url);
  return false;
}
