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

#include <rule_filter.h>
#include <debug.h>
#include <messages.h>
#include <resources/Whisper.h>

#include <util/regexp.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>

/* Used for debugging */
#ifndef NDEBUG
 static char *g_anRuleStatus[] = { "RULE_HIT", "RULE_MISS" };
 static char *g_anConditionals[] = { "NONE", "If", "or", "and" };
#endif

/* We can't have a static array like the two above, because the catalog isn't loaded,
   so we have to it the long way around if we want localised strings */
static const char * get_adjective( const adjective_e nAdjective )
{
	const char *zAdjective;

	switch( nAdjective )
	{
		case NOP:
		{
			zAdjective = MSG_CFGWND_RULEFILTER_ADJNOP.c_str();
			break;
		}

		case EQUALS:
		{
			zAdjective = MSG_CFGWND_RULEFILTER_ADJEQUALS.c_str();
			break;
		}

		case NOT_EQUALS:
		{
			zAdjective = MSG_CFGWND_RULEFILTER_ADJNOTEQUALS.c_str();
			break;
		}

		case LIKE:
		{
			zAdjective = MSG_CFGWND_RULEFILTER_ADJLIKE.c_str();
			break;
		}

		case NOT_LIKE:
		{
			zAdjective = MSG_CFGWND_RULEFILTER_ADJNOTLIKE.c_str();
			break;
		}

		case CONTAINS:
		{
			zAdjective = MSG_CFGWND_RULEFILTER_ADJCONTAINS.c_str();
			break;
		}

		case NOT_CONTAINS:
		{
			zAdjective = MSG_CFGWND_RULEFILTER_ADJNOTCONTAINS.c_str();
			break;
		}
	}

	return zAdjective;
}

using namespace os;

rule_status_e Rule::Test( Mailmessage *pcMessage )
{
	rule_status_e nStatus = RULE_MISS;

	switch( m_nObjectType )
	{
		case HEADER:
		{
			nStatus = TestHeader( pcMessage );
			break;
		}

		case BODY_TEXT:
		{
			nStatus = TestBody( pcMessage );
			break;
		}

		case ATTACHMENT:
		default:
		{
			debug( "object type %d not supported\n", m_nObjectType );
			break;
		}
	}

	/* Invert the result for any NOT adjectives */
	if( ( m_nAdjective == NOT_EQUALS ) ||
	    ( m_nAdjective == NOT_LIKE ) ||
	    ( m_nAdjective == NOT_CONTAINS ) )
	{
		nStatus = ( nStatus == RULE_MISS ) ? RULE_HIT : RULE_MISS;
	}

	return nStatus;
}

rule_status_e Rule::TestHeader( Mailmessage *pcMessage )
{
	String cHeader;

	debug( "%s %s \"%s\"\n", m_cObjectName.c_str(), get_adjective( m_nAdjective ), m_cPattern.c_str() );

	/* Get the header data */
	if( m_cObjectName.CompareNoCase( MSG_CFGWND_RULEFILTER_SUBJECT ) == EOK )
		cHeader = pcMessage->GetSubject();
	else if( m_cObjectName.CompareNoCase( MSG_CFGWND_RULEFILTER_FROM ) == EOK )
		cHeader = pcMessage->GetFrom();
	else if( m_cObjectName.CompareNoCase( MSG_CFGWND_RULEFILTER_TO ) == EOK )
		cHeader = pcMessage->GetTo();
	else
	{ 
		if( pcMessage->GetHeader( m_cObjectName, cHeader ) != EOK )
			return RULE_MISS;
	}

	return TestObject( cHeader );
}

rule_status_e Rule::TestBody( Mailmessage *pcMessage )
{
	String cBody;
	uint nPart, nPartCount;

	debug( "BODY_TEXT %s \"%s\"\n", get_adjective( m_nAdjective ), m_cPattern.c_str() );

	nPartCount = pcMessage->GetPartCount();
	for( nPart = 0; nPart < nPartCount; nPart++ )
	{
		Multipart cPart = pcMessage->GetPartInfo( nPart );
		if( cPart.eDisposition == BODY )
		{
			cBody = pcMessage->GetPart( nPart ).GetData();
			break;
		}
	}

	return TestObject( cBody );
}

rule_status_e Rule::TestObject( String cObject )
{
	rule_status_e nStatus = RULE_MISS;

	/* Choose the adjective to test */
	switch( m_nAdjective )
	{
		case EQUALS:
		case NOT_EQUALS:
		{
			nStatus = TestObjectEquals( cObject );
			break;
		}

		case LIKE:
		case NOT_LIKE:
		{
			nStatus = TestObjectLike( cObject );
			break;
		}

		case CONTAINS:
		case NOT_CONTAINS:
		{
			nStatus = TestObjectContains( cObject );
			break;
		}

		default:
		{
			debug( "unknown rule adjective (%d)\n", m_nAdjective );
		}
	}

	return nStatus;
}

/* Test for straight equality */
rule_status_e Rule::TestObjectEquals( String cObject )
{
	rule_status_e nStatus = RULE_MISS;

	if( cObject.CompareNoCase( m_cPattern ) == EOK )
		nStatus = RULE_HIT;

	return nStatus;
}

/* Treat the pattern as a regexp */
rule_status_e Rule::TestObjectLike( String cObject )
{
	rule_status_e nStatus = RULE_MISS;

	try
	{
		RegExp cRegExp( m_cPattern, true );
		if( cRegExp.Match( cObject ) )
			nStatus = RULE_HIT;
	}
	catch( RegExp::exception &e )
	{
		debug( "error %d (%s) for pattern \"%s\"\n", e.error, e.what(), m_cPattern.c_str() );
	}

	return nStatus;
}

/* Search the object for any instance of the pattern */
rule_status_e Rule::TestObjectContains( String cObject )
{
	rule_status_e nStatus = RULE_MISS;

	if( cObject.Lower().find( m_cPattern.Lower() ) != (size_t)String::npos )
		nStatus = RULE_HIT;

	return nStatus;
}

RuleChain::~RuleChain( void )
{
	std::vector<Rule*>::iterator i;
	for( i = m_vRules.begin(); i != m_vRules.end(); i++ )
		delete (*i);
	m_vRules.clear();

	std::vector<RuleAction*>::iterator j;
	for( j = m_vActions.begin(); j != m_vActions.end(); j++ )
		delete (*j);
	m_vActions.clear();
}

