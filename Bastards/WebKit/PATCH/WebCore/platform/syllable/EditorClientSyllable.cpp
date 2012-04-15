/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org> 
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.
 *
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
#include "EditorClientSyllable.h"

#include "Editor.h"
#include "FocusController.h"
#include "Frame.h"
#include "KeyboardCodes.h"
#include "KeyboardEvent.h"
#include "Document.h"
#include "Page.h"
#include "PlatformKeyboardEvent.h"
#include "NotImplemented.h"

#include <stdio.h>

namespace WebCore {

bool EditorClientSyllable::shouldDeleteRange(Range*)
{
    return true;
}

bool EditorClientSyllable::shouldShowDeleteInterface(HTMLElement*)
{
    return false;
}

bool EditorClientSyllable::isContinuousSpellCheckingEnabled()
{
    notImplemented();
    return false;
}

bool EditorClientSyllable::isGrammarCheckingEnabled()
{
    notImplemented();
    return false;
}

int EditorClientSyllable::spellCheckerDocumentTag()
{
    notImplemented();
    return 0;
}

bool EditorClientSyllable::shouldBeginEditing(WebCore::Range*)
{
    return true;
}

bool EditorClientSyllable::shouldEndEditing(WebCore::Range*)
{
    return true;
}

bool EditorClientSyllable::shouldInsertText(String, Range*, EditorInsertAction)
{
    return true;
}

bool EditorClientSyllable::shouldChangeSelectedRange(Range* fromRange, Range* toRange, EAffinity, bool stillSelecting)
{
    return true;
}

bool EditorClientSyllable::shouldApplyStyle(WebCore::CSSStyleDeclaration*,
                                      WebCore::Range*)
{
    return true;
}

bool EditorClientSyllable::shouldMoveRangeAfterDelete(Range*, Range*)
{
	return true;
}

void EditorClientSyllable::didBeginEditing()
{
    m_editing = true;
}

void EditorClientSyllable::respondToChangedContents()
{
    //m_page->d->modified = true;
}

void EditorClientSyllable::respondToChangedSelection()
{
    notImplemented();
}

void EditorClientSyllable::didEndEditing()
{
    m_editing = false;
}

void EditorClientSyllable::didWriteSelectionToPasteboard()
{
    notImplemented();
}

void EditorClientSyllable::didSetSelectionTypesForPasteboard()
{
    notImplemented();
}

bool EditorClientSyllable::selectWordBeforeMenuEvent()
{
    notImplemented();
    return false;
}

bool EditorClientSyllable::isEditable()
{
    // FIXME: should be controllable by a setting in QWebPage
    return false;
}

void EditorClientSyllable::registerCommandForUndo(WTF::PassRefPtr<WebCore::EditCommand> cmd)
{
}

void EditorClientSyllable::registerCommandForRedo(WTF::PassRefPtr<WebCore::EditCommand>)
{
}

void EditorClientSyllable::clearUndoRedoOperations()
{
    //return m_page->undoStack()->clear();
}

bool EditorClientSyllable::canUndo() const
{
    //return m_page->undoStack()->canUndo();
}

bool EditorClientSyllable::canRedo() const
{
    //return m_page->undoStack()->canRedo();
}

void EditorClientSyllable::undo()
{
    m_inUndoRedo = true;
    //m_page->undoStack()->undo();
    m_inUndoRedo = false;
}

void EditorClientSyllable::redo()
{
    m_inUndoRedo = true;
    //m_page->undoStack()->redo();
    m_inUndoRedo = false;
}

bool EditorClientSyllable::shouldInsertNode(Node*, Range*, EditorInsertAction)
{
    return true;
}

void EditorClientSyllable::pageDestroyed()
{
    notImplemented();
}

bool EditorClientSyllable::smartInsertDeleteEnabled()
{
    notImplemented();
    return false;
}

void EditorClientSyllable::toggleContinuousSpellChecking()
{
    notImplemented();
}

void EditorClientSyllable::toggleGrammarChecking()
{
    notImplemented();
}

void EditorClientSyllable::handleKeyboardEvent(KeyboardEvent* event)
{
    Frame* frame = m_page->focusController()->focusedOrMainFrame();
    if (!frame || !frame->document()->focusedNode())
        return;

    const PlatformKeyboardEvent* kevent = event->keyEvent();
    if (kevent->type()==PlatformKeyboardEvent::KeyUp)
        return;

    Node* start = frame->selectionController()->start().node();
    if (!start)
        return;

    // FIXME: refactor all of this to use Actions or something like them
    if (start->isContentEditable()) {
        switch(kevent->windowsVirtualKeyCode()) {
            case VK_RETURN:
                frame->editor()->command("InsertLineBreak").execute();
                break;
            case VK_BACK:
                frame->editor()->deleteWithDirection(SelectionController::BACKWARD,
                        CharacterGranularity, false, true);
                break;
            case VK_DELETE:
                frame->editor()->deleteWithDirection(SelectionController::FORWARD,
                        CharacterGranularity, false, true);
                break;
            case VK_LEFT:
                if (kevent->shiftKey())
                    frame->editor()->command("MoveLeftAndModifySelection").execute();
                else frame->editor()->command("MoveLeft").execute();
                break;
            case VK_RIGHT:
                if (kevent->shiftKey())
                    frame->editor()->command("MoveRightAndModifySelection").execute();
                else frame->editor()->command("MoveRight").execute();
                break;
            case VK_UP:
                if (kevent->shiftKey())
                    frame->editor()->command("MoveUpAndModifySelection").execute();
                else frame->editor()->command("MoveUp").execute();
                break;
            case VK_DOWN:
                if (kevent->shiftKey())
                    frame->editor()->command("MoveDownAndModifySelection").execute();
                else frame->editor()->command("MoveDown").execute();
                break;
            case VK_PRIOR:  // PageUp
                frame->editor()->command("MoveUpByPageAndModifyCaret").execute();
                break;
            case VK_NEXT:  // PageDown
                frame->editor()->command("MoveDownByPageAndModifyCaret").execute();
                break;
            case VK_HOME:
                if (kevent->shiftKey())
                  frame->editor()->command("MoveToBeginningOfLineAndModifySelection").execute();
                else
                  frame->editor()->command("MoveToBeginningOfLine").execute();
                break;
            case VK_END:
                if (kevent->shiftKey())
                  frame->editor()->command("MoveToEndOfLineAndModifySelection").execute();
                else
                  frame->editor()->command("MoveToEndOfLine").execute();
                break;
            /* Swallow these */
            case VK_ESCAPE:
            case VK_INSERT:
            	break;
            default:
                if (!kevent->ctrlKey() && !kevent->text().isEmpty()) {
                    frame->editor()->insertText(kevent->text(), event);
                } else if (kevent->ctrlKey()) {
                    switch (kevent->windowsVirtualKeyCode()) {
                        case VK_A:
                            frame->editor()->command("SelectAll").execute();
                            break;
                        case VK_B:
                            frame->editor()->command("ToggleBold").execute();
                            break;
                        case VK_C:
                            frame->editor()->command("Copy").execute();
                            break;
                        case VK_I:
                            frame->editor()->command("ToggleItalic").execute();
                            break;
                        case VK_V:
                            frame->editor()->command("Paste").execute();
                            break;
                        case VK_X:
                            frame->editor()->command("Cut").execute();
                            break;
                        case VK_Y:
                            frame->editor()->command("Redo").execute();
                            break;
                        case VK_Z:
                            frame->editor()->command("Undo").execute();
                            break;
                        default:
                            return;
                    }
                } else return;
        }
    } else {
        switch (kevent->windowsVirtualKeyCode()) {
            case VK_UP:
                frame->editor()->command("MoveUp").execute();
                break;
            case VK_DOWN:
                frame->editor()->command("MoveDown").execute();
                break;
            case VK_PRIOR:  // PageUp
                frame->editor()->command("MoveUpByPageAndModifyCaret").execute();
                break;
            case VK_NEXT:  // PageDown
                frame->editor()->command("MoveDownByPageAndModifyCaret").execute();
                break;
            case VK_HOME:
                if (kevent->ctrlKey())
                    frame->editor()->command("MoveToBeginningOfDocument").execute();
                break;
            case VK_END:
                if (kevent->ctrlKey())
                    frame->editor()->command("MoveToEndOfDocument").execute();
                break;
            default:
                if (kevent->ctrlKey()) {
                    switch(kevent->windowsVirtualKeyCode()) {
                        case VK_A:
                            frame->editor()->command("SelectAll").execute();
                            break;
                        case VK_C: case VK_X:
                            frame->editor()->command("Copy").execute();
                            break;
                        default:
                            return;
                    }
                } else return;
        }
    }
    event->setDefaultHandled();
	event->stopPropagation();
}

void EditorClientSyllable::handleInputMethodKeydown(KeyboardEvent*)
{
}

EditorClientSyllable::EditorClientSyllable() :
    m_editing(false), m_inUndoRedo(false)
{
}

void EditorClientSyllable::setPage( Page* page )
{
	m_page = page;
}

void EditorClientSyllable::textFieldDidBeginEditing(Element*)
{
    m_editing = true;
}

void EditorClientSyllable::textFieldDidEndEditing(Element*)
{
    m_editing = false;
}

void EditorClientSyllable::textDidChangeInTextField(Element*)
{
}

bool EditorClientSyllable::doTextFieldCommandFromEvent(Element*, KeyboardEvent*)
{
    return false;
}

void EditorClientSyllable::textWillBeDeletedInTextField(Element*)
{
}

void EditorClientSyllable::textDidChangeInTextArea(Element*)
{
}


void EditorClientSyllable::ignoreWordInSpellDocument(const String&)
{
    notImplemented();
}

void EditorClientSyllable::learnWord(const String&)
{
    notImplemented();
}

void EditorClientSyllable::checkSpellingOfString(const UChar*, int, int*, int*)
{
    notImplemented();
}

void EditorClientSyllable::checkGrammarOfString(const UChar*, int, Vector<GrammarDetail>&, int*, int*)
{
    notImplemented();
}

void EditorClientSyllable::updateSpellingUIWithGrammarString(const String&, const GrammarDetail&)
{
    notImplemented();
}

void EditorClientSyllable::updateSpellingUIWithMisspelledWord(const String&)
{
    notImplemented();
}

void EditorClientSyllable::showSpellingUI(bool)
{
    notImplemented();
}

bool EditorClientSyllable::spellingUIIsShowing()
{
    notImplemented();
    return false;
}

void EditorClientSyllable::getGuessesForWord(const String&, Vector<String>&)
{
    notImplemented();
}

void EditorClientSyllable::setInputMethodState(bool enabled)
{
    notImplemented();
}

bool EditorClientSyllable::isEditing() const
{
    return m_editing;
}

}

// vim: ts=4 sw=4 et


