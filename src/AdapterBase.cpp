#include "AdapterBase.h"

using namespace QtcMarkview::Internal;

AdapterBase::AdapterBase () {

}

AdapterBase::~AdapterBase () {

}

const QString &AdapterBase::helpMessage () const {
  return helpMessage_;
}

bool AdapterBase::isHelpAvailable () const {
  return !helpMessage_.isEmpty ();
}
