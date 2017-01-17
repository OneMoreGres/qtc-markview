#include "MarkviewWidget.h"
#include "AdapterBase.h"
#include "Constants.h"
#include "WebViewFind.h"
#include "PreviewPage.h"

#include <QComboBox>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QVBoxLayout>
#include <QToolButton>
#include <QFileInfo>
#include <QScrollBar>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>

#include <aggregation/aggregate.h>
#include <texteditor/textdocument.h>
#include <coreplugin/editormanager/editormanager.h>
#include <texteditor/texteditorconstants.h>

using namespace QtcMarkview::Internal;


MarkviewWidget::MarkviewWidget (Adapters &adapters)
  : webView_ (nullptr),adapterCombo_ (new QComboBox (this)),
  adapters_ (adapters), currentAdapter_ (nullptr) {

  QToolButton *removeButton = new QToolButton (this);
  removeButton->setIcon (QIcon (QStringLiteral (":icons/list-remove.png")));
  removeButton->setToolTip (QStringLiteral ("Remove custom editor"));
  connect (removeButton, &QToolButton::clicked, this, &MarkviewWidget::removeCustomEditor);
  connect (this, &MarkviewWidget::isCustomEditorChanged, removeButton, &QToolButton::setEnabled);
  insertExtraToolBarWidget (Left, removeButton);

  QToolButton *addButton = new QToolButton (this);
  addButton->setIcon (QIcon (QStringLiteral (":icons/list-add.png")));
  addButton->setToolTip (QStringLiteral ("Add custom editor"));
  connect (addButton, &QToolButton::clicked, this, &MarkviewWidget::addCustomEditor);
  insertExtraToolBarWidget (Left, addButton);

  QToolButton *helpButton = new QToolButton (this);
  helpButton->setIcon (QIcon (QStringLiteral (":icons/question1.png")));
  helpButton->setToolTip (QStringLiteral ("Show format help"));
  connect (helpButton, &QToolButton::clicked, this, &MarkviewWidget::showHelp);
  connect (this, &MarkviewWidget::helpAvailabilityChanged, helpButton, &QToolButton::setEnabled);
  insertExtraToolBarWidget (Left, helpButton);

  QStringList adapterNames = adapters_.keys ();
  adapterNames.sort ();
  adapterCombo_->addItems (adapterNames);
  QString plainName = adapters_.key (nullptr);
  adapterCombo_->setCurrentText (plainName); // required if plain is not first
  currentAdapterChanged (plainName);
  connect (adapterCombo_, &QComboBox::currentTextChanged, this, &MarkviewWidget::currentAdapterChanged);
  insertExtraToolBarWidget (Left, adapterCombo_);
}

MarkviewWidget::~MarkviewWidget () {
  delete webView_;
}

void MarkviewWidget::finalizeInitialization () {
  textDocument ()->setMimeType (QLatin1String (TextEditor::Constants::C_TEXTEDITOR_MIMETYPE_TEXT));

  setupGenericHighlighter ();
  configureGenericHighlighter ();

  Q_ASSERT (textDocument ());
  connect (textDocument (), &TextEditor::TextDocument::contentsChanged,
           this, &MarkviewWidget::update);
}

void MarkviewWidget::currentAdapterChanged (const QString &newAdapterName) {
  currentAdapter_ = adapters_.value (newAdapterName, nullptr);
  emit helpAvailabilityChanged (currentAdapter_ && currentAdapter_->isHelpAvailable ());
  emit isCustomEditorChanged (currentAdapter_ && currentAdapter_->isCustom ());
  currentAdapterName_ = newAdapterName;
  changeView ();
  init ();
}

void MarkviewWidget::changeView () {
  // Create webView and place on top of base editor (not viewport).
  if (currentAdapter_ && !webView_) {
    webView_ = new QWebEngineView (this);
    webView_->setPage (new PreviewPage (webView_));
    webView_->setStyleSheet (QStringLiteral ("QWebEngineView {background: #FFFFFF;}"));
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
    hide ();
    setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    horizontalScrollBar ()->setMaximumWidth (0);
    verticalScrollBar ()->setMaximumHeight (0);
    show ();
    webView_->show ();
  }
  else {
    setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy (Qt::ScrollBarAsNeeded);
    horizontalScrollBar ()->setMaximumWidth (999);
    verticalScrollBar ()->setMaximumHeight (999);
    if (webView_) {
      webView_->hide ();
    }
  }
}

void MarkviewWidget::init () {
  if (currentAdapter_) {
    Q_ASSERT (webView_);
    Q_ASSERT (textDocument ());
    QFileInfo info (textDocument ()->filePath ().toString ());
    currentAdapter_->initView (textDocument ()->plainText (), info.absolutePath (), webView_);
  }
}

void MarkviewWidget::update () {
  if (currentAdapter_) {
    Q_ASSERT (webView_);
    Q_ASSERT (textDocument ());
    QFileInfo info (textDocument ()->filePath ().toString ());
    currentAdapter_->updateView (textDocument ()->plainText (), info.absolutePath (), webView_);
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

void MarkviewWidget::addCustomEditor () {
  auto file = QFileDialog::getOpenFileName (nullptr, tr ("Select conversion script"),
                                            {}, "*.html");
  if (file.isEmpty () && QFile::exists (file)) {
    return;
  }
  auto name = QFileInfo (file).fileName ();
  if (adapters_.contains (name)) {
    return;
  }
  adapters_.insert (name, new AdapterBase ({}, file, true));
  adapterCombo_->addItem (name);
  adapterCombo_->setCurrentText (name);
}

void MarkviewWidget::removeCustomEditor () {
  if (QMessageBox::question (this, {}, tr ("Remove %1?").arg (currentAdapterName_))
      == QMessageBox::Yes) {
    adapters_.remove (currentAdapterName_);
    adapterCombo_->removeItem (adapterCombo_->currentIndex ());
  }
}
