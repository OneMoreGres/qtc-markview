#include "WebViewFind.h"

#include <QWebView>
#include <QDebug>

using namespace Core;

QWebPage::FindFlags toWebPageFlags (FindFlags flags, bool highlightAll) {
  QWebPage::FindFlags result = QWebPage::FindWrapsAroundDocument;
  result = (flags & FindBackward) ? (result | QWebPage::FindBackward) : result;
  result = (flags & FindCaseSensitively) ? (result | QWebPage::FindCaseSensitively) : result;
  result = (highlightAll) ? (result | QWebPage::HighlightAllOccurrences) : result;
  return result;
}


WebViewFind::WebViewFind (QWebView *view)
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
  view_->findText (QString (), toWebPageFlags (0, true));
}

void WebViewFind::clearHighlights () {
  Q_ASSERT (!view_.isNull ());
  view_->findText (QString (), toWebPageFlags (0, true));
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
  view_->findText (QString (), toWebPageFlags (0, true));
  bool found = view_->findText (txt, toWebPageFlags (findFlags, true));
  if (found) {
    view_->findText (QString ()); // to start selection from beginning
    findStep (txt, findFlags);
  }
  return (found) ? Found : NotFound;
}

WebViewFind::Result WebViewFind::findStep (const QString &txt, FindFlags findFlags) {
  Q_ASSERT (!view_.isNull ());
  bool found = view_->findText (txt, toWebPageFlags (findFlags, false));
  return (found) ? Found : NotFound;
}
