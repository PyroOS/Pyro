/*
 * Whisper email client for Syllable
 *
 * Copyright (C) 2005-2007 Kristian Van Der Vliet
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
 */

#include <mailbox.h>
#include <syllable_mailbox.h>

static MailboxFactory *g_pcFactory = NULL;

MailboxFactory::~MailboxFactory()
{
	std::list <MailboxNode*>::iterator i;
	for( i = m_vNodes.begin(); i != m_vNodes.end(); i++ )
		delete( (*i) );
	m_vNodes.clear();
}

MailboxFactory * MailboxFactory::GetFactory( void )
{
	if( NULL == g_pcFactory )
	{
		g_pcFactory = new MailboxFactory();
		if( g_pcFactory->_LoadAll() != EOK )
		{
			delete( g_pcFactory );
			g_pcFactory = NULL;
		}
	}

	return g_pcFactory;
}

Mailbox * MailboxFactory::FindMailbox( const os::String cIdentifier, os::String cName )
{
	Mailbox *pcMailbox = NULL;

	std::list<MailboxNode*>::const_iterator i;
	for( i = m_vNodes.begin(); i != m_vNodes.end(); i++ )
	{
		if( (*i)->GetIdentifier() == cIdentifier )
		{
			pcMailbox = (*i)->GetMailbox( cName );
			break;
		}
	}

	return pcMailbox;
}

status_t MailboxFactory::_LoadAll( void )
{
	/* Syllable mailboxes are internal */
	m_vNodes.push_back( new SyllableMailboxNode() );

	/* XXXKV: Load plugins and add nodes to the list */

	return EOK;
}

