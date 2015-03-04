#include "AdapterMarkdown.h"

using namespace QtcMarkview::Internal;

AdapterMarkdown::AdapterMarkdown ()
  : AdapterBase () {

}

AdapterMarkdown::~AdapterMarkdown () {

}

QString AdapterMarkdown::toHtml (const QString &plainText) const {
  return plainText; //TODO
}
