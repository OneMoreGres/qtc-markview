/*
 * Copyright (c) 2014-2018 Patrizio Bekerle -- http://www.bekerle.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "markdowntextedit.h"
#include <QKeyEvent>
#include <QGuiApplication>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QDir>
#include <QDesktopServices>
#include <QLayout>
#include <QTimer>
#include <QSettings>
#include <QTextBlock>
#include <QPainter>
#include <QScrollBar>

#include <texteditor/textdocument.h>


QMarkdownTextEdit::QMarkdownTextEdit (QWidget *parent)
  : TextEditor::TextEditorWidget (parent) {
  installEventFilter (this);
  viewport ()->installEventFilter (this);
  _autoTextOptions = AutoTextOption::None;
  _openingCharacters = QStringList () << "(" << "[" << "{" << "<" << "*"
                                      << "\"" << "'" << "_" << "~";
  _closingCharacters = QStringList () << ")" << "]" << "}" << ">" << "*"
                                      << "\"" << "'" << "_" << "~";

  QFont font = this->font ();

  // set the tab stop to the width of 4 spaces in the editor
  const int tabStop = 4;
  QFontMetrics metrics (font);
  setTabStopWidth (tabStop * metrics.width (' '));

  // add shortcuts for duplicating text
  //    new QShortcut( QKeySequence( "Ctrl+D" ), this, SLOT( duplicateText() ) );
  //    new QShortcut( QKeySequence( "Ctrl+Alt+Down" ), this, SLOT( duplicateText() ) );

  // add a layout to the widget
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins (0, 0, 0, 0);
  layout->setMargin (0);
  layout->addStretch ();
  this->setLayout (layout);

  QObject::connect (this, SIGNAL (textChanged ()),
                    this, SLOT (adjustRightMargin ()));

  // workaround for disabled signals up initialization
  QTimer::singleShot (300, this, SLOT (adjustRightMargin ()));
}

/**
 * Leave a little space on the right side if the document is too long, so
 * that the search buttons don't get visually blocked by the scroll bar
 */
void QMarkdownTextEdit::adjustRightMargin () {
  QMargins margins = layout ()->contentsMargins ();
  int rightMargin = document ()->size ().height () >
                    viewport ()->size ().height () ? 24 : 0;
  margins.setRight (rightMargin);
  layout ()->setContentsMargins (margins);
}

