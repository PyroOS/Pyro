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

#ifndef WHISPER_RULE_FILTER_H_
#define WHISPER_RULE_FILTER_H_

#include <filter.h>

#include <util/string.h>

#include <list>
#include <vector>

typedef enum rule_status
{
	RULE_HIT,
	RULE_MISS
} rule_status_e;

typedef enum conditional
{
	COND_NONE,
	COND_IF,
	COND_OR,
	COND_AND
} conditional_e;

typedef enum adjective
{
	NOP,
	EQUALS,
	NOT_EQUALS,
	LIKE,
	NOT_LIKE,
	CONTAINS,
	NOT_CONTAINS
} adjective_e;

class RuleFilter;

class Rule
{
	public:
		Rule( void ){};
		~Rule( void ){};

		rule_status_e Test( Mailmessage *pcMessage );

		typedef enum object_type
		{
			HEADER,
			BODY_TEXT,
			ATTACHMENT
		} object_type_e;

		void SetConditional( conditional_e nConditional )
		{
			m_nConditional = nConditional;
		};
		conditional_e GetConditional( void )
		{
			return m_nConditional;
		};

		void SetObjectType( object_type_e nObjectType )
		{
			m_nObjectType = nObjectType;
		};
		object_type_e GetObjectType( void )
		{
			return m_nObjectType;
		};

		void SetObjectName( os::String cObjectName )
		{
			m_cObjectName = cObjectName;
		};
		os::String GetObjectName( void )
		{
			return m_cObjectName;
		};

		void SetAdjective( adjective_e nAdjective )
		{
			m_nAdjective = nAdjective;
		};
		adjective_e GetAdjective( void )
		{
			return m_nAdjective;
		};

		void SetPattern( os::String cPattern )
		{
			m_cPattern = cPattern;
		};
		os::String GetPattern( void )
		{
			return m_cPattern;
		};

	private:
		conditional_e m_nConditional;
		object_type_e m_nObjectType;
		os::String m_cObjectName;
		adjective_e m_nAdjective;
		os::String m_cPattern;

		rule_status_e TestHeader( Mailmessage *pcMessage );
		rule_status_e TestBody( Mailmessage *pcMessage );

		rule_status_e TestObject( os::String cObject );
		rule_status_e TestObjectEquals( os::String cHeader );
		rule_status_e TestObjectLike( os::String cHeader );
		rule_status_e TestObjectContains( os::String cHeader );

		friend class RuleFilter;
};

typedef enum rule_action
{
	NONE,
	MOVE,
	MARK,
	FLAG,
	DELETE
} rule_action_e;

class RuleAction
{
	public:
		RuleAction( void ){};
		~RuleAction( void ){};

		void Apply( Mailmessage *pcMessage, FolderReference &cReference );

		void SetAction( rule_action_e eAction )
		{
			m_eAction = eAction;
		};
		rule_action_e GetAction( void )
		{
			return m_eAction;
		};

		void SetStatus( int32 nStatus )
		{
			m_nStatus = nStatus;
		}
		int32 GetStatus( void )
		{
			return m_nStatus;
		};

		void SetFlag( int32 nFlag )
		{
			m_nFlag = nFlag;
		};
		int32 GetFlag( void )
		{
			return m_nFlag;
		};

		void SetFolder( folder_type_e eType, os::String cName )
		{
			m_eFolderType = eType;
			m_cFolderName = cName;
		}
		folder_type_e GetFolderType( void )
		{
			return m_eFolderType;
		};
		os::String GetFolderName( void )
		{
			return m_cFolderName;
		};

	private:
		rule_action_e m_eAction;

		int32 m_nStatus;
		int32 m_nFlag;

		folder_type_e m_eFolderType;
		os::String m_cFolderName;

		friend class RuleFilter;
};

class RuleChain
{
	public:
		RuleChain( void ){};
		~RuleChain( void );

		void AddRule( Rule *pcRule )
		{
			m_vRules.push_back( pcRule );
		};

		void AddAction( RuleAction *pcAction )
		{
			m_vActions.push_back( pcAction );
		};

		rule_status_e Test( Mailmessage *pcMessage );
		void ApplyActions( Mailmessage *pcMessage, FolderReference &cReference );

	protected:
		size_t GetRuleCount( void )
		{
			return m_vRules.size();
		};
		Rule * GetRule( int nIndex )
		{
			/* Bounds check? */
			return m_vRules[nIndex];
		};

		size_t GetActionCount( void )
		{
			return m_vActions.size();
		};
		RuleAction * GetAction( int nIndex )
		{
			/* Bounds check? */
			return m_vActions[nIndex];
		};

	private:
		std::vector <Rule*> m_vRules;
		std::vector <RuleAction*> m_vActions;

		friend class RuleFilter;
		friend class RuleFilterSettingsTab;
};

/* Data stream */
struct rule_file_header
{
	uint32_t nMagic;
	uint8_t nVersion;
	uint32_t nChainCount;
};
#define RULE_FILE_MAGIC	0x09f91102
#define RULE_FILE_VERSION	1

struct chain_header
{
	uint32_t nRuleCount;
	uint32_t nActionCount;
};

struct rule_header
{
	uint32_t nMagic;
	uint32_t nSize;
};
#define RULE_HEADER_MAGIC	0x9d74e35b

