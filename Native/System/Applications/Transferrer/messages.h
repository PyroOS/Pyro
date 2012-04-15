#ifndef MESSAGES_H
#define MESSAGES_H


/* Types of job, for GUI use */
enum {
	JOB_DOWNLOAD,
	JOB_UPLOAD,
	JOB_DIRLIST,
	JOB_COMMAND,
};

/* User responses from requesters */
enum {
	RESPONSE_YES,
	RESPONSE_NO,
	RESPONSE_CANCEL
};

enum
{
	/* Main window GUI messages from menus, etc */
	M_APP_QUIT = 1,
	M_APP_ABOUT,
	M_CONNECT,
	M_LOCAL_PATH_BAR_INVOKED,
	M_REMOTE_PATH_BAR_INVOKED,
	M_LOCAL_DIR_CHANGED,
	M_REMOTE_DIR_CHANGED,
	M_SETTINGS_MENU,
	M_SETTINGS_PASSIVE,
	M_SETTINGS_MAX_CONNECTIONS_MENU,
	M_SETTINGS_MAX_CONNECTIONS,
	M_SETTINGS_SAVE_HISTORY,
	M_SETTINGS_SAVE_PASSWORDS,
	M_SETTINGS_DEBUG_MODE,
	
	/* Remote view context menu actions */
	M_REMOTE_MKDIR = 100,
	M_REMOTE_DELETE,
	M_REMOTE_RENAME,
	M_REMOTE_MOVE,
	
	/* Messages from confirmation dialogs, etc */
	M_DELETE_CONFIRMED = 200,
	M_MKDIR_CONFIRMED,
	M_RENAME_CONFIRMED,
	M_YES,
	M_NO,
	
	/* Messages sent from backend to GUI */
	M_REMOTE_DIRLISTING = 300,
	M_ENTRY_PATH,
	M_JOB_UPDATED,
	M_JOB_ERROR,
	M_JOB_OVERWRITE_QUERY,
	
	/* Messages sent to the transfer thread */
	M_TT_CLOSE = 500,
	M_TT_ADD,
	M_TT_PAUSE,
	M_TT_RESUME,
	M_TT_CANCEL,
	M_TT_REMOVE,
	M_TT_SCHEDULE,
		
	/* Messages sent between progress window, requesters and Main Window */
	M_GUI_PAUSE = 600,	/**< Message for pausing a specific download. */
	M_GUI_RESUME,		/**< Message for resuming a specific download. */
	M_GUI_CANCEL,		/**< Message for canceling a specific download. */
	
	/* Messages sent between requesters and backend */
	M_GUI_OVERWRITE_REPLY = 700,
	M_GUI_FAILURE_REPLY,
	
	/* Internal ProgressWindow GUI messages */
	M_PW_RESUME_BUTTON = 1000,
	M_PW_PAUSE_BUTTON,
	M_PW_CANCEL_BUTTON
};

#endif
