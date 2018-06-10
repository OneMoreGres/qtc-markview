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

#pragma once

#include <QPlainTextEdit>
#include <QEvent>
#include "markdownhighlighter.h"

#include <texteditor/texteditor.h>


class QMarkdownTextEdit : public TextEditor::TextEditorWidget {
  Q_OBJECT

  public:
    enum AutoTextOption {
      None = 0x0000,

      // inserts closing characters for brackets and markdown characters
      BracketClosing = 0x0001,

      // removes matching brackets and markdown characters
      BracketRemoval = 0x0002
    };

    Q_DECLARE_FLAGS (AutoTextOptions, AutoTextOption)

    explicit QMarkdownTextEdit (QWidget *parent = nullptr);
    void setIgnoredClickUrlSchemata (QStringList ignoredUrlSchemata);
    virtual void openUrl (QString urlString);
    QString getMarkdownUrlAtPosition (QString text, int position);
    void setAutoTextOptions (AutoTextOptions options);
    static bool isValidUrl (QString urlString);
    void resetMouseCursor () const;

    void finalizeInitialization ();

  public slots:
    void duplicateText ();
    void adjustRightMargin ();
    bool openLinkAtCursorPosition ();
    bool handleBracketRemoval ();

  protected:
    QPointer<MarkdownHighlighter> _highlighter;
    QStringList _ignoredClickUrlSchemata;
    QWidget *_searchFrame;
    AutoTextOptions _autoTextOptions;
    QStringList _openingCharacters;
    QStringList _closingCharacters;

    bool eventFilter (QObject *obj, QEvent *event);
    bool increaseSelectedTextIndention (bool reverse);
    bool handleTabEntered (bool reverse);
    QMap<QString, QString> parseMarkdownUrlsFromText (QString text);
    bool handleReturnEntered ();
    bool handleBracketClosing (QString openingCharacter,
                               QString closingCharacter = {});
    bool bracketClosingCheck (QString openingCharacter,
                              QString closingCharacter);
    void focusOutEvent (QFocusEvent *event);
    void paintEvent (QPaintEvent *e);

  signals:
    void urlClicked (QString url);
};
