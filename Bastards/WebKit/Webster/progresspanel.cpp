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

#include <util/looper.h>

#include <progresspanel.h>

using namespace os;

class Progress : public View
{
	public:
		Progress( Rect cFrame, String cName ) : View( cFrame, cName )
		{
			m_nProgress = 0;
			m_bProgress = false;
			SetEraseColor( get_default_color( COL_SHINE ) );
		}
		void Paint( const Rect& cUpdate )
		{
			if( !m_bProgress )
				EraseRect( cUpdate );
			else
			{
				float vX = m_nProgress - 6;
			
				SetFgColor( get_default_color( COL_SEL_WND_BORDER ) );
			
				Color32_s sDefaultColor = get_default_color( COL_SEL_WND_BORDER );
				while( vX < GetBounds().Width() + 13 )
				{
					for( int i = -6; i < 7; i++ )
					{
						Color32_s sColor;
						sColor.red = sDefaultColor.red + ( 255 - sDefaultColor.red ) * abs( i ) / 6;
						sColor.blue = sDefaultColor.blue + ( 255 - sDefaultColor.blue ) * abs( i ) / 6;
						sColor.green = sDefaultColor.green + ( 255 - sDefaultColor.green ) * abs( i ) / 6;
				
						SetFgColor( sColor );
						FillRect( Rect( vX + i, 0, vX + i, GetBounds().Height() ) );
					}
					vX += 13;
				}
			}
		}
		void SetProgress( bool bProgress )
		{
			if( bProgress == false && m_bProgress == false )
				return;
			m_bProgress = bProgress;
			m_nProgress += 1;
			m_nProgress %= 13;
			Invalidate();
			Flush();
		}
		void UpdateProgress( void )
		{
			m_nProgress += 1;
			m_nProgress %= 13;
			Invalidate();
			Flush();
		}

	private:
		int m_nProgress;
		bool m_bProgress;
};

class ProgressPanel::ProgressLooper : public Looper
{
	public:
		ProgressLooper( String cName, Progress *pcProgress ) : Looper( cName )
		{
			m_pcProgress = pcProgress;
			AddTimer( this, 0, 100000, false );
		}
		~ProgressLooper()
		{
			RemoveTimer( this, 0 );
			delete m_pcProgress;
		}
		void TimerTick( int )
		{
			m_pcProgress->UpdateProgress();
		}

	private:
		Progress *m_pcProgress;
};

ProgressPanel::ProgressPanel( String cName, double vSize ) : StatusPanel( cName, "", vSize, StatusPanel::F_FIXED )
{
	m_pcProgress = new Progress( Rect(), cName );
	AddChild( m_pcProgress );

	m_pcLooper = new ProgressLooper( "progresspanel_looper", m_pcProgress );
	m_pcLooper->Run();
}

ProgressPanel::~ProgressPanel()
{
	RemoveChild( m_pcProgress );
	m_pcLooper->PostMessage( M_TERMINATE );
}

void ProgressPanel::FrameSized( const Point &cDelta )
{
	Rect cBounds = GetBounds();
	cBounds.top += 2;
	cBounds.left += 2;
	cBounds.right -= 2;
	cBounds.bottom -= 2;

	m_pcProgress->SetFrame( cBounds );
}

void ProgressPanel::SetProgress( bool bInProgress )
{
	m_pcProgress->SetProgress( bInProgress );
}

