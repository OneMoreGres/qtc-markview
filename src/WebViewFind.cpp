#include "WebViewFind.h"

#include <QWebEngineView>
#include <QDebug>

using namespace Core;

QWebEnginePage::FindFlags toWebPageFlags (FindFlags flags) {
  QWebEnginePage::FindFlags result = 0;
  result = (flags & FindBackward) ? (result | QWebEnginePage::FindBackward) : result;
  result = (flags & FindCaseSensitively) ? (result | QWebEnginePage::FindCaseSensitively) : result;
  return result;
}


WebViewFind::WebViewFind (QWebEngineView *view)
  : view_ (view) {
}

bool WebViewFind::supportsReplace () const {
  return true;
}

FindFlags WebViewFind::supportedFindFlags () const {
  return FindBackward | FindCaseSensitively | FindWholeWords
         | FindRegularExpression | FindPreserveCase;
}

void WebViewFind::resetIncrementalSearch () {
  Q_ASSERT (!view_.isNull ());
  view_->findText (QString (), toWebPageFlags (0));
}

void WebViewFind::clearHighlights () {
  Q_ASSERT (!view_.isNull ());
  view_->findText (QString (), toWebPageFlags (0));
}

QString WebViewFind::currentFindString () const {
  Q_ASSERT (!view_.isNull ());
  return view_->selectedText ();
}

QString WebViewFind::completedFindString () const {
  Q_ASSERT (!view_.isNull ());
  return view_->selectedText ();
}

WebViewFind::Result WebViewFind::findIncremental (const QString &txt, FindFlags findFlags) {
  Q_ASSERT (!view_.isNull ());
  view_->findText (QString (), toWebPageFlags (0));
  view_->findText (txt, toWebPageFlags (findFlags));
  return NotFound;
}

WebViewFind::Result WebViewFind::findStep (const QString &txt, FindFlags findFlags) {
  Q_ASSERT (!view_.isNull ());
  view_->findText (txt, toWebPageFlags (findFlags));
  return NotFound;
}