rule_status_e RuleChain::Test( Mailmessage *pcMessage )
{
	rule_status_e nStatus = RULE_MISS;

	std::vector<Rule*>::iterator i;
	for( i = m_vRules.begin(); i != m_vRules.end(); i++ )
	{
		Rule *pcRule = (*i);
		conditional_e nConditional;
		rule_status_e nNextStatus;

		nConditional = pcRule->GetConditional();
		debug( "%s ", g_anConditionals[nConditional] );

		nNextStatus = pcRule->Test( pcMessage );
		debug( "nNextStatus = %s\n", g_anRuleStatus[nNextStatus] );

		switch( nConditional )
		{
			case COND_IF:
			{
				nStatus = nNextStatus;
				break;
			}

			case COND_OR:
			{
				if( nStatus == RULE_HIT || nNextStatus == RULE_HIT )
					nStatus = RULE_HIT;
				break;
			}

			case COND_AND:
			{
				if( nStatus == RULE_HIT && nNextStatus == RULE_HIT )
					nStatus = RULE_HIT;
				else
					nStatus = RULE_MISS;
				break;
			}

			default:
			{
				debug( "unknown rule conditional (%d)\n", nConditional );
			}
		}
	}

	return nStatus;
}

void RuleChain::ApplyActions( Mailmessage *pcMessage, FolderReference &cReference )
{
	debug( "RuleChain::ApplyActions\n" );

	std::vector<RuleAction*>::iterator i;
	for( i = m_vActions.begin(); i != m_vActions.end(); i++ )
		(*i)->Apply( pcMessage, cReference );
}

void RuleAction::Apply( Mailmessage *pcMessage, FolderReference &cReference )
{
	switch( m_eAction )
	{
		case NONE:
		{
			break;
		}

		case MOVE:
		{
			cReference.m_eType = m_eFolderType;
			cReference.m_cName = m_cFolderName;
			break;
		}

		case MARK:
		{
			pcMessage->SetStatus( m_nStatus );
			break;
		}

		case FLAG:
		{
			pcMessage->SetFlag( m_nFlag );
			break;
		}

		case DELETE:
		{
			pcMessage->SetStatus( STATUS_READ );
			cReference.m_eType = FL_TYPE_TRASH;
			break;
		}
	}
}

RuleFilter::RuleFilter( void )
{
	Load();
}

RuleFilter::~RuleFilter( void )
{
	std::vector<RuleChain*>::iterator i;
	for( i = m_vChains.begin(); i != m_vChains.end(); i++ )
		delete (*i);
	m_vChains.clear();
}

status_t RuleFilter::FilterMessage( Mailmessage *pcMessage, FolderReference &cReference )
{
	rule_status_e nStatus;

	std::vector<RuleChain*>::iterator i;
	for( i = m_vChains.begin(); i != m_vChains.end(); i++ )
	{
		RuleChain *pcChain = (*i);
		if( NULL == pcChain )
			continue;

		nStatus = pcChain->Test( pcMessage );
		if( nStatus == RULE_HIT )
		{
			pcChain->ApplyActions( pcMessage, cReference );
			break;
		}
	}

	return EOK;
}

SettingsTab * RuleFilter::GetSettingsView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow )
{
	return new RuleFilterSettingsTab( cFrame, this, pcIdentity, pcMainWindow );
}

void RuleFilter::RemoveChain( int nIndex )
{
	int n;
	std::vector<RuleChain*>::iterator i;
	for( n = 0, i = m_vChains.begin(); i != m_vChains.end(); n++, i++ )
	{
		if( n == nIndex )
		{
			i = m_vChains.erase( i );
			delete( *i );
			break;
		}
	}
}

/* Most of the following code is so ugly that adding goto's would actually help. Blergh. */
status_t RuleFilter::Load( void )
{
	int nFd;

	String cPath = String( getenv( "HOME" ) );
	if( cPath == "" )
		return EINVAL;

	cPath += "/Settings/Whisper/Rules";
	debug( "%s\n", cPath.c_str() );

	nFd = open( cPath.c_str(), O_RDONLY );
	if( nFd < 0 )
	{
		debug( "failed to open Rules file.\n" );
		return EIO;
	}

	/* Read & check the file header */
	struct rule_file_header sFileHeader;
	int nSize = sizeof( sFileHeader );
	if( read( nFd, &sFileHeader, nSize ) < nSize )
	{
		close( nFd );
		return EIO;
	}
	if( sFileHeader.nMagic != RULE_FILE_MAGIC )
	{
		debug( "rules file magic mismatch!\n" );
		close( nFd );
		return EIO;
	}
	debug( "loading a version #%d rules file with %d chain(s)\n", sFileHeader.nVersion, sFileHeader.nChainCount );

	for( uint n = 0; n < sFileHeader.nChainCount; n++ )
		LoadChain( nFd );

	close( nFd );
	return EOK;
}

status_t RuleFilter::LoadChain( int nFd )
{
	int nSize;
	status_t nError = EOK;

	/* Read the chain header */
	struct chain_header sChainHeader;
	nSize = sizeof( sChainHeader );
	if( read( nFd, &sChainHeader, nSize ) < nSize )
		return EIO;

	debug( "this chain has %d rule(s) & %d action(s)\n", sChainHeader.nRuleCount, sChainHeader.nActionCount );

	RuleChain *pcChain = new RuleChain();
	Rule *pcRule = NULL;
	RuleAction *pcAction = NULL;

	/* Read, unflatten and add each Rule */
	uint n;
	for( n = 0; n < sChainHeader.nRuleCount; n++ )
	{
		debug( "load rule #%d\n", n );

		struct rule_header sRuleHeader;
		nSize = sizeof( sRuleHeader );
		if( read( nFd, &sRuleHeader, nSize ) < nSize )
		{
			nError = EIO;
			break;
		}

		if( sRuleHeader.nMagic != RULE_HEADER_MAGIC )
		{
			debug( "rule magic mismatch!\n" );
			pcRule = NULL;
			nError = EIO;
			break;
		}

		debug( "nSize=%d\n", sRuleHeader.nSize );

		/* Process the rule */
		pcRule = new Rule();

		int32 nVal;
		String cString;

		/* Conditional */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_INT32 )
		{
			nError = EINVAL;
			break;
		}
		pcRule->SetConditional( (conditional_e)nVal );

		/* Type */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_INT32 )
		{
			nError = EINVAL;
			break;
		}
		pcRule->SetObjectType( (Rule::object_type_e)nVal );

		/* Name */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_STRING )
		{
			nError = EINVAL;
			break;
		}
		pcRule->SetObjectName( cString );

		/* Adjective */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_INT32 )
		{
			nError = EINVAL;
			break;
		}
		pcRule->SetAdjective( (adjective_e)nVal );

		/* Pattern */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_STRING )
		{
			nError = EINVAL;
			break;
		}
		pcRule->SetPattern( cString );

		pcChain->AddRule( pcRule );
	}
	if( nError != EOK )
	{
		if( pcRule )
			delete( pcRule );
		delete( pcChain );
		return nError;
	}

	/* Read, unflatten and add each RuleAction */
	for( n = 0; n < sChainHeader.nActionCount; n++ )
	{
		debug( "load action #%d\n", n );

		struct action_header sActionHeader;
		nSize = sizeof( sActionHeader );
		if( read( nFd, &sActionHeader, nSize ) < nSize )
		{
			nError = EIO;
			break;
		}

		if( sActionHeader.nMagic != ACTION_HEADER_MAGIC )
		{
			debug( "action magic mismatch!\n" );
			nError = EIO;
			break;
		}

		debug( "nSize=%d\n", sActionHeader.nSize );

		/* Process the action */
		pcAction = new RuleAction();
		int32 nVal;
		String cString;

		/* Action */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_INT32 )
		{
			nError = EINVAL;
			break;
		}
		pcAction->SetAction( (rule_action_e)nVal );

		/* Status */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_INT32 )
		{
			nError = EINVAL;
			break;
		}
		pcAction->SetStatus( nVal );

		/* Flag */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_INT32 )
		{
			nError = EINVAL;
			break;
		}
		pcAction->SetFlag( nVal );

		/* Folder type */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_INT32 )
		{
			nError = EINVAL;
			break;
		}

		/* Folder name */
		if( Unflatten( nFd, nVal, cString ) != OBJECT_TYPE_STRING )
		{
			nError = EINVAL;
			break;
		}
		pcAction->SetFolder( (folder_type_e )nVal, cString );

		pcChain->AddAction( pcAction );
	}
	if( nError != EOK )
	{
		if( pcRule )
			delete( pcRule );
		if( pcAction )
			delete( pcAction );
		delete( pcChain );
		return nError;
	}

	AddChain( pcChain );
	return EOK;
}

