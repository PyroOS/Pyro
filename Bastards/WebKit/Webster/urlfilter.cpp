/* Webster (C)opyright	2008 Kristian Van Der Vliet
 * 						2004-2007 Arno Klenke
 *						2001 Kurt Skauen
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <urlfilter.h>

using namespace os;

url_protocol_e UrlFilter::Filter( String &cURL )
{
	url_protocol_e nProtocol = URL_IS_UNKNOWN;

	/* Do some obvious checks first */
	if( cURL.empty() || cURL == "about:blank" )
		nProtocol = URL_IS_EMPTY;
	else if( cURL.substr( 0, 7 ).Lower() == "http://" )
		nProtocol = URL_IS_HTTP;
	else if( cURL.substr( 0, 8 ).Lower() == "https://" )
		nProtocol = URL_IS_HTTPS;
	else if( cURL.substr( 0, 6 ).Lower() == "ftp://" )
		nProtocol = URL_IS_FTP;
	else if( cURL.substr( 0, 7 ).Lower() == "file://" )
		nProtocol = URL_IS_FILE;
	else
	{
		/* O.K, we didn't find a protocol so try to work out if a protocol is there at all */
		ssize_t nPos;

		nPos = (ssize_t)cURL.find( ":" );
		if( nPos < 0 )
		{
			/* No protocol already. Perhaps it's a path to a file? */
			nPos = (ssize_t)cURL.find( "/" );
			if( nPos == 0 )
			{
				/* Transfom the URL into a (hopefully valid) FILE URL */
				cURL = String( "file://" ) + cURL;
				nProtocol = URL_IS_FILE;
			}
			else
			{
				/* Does this look like a URL at all? */
				nPos = (ssize_t)cURL.find( "." );
				if( nPos >= 0 )
				{
					/* Transfom the URL into a (hopefully valid) HTTP URL */
					cURL = String( "http://" ) + cURL;
					nProtocol = URL_IS_HTTP;
				}
				else
				{
					/* Let's try building a search query */
					cURL = String( "http://www.google.com/search?q=" ) + cURL + String( "&btng=google+search&meta=" );
					nProtocol = URL_IS_HTTP;

					/* XXXKV: For now this is hardwired to a non-partner Google search, but
					   eventually it should be configurable by the user */
				}
			}
		}
	}

	return nProtocol;
}