bool QMarkdownTextEdit::eventFilter (QObject *obj, QEvent *event) {
  //qDebug() << event->type();
  if (event->type () == QEvent::HoverMove) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    QWidget *viewPort = this->viewport ();
    // toggle cursor when control key has been pressed or released
    viewPort->setCursor (mouseEvent->modifiers ().testFlag (
                           Qt::ControlModifier) ?
                         Qt::PointingHandCursor :
                         Qt::IBeamCursor);
  }
  else if (event->type () == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // set cursor to pointing hand if control key was pressed
    if (keyEvent->modifiers ().testFlag (Qt::ControlModifier)) {
      QWidget *viewPort = this->viewport ();
      viewPort->setCursor (Qt::PointingHandCursor);
    }

    // disallow keys if text edit hasn't focus
    if (!this->hasFocus ()) {
      return true;
    }

    if ((keyEvent->key () == Qt::Key_Tab) ||
        (keyEvent->key () == Qt::Key_Backtab)) {
      // handle entered tab and reverse tab keys
      return handleTabEntered (
        keyEvent->key () == Qt::Key_Backtab);
    }
    else if (keyEvent->key () == Qt::Key_Backspace) {
      return handleBracketRemoval ();
    }
    else if (keyEvent->key () == Qt::Key_Asterisk) {
      return handleBracketClosing (QString ("*"));
    }
    else if (keyEvent->key () == Qt::Key_QuoteDbl) {
      return handleBracketClosing ("\"");
      // apostrophe bracket closing is temporary disabled because
      // apostrophes are used in different contexts
      //        } else if (keyEvent->key() == Qt::Key_Apostrophe) {
      //            return handleBracketClosing("'");
      // underline bracket closing is temporary disabled because
      // underlines are used in different contexts
      //        } else if (keyEvent->key() == Qt::Key_Underscore) {
      //            return handleBracketClosing("_");
    }
    else if (keyEvent->key () == Qt::Key_AsciiTilde) {
      return handleBracketClosing ("~");
    }
    else if (keyEvent->key () == Qt::Key_ParenLeft) {
      return handleBracketClosing ("(", ")");
    }
    else if (keyEvent->key () == Qt::Key_BraceLeft) {
      return handleBracketClosing ("{", "}");
    }
    else if (keyEvent->key () == Qt::Key_BracketLeft) {
      return handleBracketClosing ("[", "]");
    }
    else if (keyEvent->key () == Qt::Key_Less) {
      return handleBracketClosing ("<", ">");
    }
    else if (keyEvent->key () == Qt::Key_ParenRight) {
      return bracketClosingCheck ("(", ")");
    }
    else if (keyEvent->key () == Qt::Key_BraceRight) {
      return bracketClosingCheck ("{", "}");
    }
    else if (keyEvent->key () == Qt::Key_BracketRight) {
      return bracketClosingCheck ("[", "]");
    }
    else if ((keyEvent->key () == Qt::Key_Down) &&
             keyEvent->modifiers ().testFlag (Qt::ControlModifier) &&
             keyEvent->modifiers ().testFlag (Qt::AltModifier)) {
      // duplicate text with `Ctrl + Alt + Down`
      duplicateText ();
      return true;
    }
    else if ((keyEvent->key () == Qt::Key_Down) &&
             keyEvent->modifiers ().testFlag (Qt::ControlModifier)) {
      // scroll the page down
      auto *scrollBar = verticalScrollBar ();
      scrollBar->setSliderPosition (scrollBar->sliderPosition () + 1);
      return true;
    }
    else if ((keyEvent->key () == Qt::Key_Up) &&
             keyEvent->modifiers ().testFlag (Qt::ControlModifier)) {
      // scroll the page up
      auto *scrollBar = verticalScrollBar ();
      scrollBar->setSliderPosition (scrollBar->sliderPosition () - 1);
      return true;
    }
    else if ((keyEvent->key () == Qt::Key_Down) &&
             keyEvent->modifiers ().testFlag (Qt::NoModifier)) {
      // if you are in the last line and press cursor down the cursor will
      // jump to the end of the line
      QTextCursor cursor = textCursor ();
      if (cursor.position () >= document ()->lastBlock ().position ()) {
        cursor.movePosition (QTextCursor::EndOfLine);
        setTextCursor (cursor);
      }
      return false;
    }
    else if ((keyEvent->key () == Qt::Key_Up) &&
             keyEvent->modifiers ().testFlag (Qt::NoModifier)) {
      // if you are in the first line and press cursor up the cursor will
      // jump to the start of the line
      QTextCursor cursor = textCursor ();
      QTextBlock block = document ()->firstBlock ();
      int endOfFirstLinePos = block.position () + block.length ();

      if (cursor.position () <= endOfFirstLinePos) {
        cursor.movePosition (QTextCursor::StartOfLine);
        setTextCursor (cursor);
      }
      return false;
    }
    else if (keyEvent->key () == Qt::Key_Return) {
      return handleReturnEntered ();
    }

    return false;
  }
  else if (event->type () == QEvent::KeyRelease) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // reset cursor if control key was released
    if (keyEvent->key () == Qt::Key_Control) {
      resetMouseCursor ();
    }

    return false;
  }
  else if (event->type () == QEvent::MouseButtonRelease) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    // track `Ctrl + Click` in the text edit
    if ((obj == this->viewport ()) &&
        (mouseEvent->button () == Qt::LeftButton) &&
        (QGuiApplication::keyboardModifiers () == Qt::ExtraButton24)) {
      // open the link (if any) at the current position
      // in the noteTextEdit
      openLinkAtCursorPosition ();
      return true;
    }
  }

  return QPlainTextEdit::eventFilter (obj, event);
}

/**
 * Resets the cursor to Qt::IBeamCursor
 */
void QMarkdownTextEdit::resetMouseCursor () const {
  QWidget *viewPort = viewport ();
  viewPort->setCursor (Qt::IBeamCursor);
}

