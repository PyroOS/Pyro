#ifndef __REMOTENODE_H__
#define __REMOTENODE_H__

#include <util/string.h>
#include <util/datetime.h>


using namespace os;

/** \brief RemoteNode class.
 * This represents a node of the remote filesystem.
 */
class RemoteNode {
public:
	
	/** \brief Return the file name. */
	String GetName()
	{
		return( m_zName );
	}
	
	/** \brief Return the file path. */
	String GetPath()
	{
		return( m_zPath );
	}
	
	/** \brief Return whether the file is a directory or not.
	 * Returns true if it is a directory.
	 */
	bool IsDir()
	{
		return( m_bIsDir );
	}
	
	/** \brief Return the file size.
	 */
	size_t GetSize()
	{
		return( m_nSize );
	}
	
	/** \brief Return the file permissions. */
	uint32 GetPermissions()
	{
		return( m_nPermissions );
	}
	
	/** \brief Return the timestamp for the file. */
	DateTime& GetTimestamp()
	{
		return( m_cTimestamp );
	}
	
	String m_zName; /**< Name of the file. */
	String m_zPath; /**< Path to the file. */
	bool m_bIsDir; /**< Flag indicating if the file is a directory or not. */
	size_t m_nSize; /**< Size of the file. */
	uint32 m_nPermissions; /**< Permissions for the file. */
	DateTime m_cTimestamp; /**< Modification timestamp of the file. */
};


#endif	/* __REMOTENODE_H__ */