struct action_header
{
	uint32_t nMagic;
	uint32_t nSize;
};
#define ACTION_HEADER_MAGIC	0xd84156c5

struct object_header
{
	uint8_t nType;
	uint32_t nSize;
};
#define OBJECT_TYPE_INT32	0
#define OBJECT_TYPE_STRING	1

class RuleFilterSettingsTab;

class RuleFilter : public Filter
{
	public:
		RuleFilter( void );
		virtual ~RuleFilter( void );

		virtual status_t FilterMessage( Mailmessage *pcMessage, FolderReference &cReference );

		virtual os::String GetIdentifier ( void ) const{return "Rules";};
		virtual SettingsTab * GetSettingsView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow );

	protected:
		std::vector <RuleChain*> m_vChains;

		int GetChainCount( void )
		{
			return m_vChains.size();
		};

		RuleChain * GetChain( int nIndex )
		{
			return m_vChains[nIndex];
		};

		void AddChain( RuleChain *pcChain )
		{
			m_vChains.push_back( pcChain );
		};
		void ReplaceChain( int nIndex, RuleChain *pcChain )
		{
			delete( m_vChains[nIndex] );
			m_vChains[nIndex] = pcChain;
		};
		void RemoveChain( int nIndex );

		status_t Save( void );

	private:
		status_t Load( void );
		status_t LoadChain( int nFd );

		status_t SaveChain( int nFd, RuleChain *pcChain );

		size_t Flatten( uint8 *pBuffer, int32 nVal );
		size_t Flatten( uint8 *pBuffer, os::String cString );

		int8 Unflatten( int nFd, int32 &nVal, os::String &cString );
		int32 UnflattenInt( int nFd );
		os::String UnflattenString( int nFd, ssize_t nSize );

		friend class RuleFilterSettingsTab;
};

/* Configuration View */
#include <gui/layoutview.h>
#include <gui/stringview.h>
#include <gui/dropdownmenu.h>
#include <gui/textview.h>
#include <gui/button.h>

enum rule_filter_tab_messages{
	ID_CONDITIONAL_CHANGED,
	ID_RULE_SAVE,
	ID_RULE_SELECT,
	ID_RULE_NEW,
	ID_RULE_DELETE,
	ID_ACTION_CHANGED
};

class SentenceView : public LayoutView
{
	public:
		SentenceView( const Rect cFrame, const int nId );
		~SentenceView();

		virtual void AllAttached();

		void SetEnable( bool bEnable = true );
		void SetConditionalEnable( bool bEnable = true );

		conditional_e GetConditional( void );
		void SetConditional( conditional_e nConditional );

		String GetObject( void );
		void SetObject( String cObject );

		adjective_e GetAdjective( void );
		void SetAdjective( adjective_e nAdjective );

		String GetPattern( void );
		void SetPattern( String cPattern );

		void Clear( void );
	private:
		VLayoutNode *m_pcVLayoutNode;
		HLayoutNode *m_pcHLayoutNode;

		StringView *m_pcIf;
		DropdownMenu *m_pcConditional;
		DropdownMenu *m_pcObject;
		DropdownMenu *m_pcAdjective;
		TextView *m_pcPattern;
};

class ActionView : public LayoutView
{
	public:
		ActionView( const Rect cFrame, const int nId, const Handler *pcMainWindow );
		~ActionView();

		virtual void AllAttached();
		virtual void HandleMessage( Message *pcMessage );

		void SetEnable( bool bEnable = true );
		void SetActionEnable( bool bEnable = true );

		void SetAction( rule_action_e nAction );
		rule_action_e GetAction( void );

		void SetStatus( int32 nStatus );
		int32 GetStatus( void );

		void SetFlag( int32 nFlag );
		int32 GetFlag( void );

		void SetFolder( String cName );
		String GetFolder( void );

		void Clear( void );
	private:
		void PopulateTarget( int nType );

		Messenger *m_pcParentMessenger;
		Messenger *m_pcMainMessenger;

		HLayoutNode *m_pcHLayoutNode;

		StringView *m_pcTheMessage;
		DropdownMenu *m_pcAction;
		DropdownMenu *m_pcTarget;
};

class RuleFilterSettingsTab : public SettingsTab
{
	public:
		RuleFilterSettingsTab( const Rect cFrame, RuleFilter *pcFilter, Identity *pcIdentity, Handler *pcMainWindow );
		virtual ~RuleFilterSettingsTab();

		virtual void AllAttached();
		virtual void HandleMessage( Message *pcMessage );

		virtual status_t Save( void );

	private:
		void HandleConditionalChanged( int nId );
		void HandleActionChanged( int nId );
		void HandleSelect( void );
		void HandleSave( void );
		void HandleNew( void );
		void HandleDelete( void );

		void AppendChain( RuleChain *pcChain );

		RuleFilter *m_pcFilter;
		std::vector <SentenceView *> m_vSentences;
		std::vector <ActionView *> m_vActions;

		int m_nCurrentId;
		bool m_bNew;
		Button *m_pcSave;

		LayoutView *m_pcLayoutView;
		VLayoutNode *m_pcLayoutRoot;

		HLayoutNode *m_pcInstancesLayout;

		ListView *m_pcInstances;
		VLayoutNode *m_pcButtonsLayout;
		Button *m_pcNewInstance;
		Button *m_pcDeleteInstance;

		friend class RuleFilter;
		friend class RuleChain;
};

#endif

