                                                                                                                                                                                                       
// Hex Editor - Copyright 2007 Andrew Kennan
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef __F_CAT_HexEditor_H__
#define __F_CAT_HexEditor_H__
#include <util/catalog.h>
#ifndef CATALOG_NO_IDS
#define ID_STR_APP_NAME 1000
#define ID_STR_ABOUT_DLG_TITLE 1001
#define ID_STR_ABOUT_DLG_MSG 1002
#define ID_STR_ABOUT_DLG_BTN_CLOSE 1003
#define ID_STR_FMT_LOADED 1004
#define ID_STR_MSG_SAVED 1005
#define ID_STR_CLOSE_DLG_TITLE 1006
#define ID_STR_CLOSE_DLG_MSG_PREFIX 1007
#define ID_STR_CLOSE_DLG_MSG_SUFFIX 1008
#define ID_STR_CLOSE_DLG_BTN_DISCARD 1009
#define ID_STR_CLOSE_DLG_BTN_SAVE 1010
#define ID_STR_CLOSE_DLG_BTN_CANCEL 1011
#define ID_STR_CHANGE_DLG_TITLE 1012
#define ID_STR_CHANGE_DLG_MSG_PREFIX 1013
#define ID_STR_CHANGE_DLG_MSG_SUFFIX 1014
#define ID_STR_CHANGE_DLG_MSG_LOST 1015
#define ID_STR_CHANGE_DLG_BTN_LOAD 1016
#define ID_STR_CHANGE_DLG_BTN_IGNORE 1017
#define ID_STR_FIND_DLG_TITLE 1018
#define ID_STR_FIND_DLG_LBL_SEARCH_FOR 1019
#define ID_STR_FIND_DLG_LBL_SEARCH_TYPE 1020
#define ID_STR_FIND_DLG_BTN_NEXT 1021
#define ID_STR_FIND_DLG_BTN_PREV 1022
#define ID_STR_FIND_DLG_BTN_CLOSE 1023
#define ID_STR_MNU_APP 1024
#define ID_STR_MNU_APP_ABOUT 1025
#define ID_STR_MNU_APP_QUIT 1026
#define ID_STR_MNU_FILE 1027
#define ID_STR_MNU_FILE_OPEN 1028
#define ID_STR_MNU_FILE_SAVE 1029
#define ID_STR_MNU_FILE_SAVE_AS 1030
#define ID_STR_MNU_FILE_CLOSE 1031
#define ID_STR_MNU_EDIT 1032
#define ID_STR_MNU_EDIT_UNDO 1033
#define ID_STR_MNU_EDIT_REDO 1034
#define ID_STR_MNU_EDIT_COPY 1035
#define ID_STR_MNU_EDIT_SELECT_ALL 1036
#define ID_STR_MNU_EDIT_SELECT_NONE 1037
#define ID_STR_MNU_EDIT_FIND 1038
#define ID_STR_MNU_HELP 1039
#define ID_STR_TB_OPEN 1040
#define ID_STR_TB_SAVE 1041
#define ID_STR_TB_SAVE_AS 1042
#define ID_STR_TB_COPY 1043
#define ID_STR_TB_FIND 1044
#define ID_STR_FMT_CURSOR 1045
#define ID_STR_MSG_SEARCH_NOT_FOUND 1046
#define ID_STR_MSG_INVALID_HEX_STRING 1047
#define ID_STR_MSG_SEARCH_CONTINUE_TOP 1048
#define ID_STR_MSG_SEARCH_CONTINUE_BOTTOM 1049
#endif
#ifndef CATALOG_NO_LSTRINGS
#define STR_APP_NAME os::LString( 1000 )
#define STR_ABOUT_DLG_TITLE os::LString( 1001 )
#define STR_ABOUT_DLG_MSG os::LString( 1002 )
#define STR_ABOUT_DLG_BTN_CLOSE os::LString( 1003 )
#define STR_FMT_LOADED os::LString( 1004 )
#define STR_MSG_SAVED os::LString( 1005 )
#define STR_CLOSE_DLG_TITLE os::LString( 1006 )
#define STR_CLOSE_DLG_MSG_PREFIX os::LString( 1007 )
#define STR_CLOSE_DLG_MSG_SUFFIX os::LString( 1008 )
#define STR_CLOSE_DLG_BTN_DISCARD os::LString( 1009 )
#define STR_CLOSE_DLG_BTN_SAVE os::LString( 1010 )
#define STR_CLOSE_DLG_BTN_CANCEL os::LString( 1011 )
#define STR_CHANGE_DLG_TITLE os::LString( 1012 )
#define STR_CHANGE_DLG_MSG_PREFIX os::LString( 1013 )
#define STR_CHANGE_DLG_MSG_SUFFIX os::LString( 1014 )
#define STR_CHANGE_DLG_MSG_LOST os::LString( 1015 )
#define STR_CHANGE_DLG_BTN_LOAD os::LString( 1016 )
#define STR_CHANGE_DLG_BTN_IGNORE os::LString( 1017 )
#define STR_FIND_DLG_TITLE os::LString( 1018 )
#define STR_FIND_DLG_LBL_SEARCH_FOR os::LString( 1019 )
#define STR_FIND_DLG_LBL_SEARCH_TYPE os::LString( 1020 )
#define STR_FIND_DLG_BTN_NEXT os::LString( 1021 )
#define STR_FIND_DLG_BTN_PREV os::LString( 1022 )
#define STR_FIND_DLG_BTN_CLOSE os::LString( 1023 )
#define STR_MNU_APP os::LString( 1024 )
#define STR_MNU_APP_ABOUT os::LString( 1025 )
#define STR_MNU_APP_QUIT os::LString( 1026 )
#define STR_MNU_FILE os::LString( 1027 )
#define STR_MNU_FILE_OPEN os::LString( 1028 )
#define STR_MNU_FILE_SAVE os::LString( 1029 )
#define STR_MNU_FILE_SAVE_AS os::LString( 1030 )
#define STR_MNU_FILE_CLOSE os::LString( 1031 )
#define STR_MNU_EDIT os::LString( 1032 )
#define STR_MNU_EDIT_UNDO os::LString( 1033 )
#define STR_MNU_EDIT_REDO os::LString( 1034 )
#define STR_MNU_EDIT_COPY os::LString( 1035 )
#define STR_MNU_EDIT_SELECT_ALL os::LString( 1036 )
#define STR_MNU_EDIT_SELECT_NONE os::LString( 1037 )
#define STR_MNU_EDIT_FIND os::LString( 1038 )
#define STR_MNU_HELP os::LString( 1039 )
#define STR_TB_OPEN os::LString( 1040 )
#define STR_TB_SAVE os::LString( 1041 )
#define STR_TB_SAVE_AS os::LString( 1042 )
#define STR_TB_COPY os::LString( 1043 )
#define STR_TB_FIND os::LString( 1044 )
#define STR_FMT_CURSOR os::LString( 1045 )
#define STR_MSG_SEARCH_NOT_FOUND os::LString( 1046 )
#define STR_MSG_INVALID_HEX_STRING os::LString( 1047 )
#define STR_MSG_SEARCH_CONTINUE_TOP os::LString( 1048 )
#define STR_MSG_SEARCH_CONTINUE_BOTTOM os::LString( 1049 )
#endif
#endif /* __F_CAT_resources/HexEditor_H__ */

