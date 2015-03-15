#include "AdapterBase.h"

#include <QFile>
#include <QDebug>
#include <QWebView>
#include <QWebFrame>

using namespace QtcMarkview::Internal;

namespace {

  QByteArray readFile (const QString &name) {
    QFile f (name);
    QByteArray result;
    if (f.open (QFile::ReadOnly)) {
      result = f.readAll ();
      f.close ();
    }
    else {
      qCritical () << "Error opening file" << name;
    }
    return result;
  }

  //! Prepare string to be passed to js script.
  QString toJsString (const QString &source) {
    QString result = source;
    result = result
             .replace (QStringLiteral ("\n"), QStringLiteral ("\\n")) // support multiline
             .replace (QStringLiteral ("\""), QStringLiteral ("\\\"")) // support internal quotes
             .replace (QStringLiteral ("<script "),QStringLiteral ("&lt;script "))
             .replace (QStringLiteral ("<script>"),QStringLiteral ("&lt;script&gt;"))
             .replace (QStringLiteral ("</script>"),QStringLiteral ("&lt;/script&gt;"));
    return QStringLiteral ("\"") + result + QStringLiteral ("\"");
  }

}

AdapterBase::AdapterBase (const QString helpFileName, const QString htmlFileName)
  : helpFileName_ (helpFileName), htmlFileName_ (htmlFileName) {

}

AdapterBase::~AdapterBase () {

}

void AdapterBase::initView (const QString &plainText, QWebView *view) const {
  QString html;
  if (htmlFileName_.isEmpty ()) {
    html = plainText;
  }
  else {
    html = QString::fromLocal8Bit (readFile (htmlFileName_.toHtmlEscaped ()));
    html = html.arg (toJsString (plainText));
  }
  view->setHtml (html);
}

void AdapterBase::updateView (const QString &plainText, QWebView *view) const {
  if (htmlFileName_.isEmpty ()) {
    view->setHtml (plainText);
  }
  else {
    QString preprocessed = toJsString (plainText);
    QString updateScript = QString (QStringLiteral ("update (%1);")).arg (preprocessed);
    QWebFrame *mainFrame = view->page ()->mainFrame ();
    mainFrame->evaluateJavaScript (updateScript);
  }
}

QString AdapterBase::helpMessage () const {
  QString message;
  if (!helpFileName_.isEmpty ()) {
    message = QString::fromLocal8Bit (readFile (helpFileName_));
  }
  return message;
}

bool AdapterBase::isHelpAvailable () const {
  return !helpFileName_.isEmpty ();
}