void QMarkdownTextEdit::finalizeInitialization () {
  setMarksVisible (true);
  setLineSeparatorsAllowed (true);

  //  setupGenericHighlighter ();

  if (_highlighter) {
    delete _highlighter;
  }
  _highlighter = new MarkdownHighlighter;

  textDocument ()->setSyntaxHighlighter (_highlighter);

  setCodeFoldingSupported (false);
}

/**
 * Resets the cursor to Qt::IBeamCursor if the widget looses the focus
 */
void QMarkdownTextEdit::focusOutEvent (QFocusEvent *event) {
  resetMouseCursor ();
  QPlainTextEdit::focusOutEvent (event);
}

/**
 * Enters a closing character after an opening character if needed
 *
 * @param openingCharacter
 * @param closingCharacter
 * @return
 */
bool QMarkdownTextEdit::handleBracketClosing (QString openingCharacter,
                                              QString closingCharacter) {
  // check if bracket closing is enabled
  if (!(_autoTextOptions & AutoTextOption::BracketClosing)) {
    return false;
  }

  QTextCursor cursor = textCursor ();

  // get the current text from the block (inserted character not included)
  QString text = cursor.block ().text ();

  if (closingCharacter.isEmpty ()) {
    closingCharacter = openingCharacter;
  }

  QString selectedText = cursor.selectedText ();

  // When user currently has text selected, we prepend the openingCharacter
  // and append the closingCharacter. E.g. 'text' -> '(text)'. We keep the
  // current selectedText selected.
  //
  // TODO(sanderboom): how to make ctrl-z keep the selectedText selected?
  if (selectedText != "") {
    // Insert. The selectedText is overwritten.
    cursor.insertText (openingCharacter);
    cursor.insertText (selectedText);
    cursor.insertText (closingCharacter);

    // Re-select the selectedText.
    int selectionEnd = cursor.position () - 1;
    int selectionStart = selectionEnd - selectedText.length ();
    cursor.setPosition (selectionStart);
    cursor.setPosition (selectionEnd, QTextCursor::KeepAnchor);
    this->setTextCursor (cursor);

    return true;
  }
  else {
    // if not text was selected check if we are inside the text
    int positionInBlock = cursor.position () - cursor.block ().position ();

    // only allow the closing if the cursor was at the end of a block
    // we are making a special allowance for openingCharacter == *
    if ((positionInBlock != text.count ()) &&
        !((openingCharacter == "*") &&
          (positionInBlock == (text.count () - 1)))) {
      return false;
    }
  }


  // Remove whitespace at start of string (e.g. in multilevel-lists).
  text = text.remove (QRegExp ("^\\s+"));

  // Default positions to move the cursor back.
  int cursorSubtract = 1;

  // Special handling for `*` opening character, as this could be:
  // - start of a list (or sublist);
  // - start of a bold text;
  if (openingCharacter == "*") {
    // User wants: '*'.
    // This could be the start of a list, don't autocomplete.
    if (text == "") {
      return false;
    }
    // User wants: '**'.
    // Not the start of a list, probably bold text. We autocomplete with
    // extra closingCharacter and cursorSubtract to 'catchup'.
    else if (text == "*") {
      closingCharacter = "**";
      cursorSubtract = 2;
    }
    // User wants: '* *'.
    // We are in a list already, proceed as normal (autocomplete).
    else if (text == "* ") {
      // no-op.
    }
  }

  cursor.insertText (openingCharacter);
  cursor.insertText (closingCharacter);
  cursor.setPosition (cursor.position () - cursorSubtract);
  setTextCursor (cursor);
  return true;
}

/**
 * Checks if the closing character should be output or not
 *
 * @param openingCharacter
 * @param closingCharacter
 * @return
 */
