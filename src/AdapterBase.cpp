#include "AdapterBase.h"

#include <QFile>
#include <QDebug>
#include <QWebEngineView>
#include <QWebEnginePage>

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

AdapterBase::AdapterBase (const QString helpFileName, const QString htmlFileName, bool isCustom)
  : helpFileName_ (helpFileName), htmlFileName_ (htmlFileName), isCustom_ (isCustom) {

}

AdapterBase::~AdapterBase () {

}

void AdapterBase::initView (const QString &plainText, const QString &baseDir,
                            QWebEngineView *view) const {
  QString html;
  if (htmlFileName_.isEmpty ()) {
    html = plainText;
  }
  else {
    html = QString::fromLocal8Bit (readFile (htmlFileName_.toHtmlEscaped ()));
    html = html.arg (toJsString (plainText));
  }

  view->setHtml (html, QUrl::fromLocalFile (baseDir + QStringLiteral ("/")));
}

void AdapterBase::updateView (const QString &plainText, const QString &baseDir,
                              QWebEngineView *view) const {
  if (htmlFileName_.isEmpty ()) {
    view->setHtml (plainText, QUrl::fromLocalFile (baseDir + QStringLiteral ("/")));
  }
  else {
    QString preprocessed = toJsString (plainText);
    QString updateScript = QString (QStringLiteral ("update (%1);")).arg (preprocessed);
    QWebEnginePage *mainFrame = view->page ();
    mainFrame->runJavaScript (updateScript);
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

bool AdapterBase::isCustom () const {
  return isCustom_;
}

QString AdapterBase::htmlFileName () const {
  return htmlFileName_;
}
