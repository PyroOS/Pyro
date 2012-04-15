/*
 * Copyright (C) 2007 Kevin Ollivier
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "PlatformString.h"

namespace WebCore {

String submitButtonDefaultLabel() 
{ 
    return String("Submit"); 
}

String inputElementAltText() 
{ 
    return String(); 
}

String resetButtonDefaultLabel() 
{ 
    return String("Reset"); 
}

String defaultLanguage() 
{ 
    return String("en"); 
}

String searchableIndexIntroduction() 
{ 
    return String("Searchable Index"); 
}

String fileButtonChooseFileLabel() 
{ 
    return String("Choose File"); 
}

String fileButtonNoFileSelectedLabel() 
{ 
    return String("No file selected"); 
}

String contextMenuItemTagOpenLinkInNewWindow() 
{ 
    return String( "Open in new window" ); 
}

String contextMenuItemTagDownloadLinkToDisk() 
{ 
    return String( "Download link to disk" ); 
}

String contextMenuItemTagCopyLinkToClipboard() 
{ 
    return String( "Copy link to clipboard" ); 
}

String contextMenuItemTagOpenImageInNewWindow() 
{ 
    return String( "Open image in new window" ); 
}

String contextMenuItemTagDownloadImageToDisk() 
{ 
    return String( "Download image to disk" ); 
}

String contextMenuItemTagCopyImageToClipboard() 
{ 
    return String( "Copy image to clipboard" ); 
}

String contextMenuItemTagOpenFrameInNewWindow() 
{ 
    return String( "Open frame in new window" ); 
}

String contextMenuItemTagCopy() 
{
    return String( "Copy" );
}

String contextMenuItemTagGoBack() 
{
    return String( "Go back" );
}

String contextMenuItemTagGoForward() 
{
    return String( "Go forward" );
}

String contextMenuItemTagStop() 
{
    return String( "Stop" );
}

String contextMenuItemTagReload() 
{
    return String( "Reload" );
}

String contextMenuItemTagCut() 
{
    return String( "Cut" );
}

String contextMenuItemTagPaste() 
{
    return String( "Paste" );
}

String contextMenuItemTagNoGuessesFound() 
{
    return String( "No guesses found" );
}

String contextMenuItemTagIgnoreSpelling() 
{
    return String( "Ignore spelling" );
}

String contextMenuItemTagLearnSpelling() 
{
    return String( "Learn spelling" );
}

String contextMenuItemTagSearchWeb() 
{
    return String( "Search web" );
}

String contextMenuItemTagLookUpInDictionary() 
{
    return String( "Lookup in dictionary" );
}

String contextMenuItemTagOpenLink() 
{
    return String( "Open link" );
}

String contextMenuItemTagIgnoreGrammar() 
{
    return String( "Ignore grammar" );
}

String contextMenuItemTagSpellingMenu() 
{
    return String( "Spelling" );
}

String contextMenuItemTagShowSpellingPanel(bool show) 
{
    return String( "Show spelling panel" );
}

String contextMenuItemTagCheckSpelling() 
{
    return String( "Check spelling" );
}

String contextMenuItemTagCheckSpellingWhileTyping() 
{
    return String( "Check spelling while typing" );
}

String contextMenuItemTagCheckGrammarWithSpelling() 
{
    return String( "Check for grammar with spelling" );
}

String contextMenuItemTagFontMenu() 
{
    return String( "Font" );
}

String contextMenuItemTagBold() 
{
    return String( "Bold" );
}

String contextMenuItemTagItalic() 
{
    return String( "Italic" );
}

String contextMenuItemTagUnderline() 
{
    return String( "Underline" );
}

String contextMenuItemTagOutline() 
{
    return String( "Outline" );
}

String contextMenuItemTagWritingDirectionMenu() 
{
    return String( "Direction" );
}

String contextMenuItemTagDefaultDirection() 
{
    return String( "Default" );
}

String contextMenuItemTagLeftToRight() 
{
    return String( "Left to right" );
}

String contextMenuItemTagRightToLeft() 
{
    return String( "Right to left" );
}

String searchMenuNoRecentSearchesText() 
{
    return String("No recent searches");
}

String searchMenuRecentSearchesText() 
{
    return String("Recent searches");
}

String searchMenuClearRecentSearchesText() 
{
    return String("Clear recent searches");
}

String contextMenuItemTagInspectElement() 
{
    return String("Inspect Element");
}

String unknownFileSizeText() 
{
    return String("Unknown");
}

} // namespace WebCore