int8 RuleFilter::Unflatten( int nFd, int32 &nVal, String &cString )
{
	struct object_header sHeader;
	if( read( nFd, &sHeader, sizeof( sHeader ) ) < 0 )
		return -1;

	debug( "type=%d size=%d\n", sHeader.nType, sHeader.nSize );
	switch( sHeader.nType )
	{
		case OBJECT_TYPE_INT32:
		{
			nVal = UnflattenInt( nFd );
			break;
		}

		case OBJECT_TYPE_STRING:
		{
			cString = UnflattenString( nFd, sHeader.nSize );
			break;
		}

		default:
		{
			debug( "unknown object type %d!\n", sHeader.nType ); 
			return -1;
		}
	}

	return sHeader.nType;
}

int32 RuleFilter::UnflattenInt( int nFd )
{
	int32 nVal;

	if( read( nFd, &nVal, 4 ) < 4 )
		return -1;

	debug( "nVal=%d\n", nVal );

	return nVal;
}

String RuleFilter::UnflattenString( int nFd, ssize_t nSize )
{
	char *pBuffer = (char*)malloc( nSize + 1 );
	String cString;

	if( read( nFd, pBuffer, nSize ) == nSize )
	{
		pBuffer[nSize] = '\0';
		cString = pBuffer;
	}
	free( pBuffer );

	debug( "%s\n", cString.c_str() );

	return cString;
}

/* Flatten each RuleChain and stream them to a file */
status_t RuleFilter::Save( void )
{
	int nFd;

	String cPath = String( getenv( "HOME" ) );
	if( cPath == "" )
		return EINVAL;

	cPath += "/Settings/Whisper/Rules";
	debug( "%s\n", cPath.c_str() );

	nFd = open( cPath.c_str(), O_WRONLY|O_CREAT );
	if( nFd < 0 )
	{
		debug( "failed to open Rules file.\n" );
		return EIO;
	}

	/* Write a file header first, of course */
	struct rule_file_header sFileHeader;
	sFileHeader.nMagic = RULE_FILE_MAGIC;
	sFileHeader.nVersion = RULE_FILE_VERSION;
	sFileHeader.nChainCount = m_vChains.size();

	if( write( nFd, &sFileHeader, sizeof( sFileHeader ) ) < 0 )
	{
		close( nFd );
		return EIO;
	}

	/* Process each RuleChain */
	std::vector<RuleChain*>::iterator i;
	for( i = m_vChains.begin(); i != m_vChains.end(); i++ )
	{
		if( NULL == (*i) )
			continue;

		if( SaveChain( nFd, (*i) ) != EOK )
		{
			close( nFd );
			return EIO;
		}
	}

	close( nFd );
	return EOK;
}

status_t RuleFilter::SaveChain( int nFd, RuleChain *pcChain )
{
	struct chain_header sChainHeader;
	sChainHeader.nRuleCount = pcChain->GetRuleCount();
	sChainHeader.nActionCount = pcChain->GetActionCount();

	if( write( nFd, &sChainHeader, sizeof( sChainHeader ) ) < 0 )
		return EIO;

	/* Write all the Rules */
	uint n;
	for( n = 0; n < sChainHeader.nRuleCount; n++ )
	{
		Rule *pcRule = pcChain->GetRule( n );
		uint8 *pBuffer = (uint8*)calloc( 1, 4096 );		/* XXXKV: This is an overflow waiting to happen... */
		size_t nSize = 0;
		int nError;

		/* Flatten the Rule first so that we know the size */
		nSize += Flatten( pBuffer + nSize, pcRule->GetConditional() );
		nSize += Flatten( pBuffer + nSize, pcRule->GetObjectType() );
		nSize += Flatten( pBuffer + nSize, pcRule->GetObjectName() );
		nSize += Flatten( pBuffer + nSize, pcRule->GetAdjective() );
		nSize += Flatten( pBuffer + nSize, pcRule->GetPattern() );

		/* XXXKV: Yeah, this is real helpful */
		if( nSize > 4096 )
			debug( "overflow detected\n" );

		/* Write a Rule header */
		struct rule_header sRuleHeader;
		sRuleHeader.nMagic = RULE_HEADER_MAGIC;
		sRuleHeader.nSize = nSize;

		nError = write( nFd, &sRuleHeader, sizeof( sRuleHeader ) );
		if( nError >= 0 )
			nError = write( nFd, pBuffer, nSize );
		free( pBuffer );

		if( nError < 0 )
		{
			debug( "Failed to write rule!\n" );
			return EIO;
		}
	}

	/* Write all the RuleActions */
	for( n = 0; n < sChainHeader.nActionCount; n++ )
	{
		RuleAction *pcAction = pcChain->GetAction( n );
		uint8 *pBuffer = (uint8*)calloc( 1, 4096 );		/* XXXKV: This is an overflow waiting to happen... */
		size_t nSize = 0;
		int nError;

		/* Flatten the RuleAction first so that we know the size */
		nSize += Flatten( pBuffer + nSize, pcAction->GetAction() );
		nSize += Flatten( pBuffer + nSize, pcAction->GetStatus() );
		nSize += Flatten( pBuffer + nSize, pcAction->GetFlag() );
		nSize += Flatten( pBuffer + nSize, pcAction->GetFolderType() );
		nSize += Flatten( pBuffer + nSize, pcAction->GetFolderName() );

		/* XXXKV: Yeah, this is real helpful */
		if( nSize > 4096 )
			debug( "overflow detected\n" );

		/* Write a RuleAction header */
		struct action_header sActionHeader;
		sActionHeader.nMagic = ACTION_HEADER_MAGIC;
		sActionHeader.nSize = nSize;

		nError = write( nFd, &sActionHeader, sizeof( sActionHeader ) );
		if( nError >= 0 )
			nError = write( nFd, pBuffer, nSize );
		free( pBuffer );

		if( nError < 0 )
		{
			debug( "Failed to write action!\n" );
			return EIO;
		}
	}

	return EOK;
}