bool QMarkdownTextEdit::bracketClosingCheck (QString openingCharacter,
                                             QString closingCharacter) {
  // check if bracket closing is enabled
  if (!(_autoTextOptions & AutoTextOption::BracketClosing)) {
    return false;
  }

  QTextCursor cursor = textCursor ();
  int positionInBlock = cursor.position () - cursor.block ().position ();

  // get the current text from the block
  QString text = cursor.block ().text ();
  int textLength = text.length ();

  // if we are at the end of the line we just want to enter the character
  if (positionInBlock >= textLength) {
    return false;
  }

  QString currentChar = text.at (positionInBlock);

  // if the current character is not the closing character we just want to
  // enter the character
  if (currentChar != closingCharacter) {
    return false;
  }

  QString leftText = text.left (positionInBlock);
  int openingCharacterCount = leftText.count (openingCharacter);
  int closingCharacterCount = leftText.count (closingCharacter);

  // if there were enough opening characters just enter the character
  if (openingCharacterCount < (closingCharacterCount + 1)) {
    return false;
  }

  // move the cursor to the right and don't enter the character
  cursor.movePosition (QTextCursor::Right);
  setTextCursor (cursor);
  return true;
}

/**
 * Handles removing of matching brackets and other markdown characters
 * Only works with backspace to remove text
 *
 * @return
 */
bool QMarkdownTextEdit::handleBracketRemoval () {
  // check if bracket removal is enabled
  if (!(_autoTextOptions & AutoTextOption::BracketRemoval)) {
    return false;
  }

  QTextCursor cursor = textCursor ();

  // return if some text was selected
  if (!cursor.selectedText ().isEmpty ()) {
    return false;
  }

  int position = cursor.position ();
  int positionInBlock = position - cursor.block ().position ();

  // return if backspace was pressed at the beginning of a block
  if (positionInBlock == 0) {
    return false;
  }

  // get the current text from the block
  QString text = cursor.block ().text ();
  QString charInFront = text.at (positionInBlock - 1);
  int openingCharacterIndex = _openingCharacters.indexOf (charInFront);

  // return if the character in front of the cursor is no opening character
  if (openingCharacterIndex == -1) {
    return false;
  }

  QString closingCharacter = _closingCharacters.at (openingCharacterIndex);

  // remove everything in front of the cursor
  text.remove (0, positionInBlock);
  int closingCharacterIndex = text.indexOf (closingCharacter);

  // return if no closing character was found in the text after the cursor
  if (closingCharacterIndex == -1) {
    return false;
  }

  // removing the closing character
  cursor.movePosition (QTextCursor::Right, QTextCursor::MoveAnchor,
                       closingCharacterIndex);
  cursor.movePosition (QTextCursor::Right, QTextCursor::KeepAnchor);
  cursor.removeSelectedText ();

  // moving the cursor back to the old position so the previous character
  // can be removed
  cursor.setPosition (position);
  setTextCursor (cursor);
  return false;
}

/**
 * Increases (or decreases) the indention of the selected text
 * (if there is a text selected) in the noteTextEdit
 * @return
 */
bool QMarkdownTextEdit::increaseSelectedTextIndention (bool reverse) {
  QTextCursor cursor = this->textCursor ();
  QString selectedText = cursor.selectedText ();

  if (selectedText != "") {
    // we need this strange newline character we are getting in the
    // selected text for newlines
    QString newLine = QString::fromUtf8 (QByteArray::fromHex ("e280a9"));
    QString newText;

    if (reverse) {
      // un-indent text

      // remove strange newline characters
      newText = selectedText.replace (
        QRegularExpression (newLine + "[\\t ]"), "\n");

      // remove leading \t or space
      newText.remove (QRegularExpression ("^[\\t ]"));
    }
    else {
      // indent text
      newText = selectedText.replace (newLine, "\n\t").prepend ("\t");

      // remove trailing \t
      newText.replace (QRegularExpression ("\\t$"), "");
    }

    // insert the new text
    cursor.insertText (newText);

    // update the selection to the new text
    cursor.setPosition (cursor.position () - newText.size (), QTextCursor::KeepAnchor);
    this->setTextCursor (cursor);

    return true;
  }
  else if (reverse) {
    // if nothing was selected but we want to reverse the indention check
    // if there is a \t in front or after the cursor and remove it if so
    int position = cursor.position ();

    if (!cursor.atStart ()) {
      // get character in front of cursor
      cursor.setPosition (position - 1, QTextCursor::KeepAnchor);
    }

    // check for \t or space in front of cursor
    QRegularExpression re ("[\\t ]");
    QRegularExpressionMatch match = re.match (cursor.selectedText ());

    if (!match.hasMatch ()) {
      // (select to) check for \t or space after the cursor
      cursor.setPosition (position);

      if (!cursor.atEnd ()) {
        cursor.setPosition (position + 1, QTextCursor::KeepAnchor);
      }
    }

    match = re.match (cursor.selectedText ());

    if (match.hasMatch ()) {
      cursor.removeSelectedText ();
    }

    return true;
  }

  return false;
}

