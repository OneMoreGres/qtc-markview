#include "MarkviewWidget.h"
#include "AdapterBase.h"
#include "Constants.h"
#include "WebViewFind.h"

#include <QComboBox>
#include <QWebView>
#include <QWebSettings>
#include <QVBoxLayout>
#include <QToolButton>

#include <aggregation/aggregate.h>
#include <texteditor/textdocument.h>
#include <coreplugin/editormanager/editormanager.h>
#include <texteditor/texteditorconstants.h>

using namespace QtcMarkview::Internal;

MarkviewWidget::MarkviewWidget (const Adapters &adapters)
  : webView_ (nullptr), adapters_ (adapters), currentAdapter_ (nullptr) {

#ifdef QT_DEBUG
  QWebSettings::globalSettings ()->setAttribute (QWebSettings::DeveloperExtrasEnabled, true);
#endif

  QToolButton *helpButton = new QToolButton (this);
  helpButton->setIcon (QIcon (QStringLiteral (":icons/question1.png")));
  helpButton->setToolTip (QStringLiteral ("Show format help"));
  connect (helpButton, &QToolButton::clicked, this, &MarkviewWidget::showHelp);
  connect (this, &MarkviewWidget::helpAvailabilityChanged, helpButton, &QToolButton::setEnabled);
  insertExtraToolBarWidget (Left, helpButton);

  QComboBox *adapterCombo = new QComboBox (this);
  QStringList adapterNames = adapters_.keys ();
  adapterNames.sort ();
  adapterCombo->addItems (adapterNames);
  QString plainName = adapters_.key (nullptr);
  adapterCombo->setCurrentText (plainName); // required if plain is not first
  currentAdapterChanged (plainName);
  connect (adapterCombo, &QComboBox::currentTextChanged, this, &MarkviewWidget::currentAdapterChanged);
  insertExtraToolBarWidget (Left, adapterCombo);
}

MarkviewWidget::~MarkviewWidget () {
  delete webView_;
}

void MarkviewWidget::finalizeInitialization () {
  textDocument()->setMimeType(QLatin1String(TextEditor::Constants::C_TEXTEDITOR_MIMETYPE_TEXT));

  setupGenericHighlighter ();
  configureGenericHighlighter ();

  Q_ASSERT (textDocument ());
  connect (textDocument (), &TextEditor::TextDocument::contentsChanged,
           this, &MarkviewWidget::update);
}

void MarkviewWidget::currentAdapterChanged (const QString &newAdapterName) {
  currentAdapter_ = adapters_.value (newAdapterName, nullptr);
  emit helpAvailabilityChanged (currentAdapter_ && currentAdapter_->isHelpAvailable ());
  currentAdapterName_ = newAdapterName;
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

    // Search support
    Aggregation::Aggregate *aggregate = new Aggregation::Aggregate;
    WebViewFind *webViewFind = new WebViewFind (webView_);
    aggregate->add (webViewFind);
    aggregate->add (webView_);
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
  QString titlePattern = QString (QStringLiteral ("%1 help")).arg (currentAdapterName_);
  Core::EditorManager::openEditorWithContents (Constants::QTCMARKVIEW_ID, &titlePattern,
                                               helpText.toUtf8 ());
}
