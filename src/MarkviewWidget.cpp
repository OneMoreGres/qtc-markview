#include "MarkviewWidget.h"
#include "AdapterBase.h"
#include "Constants.h"

#include <QComboBox>
#include <QWebView>
#include <QWebSettings>
#include <QVBoxLayout>

using namespace QtcMarkview::Internal;

MarkviewWidget::MarkviewWidget (const Adapters &adapters)
  : webView_ (nullptr), adapters_ (adapters), currentAdapter_ (nullptr) {

#ifdef QT_DEBUG
  QWebSettings::globalSettings ()->setAttribute (QWebSettings::DeveloperExtrasEnabled, true);
#endif

  QComboBox *adapterCombo = new QComboBox (this);
  adapterCombo->addItems (adapters_.keys ());
  adapterCombo->setCurrentText (adapters_.key (nullptr));
  connect (adapterCombo, &QComboBox::currentTextChanged, this, &MarkviewWidget::currentAdapterChanged);
  insertExtraToolBarWidget (Left, adapterCombo);
  //TODO show help
}

MarkviewWidget::~MarkviewWidget () {
  delete webView_;
}

void MarkviewWidget::finalizeInitialization () {
  setupAsPlainEditor ();

  Q_ASSERT (textDocument ());
  connect (textDocument (), &TextEditor::TextDocument::contentsChanged,
           this, &MarkviewWidget::update);
}

void MarkviewWidget::currentAdapterChanged (const QString &newAdapterName) {
  currentAdapter_ = adapters_.value (newAdapterName, nullptr);
  changeView ();
  init ();
}

void MarkviewWidget::changeView () {
  // Create webView and place on top of base editor (not viewport).
  if (currentAdapter_ && !webView_) {
    webView_ = new QWebView (this);
    webView_->setStyleSheet (QStringLiteral ("QWebView {background: #FFFFFF;}"));
    QNetworkAccessManager *networkManager = webView_->page ()->networkAccessManager ();
    networkManager->setNetworkAccessible (QNetworkAccessManager::NotAccessible);
#ifndef QT_DEBUG
    webView_->setContextMenuPolicy (Qt::NoContextMenu);
#endif
    QVBoxLayout *layout = new QVBoxLayout (this);
    layout->setMargin (0);
    layout->addWidget (webView_);
  }

  if (currentAdapter_) {
    setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    webView_->show ();
  }
  else {
    setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy (Qt::ScrollBarAsNeeded);
    if (webView_) {
      webView_->hide ();
    }
  }
}

void MarkviewWidget::init () {
  if (currentAdapter_) {
    Q_ASSERT (webView_);
    Q_ASSERT (textDocument ());
    currentAdapter_->initView (textDocument ()->plainText (), webView_);
  }
}

void MarkviewWidget::update () {
  if (currentAdapter_) {
    Q_ASSERT (webView_);
    Q_ASSERT (textDocument ());
    currentAdapter_->updateView (textDocument ()->plainText (), webView_);
  }
}

void MarkviewWidget::showHelp () {
  if (!currentAdapter_ || !currentAdapter_->isHelpAvailable ()) {
    return;
  }
  QString helpText = currentAdapter_->helpMessage ();
  QString titlePattern (QStringLiteral ("Format help"));
  Core::EditorManager::openEditorWithContents (Constants::QTCMARKVIEW_ID, &titlePattern,
                                               helpText.toUtf8 ());
}