/**
 * @brief Opens the link (if any) at the current cursor position
 */
bool QMarkdownTextEdit::openLinkAtCursorPosition () {
  QTextCursor cursor = this->textCursor ();
  int clickedPosition = cursor.position ();

  // select the text in the clicked block and find out on
  // which position we clicked
  cursor.movePosition (QTextCursor::StartOfBlock);
  int positionFromStart = clickedPosition - cursor.position ();
  cursor.movePosition (QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

  QString selectedText = cursor.selectedText ();

  // find out which url in the selected text was clicked
  QString urlString = getMarkdownUrlAtPosition (selectedText,
                                                positionFromStart);
  QUrl url = QUrl (urlString);
  bool isRelativeFileUrl = urlString.startsWith ("file://..");

  qDebug () << __func__ << " - 'emit urlClicked( urlString )': "
            << urlString;

  emit urlClicked (urlString);

  if ((url.isValid () && isValidUrl (urlString)) || isRelativeFileUrl) {
    // ignore some schemata
    if (!(_ignoredClickUrlSchemata.contains (url.scheme ()) ||
          isRelativeFileUrl)) {
      // open the url
      openUrl (urlString);
    }

    return true;
  }

  return false;
}

/**
 * Checks if urlString is a valid url
 *
 * @param urlString
 * @return
 */
bool QMarkdownTextEdit::isValidUrl (QString urlString) {
  QRegularExpressionMatch match =
    QRegularExpression ("^\\w+:\\/\\/.+").match (urlString);
  return match.hasMatch ();
}

/**
 * Handles clicked urls
 *
 * examples:
 * - <http://www.qownnotes.org> opens the webpage
 * - <file:///path/to/my/file/QOwnNotes.pdf> opens the file
 *   "/path/to/my/file/QOwnNotes.pdf" if the operating system supports that
 *  handler
 */
void QMarkdownTextEdit::openUrl (QString urlString) {
  qDebug () << "QMarkdownTextEdit " << __func__ << " - 'urlString': "
            << urlString;

  QDesktopServices::openUrl (QUrl (urlString));
}

/**
 * @brief Sets url schemata that will be ignored when clicked on
 * @param urlSchemes
 */
void QMarkdownTextEdit::setIgnoredClickUrlSchemata (
  QStringList ignoredUrlSchemata) {
  _ignoredClickUrlSchemata = ignoredUrlSchemata;
}

/**
 * @brief Returns a map of parsed markdown urls with their link texts as key
 *
 * @param text
 * @return parsed urls
 */
QMap<QString, QString> QMarkdownTextEdit::parseMarkdownUrlsFromText (
  QString text) {
  QMap<QString, QString> urlMap;
  QRegularExpression regex;
  QRegularExpressionMatchIterator iterator;

  // match urls like this: <http://mylink>
  //    re = QRegularExpression("(<(.+?:\\/\\/.+?)>)");
  regex = QRegularExpression ("(<(.+?)>)");
  iterator = regex.globalMatch (text);
  while (iterator.hasNext ()) {
    QRegularExpressionMatch match = iterator.next ();
    QString linkText = match.captured (1);
    QString url = match.captured (2);
    urlMap[linkText] = url;
  }

  // match urls like this: [this url](http://mylink)
  //    QRegularExpression re("(\\[.*?\\]\\((.+?:\\/\\/.+?)\\))");
  regex = QRegularExpression ("(\\[.*?\\]\\((.+?)\\))");
  iterator = regex.globalMatch (text);
  while (iterator.hasNext ()) {
    QRegularExpressionMatch match = iterator.next ();
    QString linkText = match.captured (1);
    QString url = match.captured (2);
    urlMap[linkText] = url;
  }

  // match urls like this: http://mylink
  regex = QRegularExpression ("\\b\\w+?:\\/\\/[^\\s]+[^\\s>\\)]");
  iterator = regex.globalMatch (text);
  while (iterator.hasNext ()) {
    QRegularExpressionMatch match = iterator.next ();
    QString url = match.captured (0);
    urlMap[url] = url;
  }

  // match reference urls like this: [this url][1] with this later:
  // [1]: http://domain
  regex = QRegularExpression ("\\[(.*?)\\]\\s?\\[(.+?)\\]");
  iterator = regex.globalMatch (text);
  while (iterator.hasNext ()) {
    QRegularExpressionMatch match = iterator.next ();
    QString linkText = match.captured (1);
    QString referenceId = match.captured (2);

    // search for the referenced url in the whole text edit
    //        QRegularExpression refRegExp(
    //                "\\[" + QRegularExpression::escape(referenceId) +
    //                        "\\]: (.+?:\\/\\/.+)");
    QRegularExpression refRegExp (
      "\\[" + QRegularExpression::escape (referenceId) + "\\]: (.+?)");
    QRegularExpressionMatch urlMatch = refRegExp.match (toPlainText ());

    if (urlMatch.hasMatch ()) {
      QString url = urlMatch.captured (1);
      urlMap[linkText] = url;
    }
  }

  return urlMap;
}

/**
 * @brief Returns the markdown url at position
 * @param text
 * @param position
 * @return url string
 */
QString QMarkdownTextEdit::getMarkdownUrlAtPosition (
  QString text, int position) {
  QString url;

  // get a map of parsed markdown urls with their link texts as key
  QMap<QString, QString> urlMap = parseMarkdownUrlsFromText (text);

  QMapIterator<QString, QString> iterator (urlMap);
  while (iterator.hasNext ()) {
    iterator.next ();
    QString linkText = iterator.key ();
    QString urlString = iterator.value ();

    int foundPositionStart = text.indexOf (linkText);

    if (foundPositionStart >= 0) {
      // calculate end position of found linkText
      int foundPositionEnd = foundPositionStart + linkText.size ();

      // check if position is in found string range
      if ((position >= foundPositionStart) &&
          (position <= foundPositionEnd)) {
        url = urlString;
      }
    }
  }

  return url;
}

/**
 * @brief Duplicates the text in the text edit
 */
void QMarkdownTextEdit::duplicateText () {
  QTextCursor cursor = this->textCursor ();
  QString selectedText = cursor.selectedText ();

  // duplicate line if no text was selected
  if (selectedText == "") {
    int position = cursor.position ();

    // select the whole line
    cursor.movePosition (QTextCursor::StartOfLine);
    cursor.movePosition (QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    int positionDiff = cursor.position () - position;
    selectedText = "\n" + cursor.selectedText ();

    // insert text with new line at end of the selected line
    cursor.setPosition (cursor.selectionEnd ());
    cursor.insertText (selectedText);

    // set the position to same position it was in the duplicated line
    cursor.setPosition (cursor.position () - positionDiff);
  }
  else {
    // duplicate selected text
    cursor.setPosition (cursor.selectionEnd ());
    int selectionStart = cursor.position ();

    // insert selected text
    cursor.insertText (selectedText);
    int selectionEnd = cursor.position ();

    // select the inserted text
    cursor.setPosition (selectionStart);
    cursor.setPosition (selectionEnd, QTextCursor::KeepAnchor);
  }

  this->setTextCursor (cursor);
}

/**
 * Handles an entered return key
 */
bool QMarkdownTextEdit::handleReturnEntered () {
  QTextCursor cursor = this->textCursor ();
  int position = cursor.position ();

  cursor.movePosition (QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
  QString currentLineText = cursor.selectedText ();

  // if return is pressed and there is just a list symbol then we want to
  // remove the list symbol
  // Valid listCharacters: '+ ', '-' , '* ', '+ [ ] ', '+ [x] ', '- [ ] ', '- [x] ', '* [ ] ', '* [x] '.
  QRegularExpression regex ("^(\\s*)([+|\\-|\\*] \\[(x| )\\]|[+\\-\\*])(\\s+)$");
  QRegularExpressionMatchIterator iterator = regex.globalMatch (currentLineText);
  if (iterator.hasNext ()) {
    cursor.removeSelectedText ();
    return true;
  }

  // Check if we are in a list.
  // We are in a list when we have '* ', '- ' or '+ ', possibly with preceding
  // whitespace. If e.g. user has entered '**text**' and pressed enter - we
  // don't want do anymore list-stuff.
  QChar char0 = currentLineText.trimmed ()[0];
  QChar char1 = currentLineText.trimmed ()[1];
  bool inList = ((char0 == '*' || char0 == '-' || char0 == '+') && char1 == ' ');

  if (inList) {
    // if the current line starts with a list character (possibly after
    // whitespaces) add the whitespaces at the next line too
    // Valid listCharacters: '+ ', '-' , '* ', '+ [ ] ', '+ [x] ', '- [ ] ', '- [x] ', '* [ ] ', '* [x] '.
    regex = QRegularExpression ("^(\\s*)([+|\\-|\\*] \\[(x| )\\]|[+\\-\\*])(\\s+)");
    iterator = regex.globalMatch (currentLineText);
    if (iterator.hasNext ()) {
      QRegularExpressionMatch match = iterator.next ();
      QString whitespaces = match.captured (1);
      QString listCharacter = match.captured (2);
      QString whitespaceCharacter = match.captured (4);

      cursor.setPosition (position);
      cursor.insertText ("\n" + whitespaces + listCharacter + whitespaceCharacter);

      // scroll to the cursor if we are at the bottom of the document
      ensureCursorVisible ();
      return true;
    }
  }

  return false;
}

/**
 * Handles entered tab or reverse tab keys
 */
bool QMarkdownTextEdit::handleTabEntered (bool reverse) {
  QTextCursor cursor = this->textCursor ();

  // only check for lists if we haven't a text selected
  if (cursor.selectedText ().isEmpty ()) {
    cursor.movePosition (QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    QString currentLineText = cursor.selectedText ();

    // check if we want to indent or un-indent a list
    // Valid listCharacters: '+ ', '-' , '* ', '+ [ ] ', '+ [x] ', '- [ ] ', '- [x] ', '* [ ] ', '* [x] '.
    QRegularExpression re ("^(\\s*)([+|\\-|\\*] \\[(x| )\\]|[+\\-\\*])(\\s+)$");
    QRegularExpressionMatchIterator i = re.globalMatch (currentLineText);

    if (i.hasNext ()) {
      QRegularExpressionMatch match = i.next ();
      QString whitespaces = match.captured (1);
      QString listCharacter = match.captured (2);
      QString whitespaceCharacter = match.captured (4);

      // add or remove one tabulator key
      if (reverse) {
        whitespaces.chop (1);
      }
      else {
        whitespaces += "\t";
      }

      cursor.insertText (whitespaces + listCharacter + whitespaceCharacter);
      return true;
    }
  }

  // check if we want to intent the whole text
  return increaseSelectedTextIndention (reverse);
}

/**
 * Sets the auto text options
 */
void QMarkdownTextEdit::setAutoTextOptions (AutoTextOptions options) {
  _autoTextOptions = options;
}

/**
 * Overrides QPlainTextEdit::paintEvent to fix the RTL bug of QPlainTextEdit
 *
 * @param e
 */
void QMarkdownTextEdit::paintEvent (QPaintEvent *e) {
  QTextBlock block = firstVisibleBlock ();

  while (block.isValid ()) {
    QTextLayout *layout = block.layout ();

    // this fixes the RTL bug of QPlainTextEdit
    // https://bugreports.qt.io/browse/QTBUG-7516
    if (block.text ().isRightToLeft ()) {
      QTextOption opt = document ()->defaultTextOption ();
      opt = QTextOption (Qt::AlignRight);
      opt.setTextDirection (Qt::RightToLeft);
      layout->setTextOption (opt);
    }

    block = block.next ();
  }

  QPlainTextEdit::paintEvent (e);
}
