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
 * QPlainTextEdit markdown highlighter
 */


#pragma once

#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

#include <texteditor/syntaxhighlighter.h>

QT_BEGIN_NAMESPACE
class QTextDocument;

QT_END_NAMESPACE

class MarkdownHighlighter : public TextEditor::SyntaxHighlighter {
  Q_OBJECT

  public:
    enum HighlightingOption {
      None = 0,
      FullyHighlightedBlockQuote = 0x01
    };
    Q_DECLARE_FLAGS (HighlightingOptions, HighlightingOption);

    MarkdownHighlighter (QTextDocument *parent = 0,
                         HighlightingOptions highlightingOptions =
                           HighlightingOption::None);

    // we use some predefined numbers here to be compatible with
    // the peg-markdown parser
    enum HighlighterState {
      NoState = -1,
      Link = 0,
      Image = 3,
      CodeBlock,
      Italic = 7,
      Bold,
      List,
      Comment = 11,
      H1,
      H2,
      H3,
      H4,
      H5,
      H6,
      BlockQuote,
      HorizontalRuler = 21,
      Table,
      InlineCodeBlock,
      MaskedSyntax,
      CurrentLineBackgroundColor,
      BrokenLink,

      // internal
      CodeBlockEnd = 100,
      HeadlineEnd
    };

    //    enum BlockState {
    //        NoBlockState = 0,
    //        H1,
    //        H2,
    //        H3,
    //        Table,
    //        CodeBlock,
    //        CodeBlockEnd
    //    };

    void setTextFormats (QHash<HighlighterState, QTextCharFormat> formats);
    void setTextFormat (HighlighterState state, QTextCharFormat format);
    void clearDirtyBlocks ();
    void setHighlightingOptions (HighlightingOptions options);
    void initHighlightingRules ();

  signals:
    void highlightingFinished ();

  protected slots:
    void timerTick ();

  protected:
    struct HighlightingRule {
      QRegularExpression pattern;
      HighlighterState state;
      int capturingGroup;
      int maskedGroup;
      bool useStateAsCurrentBlockState;
      bool disableIfCurrentStateIsSet;
    };

    void highlightBlock (const QString &text) Q_DECL_OVERRIDE;

    void initTextFormats (int defaultFontSize = 12);

    void highlightMarkdown (QString text);

    void highlightHeadline (QString text);

    void highlightAdditionalRules (QVector<HighlightingRule> &rules,
                                   QString text);

    void highlightCodeBlock (QString text);

    void highlightCommentBlock (QString text);

    void addDirtyBlock (QTextBlock block);

    void reHighlightDirtyBlocks ();

    QVector<HighlightingRule> _highlightingRulesPre;
    QVector<HighlightingRule> _highlightingRulesAfter;
    QVector<QTextBlock> _dirtyTextBlocks;
    QHash<HighlighterState, QTextCharFormat> _formats;
    QTimer *_timer;
    bool _highlightingFinished;
    HighlightingOptions _highlightingOptions;

    void setCurrentBlockMargin (HighlighterState state);
};
