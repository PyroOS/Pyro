#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "InspectorClientSyllable.h"
#include "PlatformString.h"
#include "NotImplemented.h"

namespace WebCore {

void InspectorClientSyllable::inspectorDestroyed()
{
	notImplemented();
}

Page* InspectorClientSyllable::createPage()
{
	notImplemented();
	return NULL;
}

String InspectorClientSyllable::localizedStringsURL()
{
	notImplemented();
	return String( "" );
}

void InspectorClientSyllable::showWindow()
{
	notImplemented();
}

void InspectorClientSyllable::closeWindow()
{
	notImplemented();
}

void InspectorClientSyllable::attachWindow()
{
	notImplemented();
}

void InspectorClientSyllable::detachWindow()
{
	notImplemented();
}

void InspectorClientSyllable::highlight(Node*)
{
	notImplemented();
}

void InspectorClientSyllable::hideHighlight()
{
	notImplemented();
}

void InspectorClientSyllable::inspectedURLChanged(const String& newURL)
{
	notImplemented();
}

}

