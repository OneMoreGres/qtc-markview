#include "MarkviewWidget.h"
#include "AdapterBase.h"

#include <QComboBox>
#include <QWebView>
#include <QVBoxLayout>

using namespace QtcMarkview::Internal;

MarkviewWidget::MarkviewWidget (const Adapters &adapters)
  : webView_ (nullptr), adapters_ (adapters), currentAdapter_ (nullptr) {

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
           this, &MarkviewWidget::reload);
}

void MarkviewWidget::currentAdapterChanged (const QString &newAdapterName) {
  currentAdapter_ = adapters_.value (newAdapterName, nullptr);
  changeView ();
  reload ();
}

void MarkviewWidget::changeView () {
  // Create webView and place on top of base editor (not viewport).
  if (currentAdapter_ && !webView_) {
    webView_ = new QWebView (this);
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

void MarkviewWidget::reload () {
  if (currentAdapter_) {
    Q_ASSERT (webView_);
    Q_ASSERT (textDocument ());
    webView_->setHtml (currentAdapter_->toHtml (textDocument ()->plainText ()));
  }
}
