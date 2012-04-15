/*
 * Copyright (C) 2006 Don Gibson <dgibson77@gmail.com>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2007 Trolltech ASA
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
#include "FrameLoaderClientSyllable.h"
#include "DocumentLoader.h"
#include "FrameLoader.h"
#include "Frame.h"
#include "PlatformString.h"
#include "ResourceRequest.h"
#include "CString.h"
#include "NotImplemented.h"
#include "SyllableDebug.h"

#include <stdio.h>
#include <util/string.h>

namespace WebCore {

FrameLoaderClientSyllable::FrameLoaderClientSyllable()
    : m_frame(0)
    , m_firstData(false)
{
}

Frame* FrameLoaderClientSyllable::frame()
{ 
    return static_cast<Frame*>(m_frame);
}

String FrameLoaderClientSyllable::userAgent(const KURL&)
{
    return "Mozilla/5.0 (compatible; U; WebView 0.1; Pyro) AppleWebKit/420+ (KHTML, like Gecko)";
}

WTF::PassRefPtr<WebCore::DocumentLoader> FrameLoaderClientSyllable::createDocumentLoader(const WebCore::ResourceRequest& request, const SubstituteData& substituteData)
{
    RefPtr<DocumentLoader> loader = new DocumentLoader(request, substituteData);
    return loader.release();
}

void FrameLoaderClientSyllable::dispatchWillSubmitForm(FramePolicyFunction policyFunction,  PassRefPtr<FormState>)
{
    // FIXME: This is surely too simple
    ASSERT(frame() && policyFunction);
    if (!frame() || !policyFunction)
        return;
    (frame()->loader()->*policyFunction)(PolicyUse);
}


void FrameLoaderClientSyllable::committedLoad(DocumentLoader* loader, const char* data, int length)
{
    if (!frame())
        return;
    FrameLoader *fl = loader->frameLoader();
    fl->setEncoding(m_response.textEncodingName(), false);
    fl->addData(data, length);
}

void FrameLoaderClientSyllable::dispatchDidReceiveAuthenticationChallenge(DocumentLoader*, unsigned long  identifier, const AuthenticationChallenge&)
{
    notImplemented();
}

void FrameLoaderClientSyllable::dispatchDidCancelAuthenticationChallenge(DocumentLoader*, unsigned long  identifier, const AuthenticationChallenge&)
{
    notImplemented();
}

void FrameLoaderClientSyllable::dispatchWillSendRequest(DocumentLoader*, unsigned long , ResourceRequest&, const ResourceResponse&)
{
    notImplemented();
}

void FrameLoaderClientSyllable::assignIdentifierToInitialRequest(unsigned long identifier, DocumentLoader*, const ResourceRequest&)
{
    notImplemented();   
}

void FrameLoaderClientSyllable::postProgressStartedNotification()
{
    // no progress notification for now
}

void FrameLoaderClientSyllable::postProgressEstimateChangedNotification()
{
    // no progress notification for now    
}

void FrameLoaderClientSyllable::postProgressFinishedNotification()
{
    // no progress notification for now
}

void FrameLoaderClientSyllable::frameLoaderDestroyed()
{
    m_frame = 0;
    delete this;
}

void FrameLoaderClientSyllable::dispatchDidReceiveResponse(DocumentLoader*, unsigned long, const ResourceResponse& response)
{
	DEBUG("DID RECEIVE RESPONSE!\n");
    m_response = response;
    m_firstData = true;
}

void FrameLoaderClientSyllable::dispatchDecidePolicyForMIMEType(FramePolicyFunction policyFunction, const String&, const ResourceRequest&)
{
	notImplemented();
    // FIXME: we need to call directly here (comment copied from Qt version)
    ASSERT(frame() && policyFunction);
    if (!frame() || !policyFunction)
        return;
    (frame()->loader()->*policyFunction)(PolicyUse);
}

void FrameLoaderClientSyllable::dispatchDecidePolicyForNewWindowAction(FramePolicyFunction policyFunction, const NavigationAction&, const ResourceRequest&, const String&)
{
	notImplemented();
    ASSERT(frame() && policyFunction);
    if (!frame() || !policyFunction)
        return;
    // FIXME: I think Qt version marshals this to another thread so when we
    // have multi-threaded download, we might need to do the same
    (frame()->loader()->*policyFunction)(PolicyUse);
}

void FrameLoaderClientSyllable::dispatchDecidePolicyForNavigationAction(FramePolicyFunction policyFunction, const NavigationAction&, const ResourceRequest&)
{
	notImplemented();
    ASSERT(frame() && policyFunction);
    if (!frame() || !policyFunction)
        return;
    (frame()->loader()->*policyFunction)(PolicyUse);
}

Widget* FrameLoaderClientSyllable::createPlugin(const IntSize&, Element*, const KURL&, const Vector<String>&, const Vector<String>&, const String&, bool loadManually)
{
    notImplemented();
    return 0;
}

PassRefPtr<Frame> FrameLoaderClientSyllable::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement,
                                        const String& referrer, bool allowsScrolling, int marginWidth, int marginHeight)
{
    notImplemented();
    return 0;
}

void FrameLoaderClientSyllable::redirectDataToPlugin(Widget* pluginWidget)
{
    notImplemented();
    return;
}

Widget* FrameLoaderClientSyllable::createJavaAppletWidget(const IntSize&, Element*, const KURL& baseURL,
                                                    const Vector<String>& paramNames, const Vector<String>& paramValues)
{
    notImplemented();
    return 0;
}

ObjectContentType FrameLoaderClientSyllable::objectContentType(const KURL& url, const String& mimeType)
{
    notImplemented();
    return ObjectContentType();
}

String FrameLoaderClientSyllable::overrideMediaType() const
{
    notImplemented();
    return String();
}

void FrameLoaderClientSyllable::windowObjectCleared()
{
    notImplemented();
}

void FrameLoaderClientSyllable::didPerformFirstNavigation() const
{
    notImplemented();
}

void FrameLoaderClientSyllable::setMainFrameDocumentReady(bool) 
{
    // this is only interesting once we provide an external API for the DOM
}

bool FrameLoaderClientSyllable::hasWebView() const
{
    notImplemented();
    return true;
}

bool FrameLoaderClientSyllable::hasFrameView() const
{
    notImplemented();
    return true;
}

void FrameLoaderClientSyllable::dispatchDidFinishLoad() 
{ 
	notImplemented();
}

void FrameLoaderClientSyllable::frameLoadCompleted() 
{
    notImplemented(); 
}

void FrameLoaderClientSyllable::saveViewStateToItem(HistoryItem*)
{
    notImplemented(); 
}

void FrameLoaderClientSyllable::restoreViewState()
{
    notImplemented(); 
}

bool FrameLoaderClientSyllable::shouldGoToHistoryItem(HistoryItem* item) const 
{
    // FIXME: This is a very simple implementation. More sophisticated
    // implementation would delegate the decision to a PolicyDelegate.
    // See mac implementation for example.
    return item != 0;
}

void FrameLoaderClientSyllable::download(ResourceHandle* handle, const ResourceRequest& request, const ResourceRequest&, const ResourceResponse&)
{
	notImplemented();
}

bool FrameLoaderClientSyllable::privateBrowsingEnabled() const { notImplemented(); return false; }
void FrameLoaderClientSyllable::makeDocumentView() { notImplemented(); }
void FrameLoaderClientSyllable::makeRepresentation(DocumentLoader*) { notImplemented(); }
void FrameLoaderClientSyllable::forceLayout() { notImplemented(); }
void FrameLoaderClientSyllable::forceLayoutForNonHTML() { notImplemented(); }
void FrameLoaderClientSyllable::setCopiesOnScroll() { notImplemented(); }
void FrameLoaderClientSyllable::detachedFromParent1() { notImplemented(); }
void FrameLoaderClientSyllable::detachedFromParent2() { notImplemented(); }
void FrameLoaderClientSyllable::detachedFromParent3() { notImplemented(); }
void FrameLoaderClientSyllable::detachedFromParent4() { notImplemented(); }
void FrameLoaderClientSyllable::loadedFromCachedPage() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidHandleOnloadEvents() {notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidReceiveServerRedirectForProvisionalLoad() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidCancelClientRedirect() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchWillPerformClientRedirect(const KURL&, double, double) { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidChangeLocationWithinPage() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchWillClose() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidReceiveIcon() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidStartProvisionalLoad() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidReceiveTitle(const String&) { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidCommitLoad() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidFinishDocumentLoad() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidFirstLayout() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchShow() { notImplemented(); }
void FrameLoaderClientSyllable::cancelPolicyCheck() { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidLoadMainResource(DocumentLoader*) { notImplemented(); }
void FrameLoaderClientSyllable::revertToProvisionalState(DocumentLoader*) { notImplemented(); }
void FrameLoaderClientSyllable::clearUnarchivingState(DocumentLoader*) { notImplemented(); }
void FrameLoaderClientSyllable::willChangeTitle(DocumentLoader*) { notImplemented(); }
void FrameLoaderClientSyllable::didChangeTitle(DocumentLoader *l) { setTitle(l->title(), l->url()); }
void FrameLoaderClientSyllable::finishedLoading(DocumentLoader*) { notImplemented(); }
void FrameLoaderClientSyllable::finalSetupForReplace(DocumentLoader*) { notImplemented(); }
void FrameLoaderClientSyllable::setDefersLoading(bool) { notImplemented(); }
bool FrameLoaderClientSyllable::isArchiveLoadPending(ResourceLoader*) const { notImplemented(); return false; }
void FrameLoaderClientSyllable::cancelPendingArchiveLoad(ResourceLoader*) { notImplemented(); }
void FrameLoaderClientSyllable::clearArchivedResources() { notImplemented(); }
bool FrameLoaderClientSyllable::canHandleRequest(const ResourceRequest&) const { notImplemented(); return true; }
bool FrameLoaderClientSyllable::canShowMIMEType(const String& string) const { DEBUG( "%s\n", os::String( string ).c_str() ); notImplemented(); return true; }
bool FrameLoaderClientSyllable::representationExistsForURLScheme(const String&) const { notImplemented(); return false; }
String FrameLoaderClientSyllable::generatedMIMETypeForURLScheme(const String&) const { notImplemented(); return String(); }
void FrameLoaderClientSyllable::provisionalLoadStarted() { notImplemented(); }
void FrameLoaderClientSyllable::didFinishLoad() { notImplemented(); }
void FrameLoaderClientSyllable::prepareForDataSourceReplacement() { notImplemented(); }
void FrameLoaderClientSyllable::setTitle(const String&, const KURL&) { notImplemented(); }
void FrameLoaderClientSyllable::setDocumentViewFromCachedPage(WebCore::CachedPage*) { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidReceiveContentLength(DocumentLoader*, unsigned long  identifier, int lengthReceived) { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidFinishLoading(DocumentLoader*, unsigned long  identifier) { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidFailLoading(DocumentLoader*, unsigned long  identifier, const ResourceError&) { notImplemented(); }
bool FrameLoaderClientSyllable::dispatchDidLoadResourceFromMemoryCache(DocumentLoader*, const ResourceRequest&, const ResourceResponse&, int length) { notImplemented(); return false; }
void FrameLoaderClientSyllable::dispatchDidFailProvisionalLoad(const ResourceError&) { notImplemented(); }
void FrameLoaderClientSyllable::dispatchDidFailLoad(const ResourceError&) { notImplemented(); }
ResourceError FrameLoaderClientSyllable::cancelledError(const ResourceRequest&) { notImplemented(); return ResourceError(); }
ResourceError FrameLoaderClientSyllable::blockedError(const ResourceRequest&) { notImplemented(); return ResourceError(); }
ResourceError FrameLoaderClientSyllable::cannotShowURLError(const ResourceRequest&) { notImplemented(); return ResourceError(); }
ResourceError FrameLoaderClientSyllable::interruptForPolicyChangeError(const ResourceRequest&) { notImplemented(); return ResourceError(); }
ResourceError FrameLoaderClientSyllable::cannotShowMIMETypeError(const ResourceResponse&) { notImplemented(); return ResourceError(); }
ResourceError FrameLoaderClientSyllable::fileDoesNotExistError(const ResourceResponse&) { notImplemented(); return ResourceError(); }
bool FrameLoaderClientSyllable::shouldFallBack(const ResourceError&) { notImplemented(); return false; }
bool FrameLoaderClientSyllable::willUseArchive(ResourceLoader*, const ResourceRequest&, const KURL& originalURL) const { notImplemented(); return false; }
void FrameLoaderClientSyllable::saveDocumentViewToCachedPage(CachedPage*) { notImplemented(); }
bool FrameLoaderClientSyllable::canCachePage() const { notImplemented(); return false; }
Frame* FrameLoaderClientSyllable::dispatchCreatePage() { notImplemented(); return 0; }
void FrameLoaderClientSyllable::dispatchUnableToImplementPolicy(const ResourceError&) { notImplemented(); }
void FrameLoaderClientSyllable::setMainDocumentError(DocumentLoader*, const ResourceError&) { notImplemented(); }
void FrameLoaderClientSyllable::startDownload(const ResourceRequest&) { notImplemented(); }
void FrameLoaderClientSyllable::updateGlobalHistoryForStandardLoad(const KURL&) { notImplemented(); }
void FrameLoaderClientSyllable::updateGlobalHistoryForReload(const KURL&) { notImplemented(); }
void FrameLoaderClientSyllable::savePlatformDataToCachedPage(CachedPage*) { notImplemented(); }
void FrameLoaderClientSyllable::transitionToCommittedFromCachedPage(CachedPage*) { notImplemented(); }
void FrameLoaderClientSyllable::transitionToCommittedForNewPage() { notImplemented(); }
void FrameLoaderClientSyllable::registerForIconNotification(bool listen) { notImplemented(); }

}















