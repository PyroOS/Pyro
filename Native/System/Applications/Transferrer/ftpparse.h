#ifndef FTPPARSE_H
#define FTPPARSE_H

/*
ftpparse(&fp,buf,len) tries to parse one line of LIST output.

The line is an array of len characters stored in buf.
It should not include the terminating CR LF; so buf[len] is typically CR.

If ftpparse() can't find a filename, it returns 0.

If ftpparse() can find a filename, it fills in fp and returns 1.
fp is a struct ftpparse, defined below.
The name is an array of fp.namelen characters stored in fp.name;
fp.name points somewhere within buf.
*/

/** \brief FTPParse data structure.
 * This stores all the necessary data gathered from parsing a ftp input.
 */
struct ftpparse {
  char *name; /**< Name of the file. Not necessarily 0-terminated. */
  int namelen; /**< Length of name. */
  int flagtrycwd; /**< Used for EPLF types of listings. 0 if cwd is definitely pointless, 1 otherwise */
  int flagtryretr; /**< Used for EPLF types of listings. 0 if retr is definitely pointless, 1 otherwise */
  int sizetype; /**< Number of octets for the type. See FTPPARSE_SIZE_* constants. */
  long size; /**< Number of octets */
  int mtimetype; /**< Integer storing the modification time type. */
  time_t mtime; /**< Modification time */
  int idtype; /**< Type of the id. */
  char *id; /**< ID string. Not necessarily 0-terminated */
  int idlen; /**< Length of the ID string. */
} ;

#define FTPPARSE_SIZE_UNKNOWN 0
#define FTPPARSE_SIZE_BINARY 1 /* size is the number of octets in 'TYPE I' command */
#define FTPPARSE_SIZE_ASCII 2 /* size is the number of octets in 'TYPE A' command */

#define FTPPARSE_MTIME_UNKNOWN 0
#define FTPPARSE_MTIME_LOCAL 1 /* time is correct */
#define FTPPARSE_MTIME_REMOTEMINUTE 2 /* time zone and secs are unknown */
#define FTPPARSE_MTIME_REMOTEDAY 3 /* time zone and time of day are unknown */
/*
When a time zone is unknown, it is assumed to be GMT. You may want
to use localtime() for LOCAL times, along with an indication that the
time is correct in the local time zone, and gmtime() for REMOTE* times.
*/

#define FTPPARSE_ID_UNKNOWN 0
#define FTPPARSE_ID_FULL 1 /* unique identifier for files on this FTP server */

extern int ftpparse(struct ftpparse *,const char *,int);

#endif