/* Flatten the value into pBuffer. Returns the total bytes written to pBuffer */
size_t RuleFilter::Flatten( uint8 *pBuffer, int32 nVal )
{
	struct object_header sHeader;
	sHeader.nType = OBJECT_TYPE_INT32;
	sHeader.nSize = 4;

	size_t nSize = sizeof( sHeader );

	memcpy( pBuffer, &sHeader, nSize );
	memcpy( pBuffer + nSize, &nVal, 4 );

	return nSize + 4;
}

/* Flatten the string into pBuffer. Returns the total bytes written to pBuffer */
size_t RuleFilter::Flatten( uint8 *pBuffer, String cString )
{
	size_t nLen = cString.Length();

	struct object_header sHeader;
	sHeader.nType = OBJECT_TYPE_STRING;
	sHeader.nSize = nLen;

	size_t nSize = sizeof( sHeader );

	memcpy( pBuffer, &sHeader, nSize );
	memcpy( pBuffer + nSize, cString.c_str(), nLen );

	return nSize + nLen;
}

SentenceView::SentenceView( const Rect cFrame, const int nId ) : LayoutView( cFrame, "rule_sentence", NULL, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT )
{
	m_pcIf = NULL;
	m_pcConditional =
	m_pcObject =
	m_pcAdjective = NULL;
	m_pcPattern = NULL;

	m_pcVLayoutNode = new VLayoutNode( "rule_sentence_v_layout" );
	m_pcHLayoutNode = new HLayoutNode( "rule_sentence_h_layout" );
	HLayoutSpacer *pcSpacer;

	/* The first sentance always begins with "If" */
	if( nId == 0 )
	{
		m_pcIf = new StringView( Rect(), "rules_if", MSG_CFGWND_RULEFILTER_IF, ALIGN_LEFT, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
		m_pcHLayoutNode->AddChild( m_pcIf, 0.0f );
	}
	else
	{
		m_pcConditional = new DropdownMenu( Rect(), "rules_conditional", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
		m_pcConditional->AppendItem( "" );
		m_pcConditional->AppendItem( "or" );
		m_pcConditional->AppendItem( "and" );

		Message *pcMessage = new Message( ID_CONDITIONAL_CHANGED );
		pcMessage->AddInt32( "id", nId );

		m_pcConditional->SetSelectionMessage( pcMessage );
		m_pcConditional->SetReadOnly();

		m_pcHLayoutNode->AddChild( m_pcConditional, 0.1f );
	}
	pcSpacer = new HLayoutSpacer( "rule_h_spacer_1", 0.0f );
	m_pcHLayoutNode->AddChild( pcSpacer );

	/* Object */
	m_pcObject = new DropdownMenu( Rect(), "rules_object", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
	m_pcObject->AppendItem( "" );
	m_pcObject->AppendItem( MSG_CFGWND_RULEFILTER_SUBJECT );
	m_pcObject->AppendItem( MSG_CFGWND_RULEFILTER_FROM );
	m_pcObject->AppendItem( MSG_CFGWND_RULEFILTER_TO );
	m_pcObject->AppendItem( MSG_CFGWND_RULEFILTER_MESSAGETEXT );

	m_pcHLayoutNode->AddChild( m_pcObject, 0.1f );

	pcSpacer = new HLayoutSpacer( "rule_h_spacer_2", 0.0f );
	m_pcHLayoutNode->AddChild( pcSpacer );

	/* Adjective */
	m_pcAdjective = new DropdownMenu( Rect(), "rules_adjective", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
	m_pcAdjective->AppendItem( "" );
	m_pcAdjective->AppendItem( MSG_CFGWND_RULEFILTER_ADJEQUALS );
	m_pcAdjective->AppendItem( MSG_CFGWND_RULEFILTER_ADJNOTEQUALS );
	m_pcAdjective->AppendItem( MSG_CFGWND_RULEFILTER_ADJLIKE );
	m_pcAdjective->AppendItem( MSG_CFGWND_RULEFILTER_ADJNOTLIKE );
	m_pcAdjective->AppendItem( MSG_CFGWND_RULEFILTER_ADJCONTAINS );
	m_pcAdjective->AppendItem( MSG_CFGWND_RULEFILTER_ADJNOTCONTAINS );
	m_pcAdjective->SetReadOnly();

	m_pcHLayoutNode->AddChild( m_pcAdjective, 0.1f );

	pcSpacer = new HLayoutSpacer( "rule_h_spacer_3", 0.0f );
	m_pcHLayoutNode->AddChild( pcSpacer );

	m_pcVLayoutNode->AddChild( m_pcHLayoutNode );

	/* Pattern */
	m_pcPattern = new TextView( Rect(), "rule_pattern", NULL, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
	m_pcVLayoutNode->AddChild( m_pcPattern, 1.0f );

	SetRoot( m_pcVLayoutNode );
}

SentenceView::~SentenceView()
{
}

void SentenceView::AllAttached()
{
	View::AllAttached();
	if( m_pcConditional )
		m_pcConditional->SetTarget( GetParent() );
}

void SentenceView::SetEnable( bool bEnable )
{
	if( m_pcConditional )
		m_pcConditional->SetEnable( bEnable );
	m_pcObject->SetEnable( bEnable );
	m_pcAdjective->SetEnable( bEnable );
	m_pcPattern->SetEnable( bEnable );
}

void SentenceView::SetConditionalEnable( bool bEnable )
{
	if( m_pcConditional )
		m_pcConditional->SetEnable( bEnable );
}

/* These Seters & Geters rely on the order of the items in the DropdownMenu, making them lazy and semi-magical */

conditional_e SentenceView::GetConditional( void )
{
	int nSelection;
	conditional_e nConditional;

	if( m_pcConditional )
	{
		nSelection = m_pcConditional->GetSelection();
		if( nSelection == 0 )
			nConditional = COND_NONE;
		else if( nSelection == 1 )
			nConditional = COND_OR;
		else
			nConditional = COND_AND;
	}
	else
		nConditional = COND_IF;

	return nConditional;
}

void SentenceView::SetConditional( conditional_e nConditional )
{
	if( m_pcConditional )
	{
		int nSelection;
		if( nConditional == COND_NONE || nConditional == COND_IF )
			nSelection = 0;
		else if( nConditional == COND_OR )
			nSelection = 1;
		else
			nSelection = 2;

		m_pcConditional->SetSelection( nSelection );
	}
}

String SentenceView::GetObject( void )
{
	return m_pcObject->GetCurrentString();
}

void SentenceView::SetObject( String cObject )
{
	/* Look for common cases */
	if( cObject.CompareNoCase( MSG_CFGWND_RULEFILTER_SUBJECT ) == 0 )
		m_pcObject->SetSelection( 1 );
	else if( cObject.CompareNoCase( MSG_CFGWND_RULEFILTER_FROM ) == 0 )
		m_pcObject->SetSelection( 2 );
	else if( cObject.CompareNoCase( MSG_CFGWND_RULEFILTER_TO ) == 0 )
		m_pcObject->SetSelection( 3 );
	else if( cObject.CompareNoCase( MSG_CFGWND_RULEFILTER_MESSAGETEXT ) == 0 )
		m_pcObject->SetSelection( 4 );
	else
		m_pcObject->SetCurrentString( cObject );
}

adjective_e SentenceView::GetAdjective( void )
{
	return( (adjective_e)m_pcAdjective->GetSelection() );
}

void SentenceView::SetAdjective( adjective_e nAdjective )
{
	m_pcAdjective->SetSelection( (int)nAdjective );
}

String SentenceView::GetPattern( void )
{
	return m_pcPattern->GetValue().AsString();
}

void SentenceView::SetPattern( String cPattern )
{
	m_pcPattern->SetValue( cPattern );
}

void SentenceView::Clear( void )
{
	if( m_pcConditional )
		m_pcConditional->SetSelection( 0, false );
	m_pcObject->SetSelection( 0 );
	m_pcAdjective->SetSelection( 0 );
	m_pcPattern->Clear( false );	
}

ActionView::ActionView( const Rect cFrame, const int nId, const Handler *pcMainWindow ): LayoutView( cFrame, "rule_action", NULL, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT )
{
	m_pcAction =
	m_pcTarget = NULL;

	m_pcMainMessenger = new Messenger( pcMainWindow );

	m_pcHLayoutNode = new HLayoutNode( "rule_action_h_layout" );
	HLayoutSpacer *pcSpacer;

	/* Action */
	m_pcAction = new DropdownMenu( Rect(), "rules_action", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
	m_pcAction->AppendItem( "" );
	m_pcAction->AppendItem( MSG_CFGWND_RULEFILTER_ACTIONMOVE );
	m_pcAction->AppendItem( MSG_CFGWND_RULEFILTER_ACTIONMARK );
	m_pcAction->AppendItem( MSG_CFGWND_RULEFILTER_ACTIONFLAG );
	m_pcAction->AppendItem( MSG_CFGWND_RULEFILTER_ACTIONDELETE );

	Message *pcMessage = new Message( ID_ACTION_CHANGED );
	pcMessage->AddInt32( "id", nId );

	m_pcAction->SetSelectionMessage( pcMessage );
	m_pcAction->SetReadOnly();

	m_pcHLayoutNode->AddChild( m_pcAction, 0.1f );

	pcSpacer = new HLayoutSpacer( "rule_h_spacer_1", 0.0f );
	m_pcHLayoutNode->AddChild( pcSpacer );

	m_pcTheMessage = new StringView( Rect(), "rules_the_message", MSG_CFGWND_RULEFILTER_THEMESSAGE, ALIGN_LEFT, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
	m_pcHLayoutNode->AddChild( m_pcTheMessage, 0.0f );

	pcSpacer = new HLayoutSpacer( "rule_h_spacer_2", 0.0f );
	m_pcHLayoutNode->AddChild( pcSpacer );

	/* Target */
	m_pcTarget = new DropdownMenu( Rect(), "rules_target", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
	m_pcTarget->AppendItem( "" );

	m_pcHLayoutNode->AddChild( m_pcTarget, 0.1f );

	SetRoot( m_pcHLayoutNode );
}

ActionView::~ActionView()
{
	delete( m_pcParentMessenger );
	delete( m_pcMainMessenger );
}

void ActionView::AllAttached()
{
	View::AllAttached();
	m_pcAction->SetTarget( this );
	m_pcTarget->SetTarget( this );

	m_pcParentMessenger = new Messenger( GetParent() );
}

void ActionView::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_ACTION_CHANGED:
		{
			int nSelected = m_pcAction->GetSelection();
			PopulateTarget( nSelected );

			m_pcParentMessenger->SendMessage( pcMessage );

			break;
		}

		default:
		{
			View::HandleMessage( pcMessage );
		}
	}
}

void ActionView::PopulateTarget( int nSelection )
{
	/* Reset the current contents of the dropdown */
	m_pcTarget->Clear();
	m_pcTarget->SetCurrentString( "" );

	switch( nSelection )
	{
		case 1:	/* Move */
		{
			/* Populate m_pcTarget with a list of folders */
			m_pcTarget->AppendItem( "" );

			/* Retrieve a list of the available folders from the main window */
			Message *pcMessage = new Message( M_GET_FOLDER_LIST );
			Message *pcReply = new Message();

			m_pcMainMessenger->SendMessage( pcMessage, pcReply );

			if( pcReply->GetCode() == M_GET_FOLDER_LIST )
			{
				String cName;
				int64 nType;

				int n = 0;
				while( pcReply->FindString( "name", &cName, n ) == 0 &&
					   pcReply->FindInt64( "type", &nType, n ) == 0 )
				{
					/* XXXKV: We have no simple way to store the type */
					m_pcTarget->AppendItem( cName );
					n++;
				}
			}

			delete( pcMessage );
			delete( pcReply );

			m_pcTarget->SetEnable( true );
			break;
		}

		case 2:	/* Mark */
		{
			/* Populate m_pcTarget with a list of statuses */
			m_pcTarget->AppendItem( MSG_MAINWND_MENU_MESSAGE_MARK_READ );		/* STATUS_READ */
			m_pcTarget->AppendItem( MSG_MAINWND_MENU_MESSAGE_MARK_UNREAD );	/* STATUS_UNREAD */
			m_pcTarget->AppendItem( MSG_MAINWND_MENU_MESSAGE_MARK_DRAFT );	/* STATUS_DRAFT */

			m_pcTarget->SetEnable( true );
			break;
		}

		case 3:	/* Flag */
		{
			/* Populate m_pcTarget with a list of flags */
			m_pcTarget->AppendItem( MSG_MAINWND_MENU_MESSAGE_FLAG_NONE ); /* FLAG_NONE */
			m_pcTarget->AppendItem( MSG_MAINWND_MENU_MESSAGE_FLAG_LOW ); /* FLAG_LOW */
			m_pcTarget->AppendItem( MSG_MAINWND_MENU_MESSAGE_FLAG_MEDIUM ); /* FLAG_MEDIUM */
			m_pcTarget->AppendItem( MSG_MAINWND_MENU_MESSAGE_FLAG_HIGH ); /* FLAG_HIGH */
			m_pcTarget->AppendItem( MSG_MAINWND_MENU_MESSAGE_FLAG_URGENT ); /* FLAG_URGENT */

			m_pcTarget->SetEnable( true );
			break;
		}

		default:
		case 0:	/* Empty */
		case 4:	/* Delete */
		{
			m_pcTarget->SetEnable( false );
			break;
		}
	}
}

void ActionView::SetEnable( bool bEnable )
{
	m_pcAction->SetEnable( bEnable );
	int nSelection = m_pcAction->GetSelection();
	if( bEnable && ( nSelection != 0 && nSelection != 4 ) )
		m_pcTarget->SetEnable( true );
	else
		m_pcTarget->SetEnable( false );
}

void ActionView::SetActionEnable( bool bEnable )
{
	m_pcAction->SetEnable( bEnable );
}

void ActionView::SetAction( rule_action_e nAction )
{
	int nSelection = (int)nAction;

	m_pcAction->SetSelection( nSelection, false );
	PopulateTarget( nSelection );
}

rule_action_e ActionView::GetAction( void )
{
	return( (rule_action_e)m_pcAction->GetSelection() );
}

void ActionView::SetStatus( int32 nStatus )
{
	int nSelection;
	switch( nStatus )
	{
		case STATUS_READ:
		{
			nSelection = 0;
			break;
		}

		case STATUS_UNREAD:
		{
			nSelection = 1;
			break;
		}

		case STATUS_DRAFT:
		{
			nSelection = 2;
			break;
		}
	}
	m_pcTarget->SetSelection( nSelection );
}

int32 ActionView::GetStatus( void )
{
	int32 nStatus;
	switch( m_pcTarget->GetSelection() )
	{
		case 0:
		{
			nStatus = STATUS_READ;
			break;
		}

		case 1:
		{
			nStatus = STATUS_UNREAD;
			break;
		}

		case 2:
		{
			nStatus = STATUS_DRAFT;
			break;
		}
	}
	return( nStatus );
}

void ActionView::SetFlag( int32 nFlag )
{
	m_pcTarget->SetSelection( nFlag );
}

int32 ActionView::GetFlag( void )
{
	return( m_pcTarget->GetSelection() );
}

void ActionView::SetFolder( String cName )
{
	m_pcTarget->SetCurrentString( cName );
}

String ActionView::GetFolder( void )
{
	return m_pcTarget->GetCurrentString();
}

void ActionView::Clear( void )
{
	m_pcAction->SetSelection( 0, false );
	m_pcTarget->SetSelection( 0, false );
	m_pcTarget->SetCurrentString( "");
}

RuleFilterSettingsTab::RuleFilterSettingsTab( const Rect cFrame, RuleFilter *pcFilter, Identity *pcIdentity, Handler *pcMainWindow ) : SettingsTab( cFrame, "rule_filter_settings_tab", pcIdentity )
{
	m_pcFilter = pcFilter;

	Rect cBounds = GetBounds();
	Rect cSentenceFrame;
	cSentenceFrame.left = 10;
	cSentenceFrame.top = 10;
	cSentenceFrame.right = cBounds.Width() - 10;
	cSentenceFrame.bottom = cSentenceFrame.top + 50;

	/* Four rules... */
	for( int n = 0; n < 4; n++ )
	{
		SentenceView *pcSentence;

		pcSentence = new SentenceView( cSentenceFrame, n );
		pcSentence->SetEnable( false );

		m_vSentences.push_back( pcSentence );

		AddChild( pcSentence );

		cSentenceFrame.top = cSentenceFrame.bottom + 5;
		cSentenceFrame.bottom = cSentenceFrame.top + 50;
	}

	/* ...and two actions */
	Rect cActionFrame = cSentenceFrame;
	for( int n = 0; n < 2; n++ )
	{
		ActionView *pcAction = new ActionView( cActionFrame, n, pcMainWindow );
		pcAction->SetEnable( false );

		m_vActions.push_back( pcAction );

		AddChild( pcAction );

		cActionFrame.top = cActionFrame.bottom + 5;
		cActionFrame.bottom = cActionFrame.top + 50;
	}

	/* A button to save the edit */
	Rect cSaveFrame = cBounds;
	cSaveFrame.left = ( cSaveFrame.right / 4 ) * 3;
	cSaveFrame.top = cActionFrame.bottom + 5;
	cSaveFrame.right = cBounds.right - 5;
	cSaveFrame.bottom = cSaveFrame.top + INPUT_HEIGHT;

	m_pcSave = new Button( cSaveFrame, "rules_save", MSG_CFGWND_RULEFILTER_BUTTON_SAVE, new Message( ID_RULE_SAVE ), CF_FOLLOW_RIGHT );
	m_pcSave->SetTabOrder( NEXT_TAB_ORDER );

	cSaveFrame.left = cSaveFrame.right - m_pcSave->GetPreferredSize( false ).x + 1;
	cSaveFrame.bottom = cSaveFrame.top + m_pcSave->GetPreferredSize( false ).y + 1;
	m_pcSave->SetFrame( cSaveFrame );

	AddChild( m_pcSave );

	/* The "instances" layout is composed of a ListView and some buttons, all within a
	   LayoutView.  The buttons are contained in a vertical layout node of their own */
	Rect cInstanceFrame = cBounds;
	cInstanceFrame.top = cSaveFrame.bottom + 5;

	m_pcLayoutView = new LayoutView( cInstanceFrame, "rules_layout_view" );

	m_pcLayoutRoot = new VLayoutNode( "rules_layout_root" );
	m_pcLayoutRoot->SetBorders( Rect( 5, 5, 5, 5 ) );

	m_pcInstancesLayout = new HLayoutNode( "rules_instance_layout" );

	m_pcInstances = new ListView( Rect(), "rules_instances", ListView::F_NO_AUTO_SORT | ListView::F_RENDER_BORDER );
	m_pcInstances->InsertColumn( MSG_CFGWND_RULEFILTER_RULES.c_str(), 1 );
	m_pcInstances->SetSelChangeMsg( new Message( ID_RULE_SELECT ) );
	m_pcInstances->SetTabOrder( NEXT_TAB_ORDER );
	m_pcInstancesLayout->AddChild( m_pcInstances, 30.0f );
	m_pcInstancesLayout->AddChild( new HLayoutSpacer( "rules_h_spacer", 1.0f ) );

	/* Buttons to manipulate the items in the list */
	m_pcButtonsLayout = new VLayoutNode( "rules_layout_buttons", 2.0f );

	m_pcNewInstance = new Button( Rect(),"rules_new", MSG_CFGWND_RULEFILTER_BUTTON_NEW, new Message( ID_RULE_NEW ) );
	m_pcNewInstance->SetTabOrder( NEXT_TAB_ORDER );
	m_pcButtonsLayout->AddChild( m_pcNewInstance, 0.0f );

	m_pcDeleteInstance = new Button( Rect(),"rules_delete", MSG_CFGWND_RULEFILTER_BUTTON_DEL, new Message( ID_RULE_DELETE ) );
	m_pcDeleteInstance->SetTabOrder( NEXT_TAB_ORDER );
	m_pcButtonsLayout->AddChild( m_pcDeleteInstance, 0.0f );

	m_pcButtonsLayout->AddChild( new VLayoutSpacer( "rules_v_spacer", 20.0f ) );

	m_pcInstancesLayout->AddChild( m_pcButtonsLayout );

	/* Add the "instances" layout to the main layout node */
	m_pcLayoutRoot->AddChild( m_pcInstancesLayout );

	/* Set the root layoutnode and add the layoutview to the view */
	m_pcLayoutView->SetRoot( m_pcLayoutRoot );
	AddChild( m_pcLayoutView );

	/* Disable everything but the ListView & New button until something is selected */
	m_pcSave->SetEnable( false );
	m_pcDeleteInstance->SetEnable( false );

	/* Populate the instances list */
	/* XXXKV: LOCKING! */
	for( int n = 0; n < m_pcFilter->GetChainCount(); n++ )
	{
		RuleChain *pcChain = m_pcFilter->GetChain( n );
		AppendChain( pcChain );
	}
}

RuleFilterSettingsTab::~RuleFilterSettingsTab()
{
	std::vector <SentenceView *> ::iterator i;
	for( i = m_vSentences.begin(); i != m_vSentences.end(); i++ )
	{
		RemoveChild( (*i) );
		delete( (*i) );
	}
	m_vSentences.clear();

	std::vector <ActionView *> ::iterator j;
	for( j = m_vActions.begin(); j != m_vActions.end(); j++ )
	{
		RemoveChild( (*j) );
		delete( (*j) );
	}
	m_vActions.clear();

	if( m_pcSave )
	{
		RemoveChild( m_pcSave );
		delete( m_pcSave );
	}
}

void RuleFilterSettingsTab::AllAttached()
{
	View::AllAttached();
	m_pcSave->SetTarget( this );
	m_pcInstances->SetTarget( this );
	m_pcNewInstance->SetTarget( this );
	m_pcDeleteInstance->SetTarget( this );
}

void RuleFilterSettingsTab::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_CONDITIONAL_CHANGED:
		{
			int nId;

			if( pcMessage->FindInt32( "id", &nId ) == 0 &&
				nId >= 0 && nId <= (int)m_vSentences.size() )
			{
				HandleConditionalChanged( nId );
			}
			break;
		}

		case ID_ACTION_CHANGED:
		{
			int nId;

			if( pcMessage->FindInt32( "id", &nId ) == 0 &&
				nId >= 0 && nId <= (int)m_vActions.size() )
			{
				HandleActionChanged( nId );
			}
			break;
		}

		case ID_RULE_SAVE:
		{
			HandleSave();
			break;
		};

		case ID_RULE_SELECT:
		{
			HandleSelect();
			break;
		};

		case ID_RULE_NEW:
		{
			HandleNew();
			break;
		};

		case ID_RULE_DELETE:
		{
			HandleDelete();
			break;
		};

		default:
			View::HandleMessage( pcMessage );
	}
}

void RuleFilterSettingsTab::HandleConditionalChanged( int nId )
{
	int nNextId;
	SentenceView *pcSentence;

	nNextId = nId + 1;
	pcSentence = m_vSentences[nId];

	/* Check if the selection has been reset I.e. Empty */
	if( pcSentence->GetConditional() == COND_NONE )
	{
		/* Disable this sentence */
		pcSentence->SetEnable( false );
		pcSentence->SetConditionalEnable( true );

		/* Disable all the subsequent sentences */
		for( ; nNextId < (int)m_vSentences.size(); nNextId++ )
		{
			pcSentence = m_vSentences[nNextId];
			pcSentence->SetEnable( false );
		}
	}
	else
	{
		/* Enable this sentence */
		pcSentence->SetEnable( true );

		if( nNextId < (int)m_vSentences.size() )
		{
			/* Enable the conditional of the next sentence */
			pcSentence = m_vSentences[nNextId];
			pcSentence->SetConditionalEnable( true );
		}
	}
}

void RuleFilterSettingsTab::HandleActionChanged( int nId )
{
	int nNextId;
	ActionView *pcAction;

	nNextId = nId + 1;
	pcAction = m_vActions[nId];

	/* Check if the selection has been reset I.e. Empty */
	if( pcAction->GetAction() == NONE )
	{
		/* Disable this action */
		pcAction->SetEnable( false );
		pcAction->SetActionEnable( true );

		/* Disable all the subsequent actions */
		for( ; nNextId < (int)m_vActions.size(); nNextId++ )
		{
			pcAction = m_vActions[nNextId];
			pcAction->SetEnable( false );
		}
	}
	else if( nNextId < (int)m_vActions.size() )
	{
		/* Enable the next action */
		pcAction = m_vActions[nNextId];
		pcAction->SetActionEnable( true );
	}
}

void RuleFilterSettingsTab::HandleSelect( void )
{
	int nRuleCount, nSentences, nActionCount, nActionViews, n;
	SentenceView *pcSentence;
	ActionView *pcActionView;
	RuleChain *pcChain;

	m_nCurrentId = m_pcInstances->GetLastSelected();
	if( m_nCurrentId < 0 )
		return;

	pcChain = m_pcFilter->GetChain( m_nCurrentId );
	if( NULL == pcChain )
		return;

	nRuleCount = pcChain->GetRuleCount();
	nSentences = m_vSentences.size();

	/* Disable & clear all the sentences */
	for( n=0; n < nSentences; n++ )
	{
		pcSentence = m_vSentences[n];

		pcSentence->Clear();
		pcSentence->SetEnable( false );
	}

	for( n=0; n < nRuleCount && n < nSentences; n++ )
	{
		Rule *pcRule = pcChain->GetRule( n );
		pcSentence = m_vSentences[n];

		pcSentence->SetConditional( pcRule->GetConditional() );
		pcSentence->SetObject( pcRule->GetObjectName() );
		pcSentence->SetAdjective( pcRule->GetAdjective() );
		pcSentence->SetPattern( pcRule->GetPattern() );

		/* Enable this sentence */
		pcSentence->SetEnable( true );
	}

	/* Enable the subsequent sentence, if we have one available */
	if( n < nSentences )
	{
		pcSentence = m_vSentences[n];
		pcSentence->SetEnable( false );
		pcSentence->SetConditionalEnable( true );
	}

	nActionCount = pcChain->GetActionCount();
	nActionViews = m_vActions.size();

	/* Disable & clear all the action */
	for( n=0; n < nActionCount; n++ )
	{
		pcActionView = m_vActions[n];

		pcActionView->Clear();
		pcActionView->SetEnable( false );
	}

	for( n=0; n < nActionCount && n < nActionViews; n++ )
	{
		RuleAction *pcAction = pcChain->GetAction( n );
		pcActionView = m_vActions[n];

		rule_action_e nAction = pcAction->GetAction();
		pcActionView->SetAction( nAction );
		if( nAction == NONE )
			break;

		switch( nAction )
		{
			case MOVE:
			{
				pcActionView->SetFolder( pcAction->GetFolderName() );
				break;
			}

			case MARK:
			{
				pcActionView->SetStatus( pcAction->GetStatus() );
				break;
			}

			case FLAG:
			{
				pcActionView->SetFlag( pcAction->GetFlag() );
				break;
			}

			default:
			case NONE:
			case DELETE:
			{
				/* Nothing else required */
				break;
			}
		}

		/* Enable this action */
		pcActionView->SetEnable( true );
	}

	m_pcDeleteInstance->SetEnable( true );
	m_pcSave->SetEnable( true );
	m_bNew = false;
}

void RuleFilterSettingsTab::HandleSave( void )
{
	int nSentences, nActions, n;
	SentenceView *pcSentence;
	ActionView *pcActionView;

	nSentences = m_vSentences.size();
	nActions = m_vActions.size();

	/* Build a new RuleChain */
	RuleChain *pcChain = new RuleChain();

	/* Each sentence forms a Rule */
	for( n=0; n < nSentences; n++ )
	{
		Rule *pcRule;
		Rule::object_type_e nObjectType;
		String cObject;

		pcSentence = m_vSentences[n];

		conditional_e nConditional = pcSentence->GetConditional();
		if( nConditional == COND_NONE )
			break;

		pcRule = new Rule();
		pcRule->SetConditional( nConditional );

		cObject = pcSentence->GetObject();
		if( cObject.CompareNoCase( MSG_CFGWND_RULEFILTER_MESSAGETEXT ) == 0 )
			nObjectType = Rule::BODY_TEXT;
		else
			nObjectType = Rule::HEADER;

		pcRule->SetObjectType( nObjectType );
		pcRule->SetObjectName( cObject );

		pcRule->SetAdjective( pcSentence->GetAdjective() );
		pcRule->SetPattern( pcSentence->GetPattern() );

		debug( "%s %s %s %s\n", g_anConditionals[nConditional], cObject.c_str(), get_adjective( pcRule->GetAdjective() ), pcRule->GetPattern().c_str() );

		pcChain->AddRule( pcRule );
	}

	/* Actions */
	for( n=0; n < nActions; n++ )
	{
		RuleAction *pcRuleAction;

		pcActionView = m_vActions[n];

		pcRuleAction = new RuleAction();

		rule_action_e nAction = pcActionView->GetAction();
		switch( nAction )
		{
			case MOVE:
			{
				String cFolder = pcActionView->GetFolder();
				/* XXXKV: Map names to proper folder types! */
				pcRuleAction->SetFolder( FL_TYPE_NORMAL, cFolder );
				break;
			}

			case MARK:
			{
				pcRuleAction->SetStatus( pcActionView->GetStatus() );
				break;
			}

			case FLAG:
			{
				pcRuleAction->SetFlag( pcActionView->GetFlag() );
				break;
			}

			default:
			case NONE:
			case DELETE:
			{
				/* Nothing else required */
				break;
			}
		}
		pcRuleAction->SetAction( nAction );

		pcChain->AddAction( pcRuleAction );
	}

	if( m_bNew )
	{
		/* Add the chain */
		m_pcFilter->AddChain( pcChain );
		AppendChain( pcChain );
	}
	else
	{
		/* Replace the existing chain */
		m_pcFilter->ReplaceChain( m_nCurrentId, pcChain );
	}

	/* Disable & clear all the sentences */
	for( n=0; n < nSentences; n++ )
	{
		pcSentence = m_vSentences[n];

		pcSentence->Clear();
		pcSentence->SetEnable( false );
	}

	/* Disable & clear all the actions */
	for( n=0; n < nActions; n++ )
	{
		pcActionView = m_vActions[n];

		pcActionView->Clear();
		pcActionView->SetEnable( false );
	}

	m_pcDeleteInstance->SetEnable( false );
	m_pcSave->SetEnable( false );
}

void RuleFilterSettingsTab::HandleNew( void )
{
	int nSentences, nActions, n;
	SentenceView *pcSentence;
	ActionView *pcActionView;

	nSentences = m_vSentences.size();

	/* Disable & clear all the sentences */
	for( n=0; n < nSentences; n++ )
	{
		pcSentence = m_vSentences[n];

		pcSentence->Clear();
		pcSentence->SetEnable( false );
	}

	/* Enable the first sentence and the conditional of the second sentence */
	m_vSentences[0]->SetEnable( true );
	m_vSentences[1]->SetConditionalEnable( true );

	nActions = m_vActions.size();

	/* Disable & clear all the actions */
	for( n=0; n < nActions; n++ )
	{
		pcActionView = m_vActions[n];

		pcActionView->Clear();
		pcActionView->SetEnable( false );
	}

	/* Enable the first action */
	m_vActions[0]->SetActionEnable( true );

	m_pcDeleteInstance->SetEnable( false );
	m_pcSave->SetEnable( true );
	m_bNew = true;
}

void RuleFilterSettingsTab::HandleDelete( void )
{
	int nSentences, nActions, n;
	SentenceView *pcSentence;
	ActionView *pcActionView;

	nSentences = m_vSentences.size();

	/* Delete the chain */
	m_pcFilter->RemoveChain( m_nCurrentId );
	m_pcInstances->RemoveRow( m_nCurrentId, true );

	/* Disable & clear all the sentences */
	for( n=0; n < nSentences; n++ )
	{
		pcSentence = m_vSentences[n];

		pcSentence->Clear();
		pcSentence->SetEnable( false );
	}

	nActions = m_vActions.size();

	/* Disable & clear all the actions */
	for( n=0; n < nActions; n++ )
	{
		pcActionView = m_vActions[n];

		pcActionView->Clear();
		pcActionView->SetEnable( false );
	}

	m_pcDeleteInstance->SetEnable( false );
	m_pcSave->SetEnable( false );
}

void RuleFilterSettingsTab::AppendChain( RuleChain *pcChain )
{
	if( pcChain->GetRuleCount() > 0 )
	{
		Rule *pcRule = pcChain->GetRule( 0 );

		String cName;
		cName.Format( "%s %s %s %s", MSG_CFGWND_RULEFILTER_IF.c_str(), pcRule->GetObjectName().c_str(), get_adjective( pcRule->GetAdjective() ), pcRule->GetPattern().c_str() );

		ListViewStringRow *pcRow = new ListViewStringRow();
		pcRow->AppendString( cName );

		m_pcInstances->InsertRow( pcRow ); 
	}
}

status_t RuleFilterSettingsTab::Save( void )
{
	debug( "RuleFilterSettingsTab::Save()\n" );

	m_pcFilter->Save();
	return EOK;
}

