#include "../Cheat.h"

HFont F::Meme;
HFont F::ESP;
HFont F::Watermark;
HFont F::Revue;

void D::SetupFonts()
{
	//"Courier New", 14, 450, 0, 0, FONTFLAG_OUTLINE, 0, 0); <- STYLES FONT
	I::Surface->SetFontGlyphSet(F::Revue = I::Surface->Create_Font(), charenc("Revue"), 24, FW_DONTCARE, NULL, NULL, FONTFLAG_ANTIALIAS);
	I::Surface->SetFontGlyphSet(F::Meme = I::Surface->Create_Font(), charenc("MS Sans Serif"), 10, FW_DONTCARE, NULL, NULL, FONTFLAG_ANTIALIAS);
	I::Surface->SetFontGlyphSet(F::ESP = I::Surface->Create_Font(), charenc("Verdana"), 12, 800, NULL, NULL, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	I::Surface->SetFontGlyphSet(F::Watermark = I::Surface->Create_Font(), charenc("MS Sans Serif"), 12, FW_DONTCARE, NULL, NULL, FONTFLAG_OUTLINE);
}

void D::DrawString( HFont font, int x, int y, Color color, DWORD alignment, const char* msg, ... )
{
	va_list va_alist;
	char buf[1024];
	va_start( va_alist, msg );
	_vsnprintf( buf, sizeof( buf ), msg, va_alist );
	va_end( va_alist );
	wchar_t wbuf[1024];
	MultiByteToWideChar( CP_UTF8, 0, buf, 256, wbuf, 256 );

	int r = 255, g = 255, b = 255, a = 255; 
	color.GetColor( r, g, b, a );

	int width, height;
	I::Surface->GetTextSize( font, wbuf, width, height );

	if( alignment & FONT_RIGHT )
		x -= width;
	if( alignment & FONT_CENTER )
		x -= width / 2;

	I::Surface->DrawSetTextFont( font );
	I::Surface->DrawSetTextColor( r, g, b, a );
	I::Surface->DrawSetTextPos( x, y - height / 2 );
	I::Surface->DrawPrintText( wbuf, wcslen( wbuf ) );
}

void D::DrawStringUnicode( HFont font, int x, int y, Color color, bool bCenter, const wchar_t* msg, ... )
{
	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor( r, g, b, a );

	int iWidth, iHeight;

	I::Surface->GetTextSize( font, msg, iWidth, iHeight );
	I::Surface->DrawSetTextFont( font );
	I::Surface->DrawSetTextColor( r, g, b, a );
	I::Surface->DrawSetTextPos( !bCenter ? x : x - iWidth / 2, y - iHeight / 2 );
	I::Surface->DrawPrintText( msg, wcslen( msg ) );
}

void D::DrawRect( int x, int y, int w, int h, Color col )
{
	I::Surface->DrawSetColor( col );
	I::Surface->DrawFilledRect( x, y, x + w, y + h );
}

void D::Rectangle(float x, float y, float w, float h, float px, Color col)
{
	DrawRect(x, (y + h - px), w, px, col);
	DrawRect(x, y, px, h, col);
	DrawRect(x, y, w, px, col);
	DrawRect((x + w - px), y, px, h, col);
}

void D::Border(int x, int y, int w, int h, int line, Color col)
{
	DrawRect(x, y, w, line, col);
	DrawRect(x, y, line, h, col);
	DrawRect((x + w), y, line, h, col);
	DrawRect(x, (y + h), (w + line), line, col);
}

void D::DrawRectRainbow( int x, int y, int width, int height, float flSpeed, float &flRainbow )
{
	Color colColor( 0, 0, 0 );

	flRainbow += flSpeed;
	if ( flRainbow > 1.f ) flRainbow = 0.f;

	for ( int i = 0; i < width; i++ )
	{
		float hue = ( 1.f / ( float ) width ) * i;
		hue -= flRainbow;
		if ( hue < 0.f ) hue += 1.f;

		Color colRainbow = colColor.FromHSB( hue, 1.f, 1.f );
		D::DrawRect( x + i, y, 1, height, colRainbow );
	}
}

void D::DrawRectGradientHorizontal( int x, int y, int width, int height, Color color1, Color color2 )
{
	float flDifferenceR = ( float ) ( color2.r( ) - color1.r( ) ) / ( float ) width;
	float flDifferenceG = ( float ) ( color2.g( ) - color1.g( ) ) / ( float ) width;
	float flDifferenceB = ( float ) ( color2.b( ) - color1.b( ) ) / ( float ) width;

	for ( float i = 0.f; i < width; i++ )
	{
		Color colGradient = Color( color1.r( ) + ( flDifferenceR * i ), color1.g( ) + ( flDifferenceG * i ), color1.b( ) + ( flDifferenceB * i ), color1.a( ) );
		D::DrawRect( x + i, y, 1, height, colGradient );
	}
}

void D::DrawPixel( int x, int y, Color col )
{
	I::Surface->DrawSetColor( col );
	I::Surface->DrawFilledRect( x, y, x + 1, y + 1 );
}

void D::DrawOutlinedRect( int x, int y, int w, int h, Color col )
{
	I::Surface->DrawSetColor( col );
	I::Surface->DrawOutlinedRect( x, y, x + w, y + h );
}

void D::DrawOutlinedCircle( int x, int y, int r, Color col )
{
	I::Surface->DrawSetColor( col );
	I::Surface->DrawOutlinedCircle( x, y, r, 1 );
}

void D::DrawLine( int x0, int y0, int x1, int y1, Color col )
{
	I::Surface->DrawSetColor( col );
	I::Surface->DrawLine( x0, y0, x1, y1 );
}

void D::DrawCorner( int iX, int iY, int iWidth, int iHeight, bool bRight, bool bDown, Color colDraw )
{
	int iRealX = bRight ? iX - iWidth : iX;
	int iRealY = bDown ? iY - iHeight : iY;

	if ( bDown && bRight )
		iWidth = iWidth + 1;

	D::DrawRect( iRealX, iY, iWidth, 1, colDraw );
	D::DrawRect( iX, iRealY, 1, iHeight, colDraw );

	D::DrawRect( iRealX, bDown ? iY + 1 : iY - 1, !bDown && bRight ? iWidth + 1 : iWidth, 1, Color( 0, 0, 0, 255 ) );
	D::DrawRect( bRight ? iX + 1 : iX - 1, bDown ? iRealY : iRealY - 1, 1, bDown ? iHeight + 2 : iHeight + 1, Color( 0, 0, 0, 255 ) );
}

void D::DrawPolygon( int count, Vertex_t* Vertexs, Color color )
{
	static int Texture = I::Surface->CreateNewTextureID( true );
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	I::Surface->DrawSetTextureRGBA( Texture, buffer, 1, 1 );
	I::Surface->DrawSetColor( color );
	I::Surface->DrawSetTexture( Texture );

	I::Surface->DrawTexturedPolygon( count, Vertexs );
}

void D::DrawBox(int x, int y, int w, int h, Color color)
{
	// top
	DrawRect(x, y, w, 1, color);
	// right
	DrawRect(x, y + 1, 1, h - 1, color);
	// left
	DrawRect(x + w - 1, y + 1, 1, h - 1, color);
	// bottom
	DrawRect(x, y + h - 1, w - 1, 1, color);
}

void D::DrawRoundedBox( int x, int y, int w, int h, int r, int v, Color col )
{
	std::vector<Vertex_t> p;
	for ( int _i = 0; _i < 3; _i++ )
	{
		int _x = x + ( _i < 2 && r || w - r );
		int _y = y + ( _i % 3 > 0 && r || h - r );
		for ( int i = 0; i < v; i++ )
		{
			int a = RAD2DEG( ( i / v ) * -90 - _i * 90 );
			p.push_back( Vertex_t( Vector2D( _x + sin( a ) * r, _y + cos( a ) * r ) ) );
		}
	}

	D::DrawPolygon( 4 * ( v + 1 ), &p[0], col );
	/*
	function DrawRoundedBox(x, y, w, h, r, v, col)
	local p = {};
	for _i = 0, 3 do
	local _x = x + (_i < 2 && r || w - r)
	local _y = y + (_i%3 > 0 && r || h - r)
	for i = 0, v do
	local a = math.rad((i / v) * - 90 - _i * 90)
	table.insert(p, {x = _x + math.sin(a) * r, y = _y + math.cos(a) * r})
	end
	end

	surface.SetDrawColor(col.r, col.g, col.b, 255)
	draw.NoTexture()
	surface.DrawPoly(p)
	end
	*/

	// Notes: amount of vertexes is 4(v + 1) where v is the number of vertices on each corner bit.
	// I did it in lua cause I have no idea how the vertex_t struct works and i'm still aids at C++
}

bool D::ScreenTransform( const Vector &point, Vector &screen ) // tots not pasted
{
	float w;
	const VMatrix &worldToScreen = I::Engine->WorldToScreenMatrix( );

	screen.x = worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3];
	screen.y = worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3];
	w = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];
	screen.z = 0.0f;

	bool behind = false;

	if ( w < 0.001f )
	{
		behind = true;
		screen.x *= 100000;
		screen.y *= 100000;
	}
	else
	{
		behind = false;
		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;
	}

	return behind;
}

bool D::WorldToScreen( const Vector &origin, Vector &screen )
{
	if ( !ScreenTransform( origin, screen ) )
	{
		int ScreenWidth, ScreenHeight;
		I::Engine->GetScreenSize( ScreenWidth, ScreenHeight );
		float x = ScreenWidth / 2;
		float y = ScreenHeight / 2;
		x += 0.5 * screen.x * ScreenWidth + 0.5;
		y -= 0.5 * screen.y * ScreenHeight + 0.5;
		screen.x = x;
		screen.y = y;
		return true;
	}

	return false;
}

RECT D::GetViewport()
{
	RECT Viewport = { 0, 0, 0, 0 };
	int w, h;
	I::Engine->GetScreenSize(w, h);
	Viewport.right = w; Viewport.bottom = h;
	return Viewport;
}

int D::GetStringWidth( HFont font, const char* msg, ... )
{
	va_list va_alist;
	char buf[1024];
	va_start( va_alist, msg );
	_vsnprintf( buf, sizeof( buf ), msg, va_alist );
	va_end( va_alist );
	wchar_t wbuf[1024];
	MultiByteToWideChar( CP_UTF8, 0, buf, 256, wbuf, 256 );

	int iWidth, iHeight;

	I::Surface->GetTextSize( font, wbuf, iWidth, iHeight );

	return iWidth;
}

RECT D::GetTextSize( HFont font, const char* text)
{
	size_t origsize = strlen(text) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

	RECT rect; int x, y;
	I::Surface->GetTextSize(font, wcstring, x, y);
	rect.left = x; rect.bottom = y;
	rect.right = x;
	return rect;
}

void D::Draw3DBox( Vector* boxVectors, Color color )
{
	Vector boxVectors0, boxVectors1, boxVectors2, boxVectors3,
		boxVectors4, boxVectors5, boxVectors6, boxVectors7;


	if( D::WorldToScreen( boxVectors[ 0 ], boxVectors0 ) &&
		D::WorldToScreen( boxVectors[ 1 ], boxVectors1 ) &&
		D::WorldToScreen( boxVectors[ 2 ], boxVectors2 ) &&
		D::WorldToScreen( boxVectors[ 3 ], boxVectors3 ) &&
		D::WorldToScreen( boxVectors[ 4 ], boxVectors4 ) &&
		D::WorldToScreen( boxVectors[ 5 ], boxVectors5 ) &&
		D::WorldToScreen( boxVectors[ 6 ], boxVectors6 ) &&
		D::WorldToScreen( boxVectors[ 7 ], boxVectors7 ) )
	{

		/*
			   .+--5---+
			 .8 4    6'|
			+--7---+'  11
			9   |  10  |
			|  ,+--|3--+
			|.0    | 2'
			+--1---+'
		*/

		Vector2D lines[ 12 ][ 2 ];
		//bottom of box
		lines[ 0 ][ 0 ] = { boxVectors0.x, boxVectors0.y };
		lines[ 0 ][ 1 ] = { boxVectors1.x, boxVectors1.y };
		lines[ 1 ][ 0 ] = { boxVectors1.x, boxVectors1.y };
		lines[ 1 ][ 1 ] = { boxVectors2.x, boxVectors2.y };
		lines[ 2 ][ 0 ] = { boxVectors2.x, boxVectors2.y };
		lines[ 2 ][ 1 ] = { boxVectors3.x, boxVectors3.y };
		lines[ 3 ][ 0 ] = { boxVectors3.x, boxVectors3.y };
		lines[ 3 ][ 1 ] = { boxVectors0.x, boxVectors0.y };

		lines[ 4 ][ 0 ] = { boxVectors0.x, boxVectors0.y };
		lines[ 4 ][ 1 ] = { boxVectors6.x, boxVectors6.y };

		// top of box
		lines[ 5 ][ 0 ] = { boxVectors6.x, boxVectors6.y };
		lines[ 5 ][ 1 ] = { boxVectors5.x, boxVectors5.y };
		lines[ 6 ][ 0 ] = { boxVectors5.x, boxVectors5.y };
		lines[ 6 ][ 1 ] = { boxVectors4.x, boxVectors4.y };
		lines[ 7 ][ 0 ] = { boxVectors4.x, boxVectors4.y };
		lines[ 7 ][ 1 ] = { boxVectors7.x, boxVectors7.y };
		lines[ 8 ][ 0 ] = { boxVectors7.x, boxVectors7.y };
		lines[ 8 ][ 1 ] = { boxVectors6.x, boxVectors6.y };


		lines[ 9 ][ 0 ] = { boxVectors5.x, boxVectors5.y };
		lines[ 9 ][ 1 ] = { boxVectors1.x, boxVectors1.y };

		lines[ 10 ][ 0 ] = { boxVectors4.x, boxVectors4.y };
		lines[ 10 ][ 1 ] = { boxVectors2.x, boxVectors2.y };

		lines[ 11 ][ 0 ] = { boxVectors7.x, boxVectors7.y };
		lines[ 11 ][ 1 ] = { boxVectors3.x, boxVectors3.y };

		for( int i = 0; i < 12; i++ )
		{
			D::DrawLine( lines[ i ][ 0 ].x, lines[ i ][ 0 ].y, lines[ i ][ 1 ].x, lines[ i ][ 1 ].y, color );
		}
	}
}

void D::DrawCircle( float x, float y, float r, float s, Color color )
{
	float Step = M_PI * 2.0 / s;
	for( float a = 0; a < ( M_PI*2.0 ); a += Step )
	{
		float x1 = r * cos( a ) + x;
		float y1 = r * sin( a ) + y;
		float x2 = r * cos( a + Step ) + x;
		float y2 = r * sin( a + Step ) + y;

		DrawLine( x1, y1, x2, y2, color );
	}
}

void D::ShowRadar(float X, float Y, float Size)
{
	/*DrawOutlinedRect(X - Size / 2 - 1, Y - 1, Size + 2, Size + 2, Color(23, 23, 23, 255));
	DrawOutlinedRect(X - Size / 2, Y, Size, Size, Color(90, 90, 90, 255));

	DrawLine(X - Size / 2, Y + Size / 2, X + Size / 2, Y + Size / 2, Color(90, 90, 90, 255));
	DrawLine(X, Y, X, Y + Size, Color(90, 90, 90, 255));*/
	int centerx = X + (Size / 2);
	int centery = Y + (Size / 2);

	auto mColor = Vars.g_fBColor;
	auto tColor = Vars.g_fTColor;

	DrawRect(centerx, centery - Size, 1, 2 * Size, Color(mColor[0] * 255.0f, mColor[1] * 255.0f, mColor[2] * 255.0f, 255));
	DrawRect(centerx - Size, centery, 2 * Size, 1, Color(mColor[0] * 255.0f, mColor[1] * 255.0f, mColor[2] * 255.0f, 255));
	Border(centerx - Size - 6, centery - Size - 6, 2 * Size + 6, 2 * Size + 6, 6, Color(mColor[0] * 255.0f, mColor[1] * 255.0f, mColor[2] * 255.0f, 255)); //all border
	DrawRect(centerx - Size, centery - Size, 2 * Size, 2 * Size, Color(50, 50, 50, 220));
	DrawRect(centerx, centery - Size, 1, 2 * Size, Color(mColor[0] * 255.0f, mColor[1] * 255.0f, mColor[2] * 255.0f, 255));
	DrawRect(centerx - Size, centery, 2 * Size, 1, Color(mColor[0] * 255.0f, mColor[1] * 255.0f, mColor[2] * 255.0f, 255));
	DrawRect(centerx - Size - 6, centery - Size - 22, 2 * Size - (Size / 235) + 12, 25, Color(mColor[0] * 255.0f, mColor[1] * 255.0f, mColor[2] * 255.0f, 255)); //upper line
	DrawString(F::ESP,centerx - Size + 15, centery - Size - 7, Color(tColor[0] * 255.0f, tColor[1] * 255.0f, tColor[2] * 255.0f, 255), FONT_CENTER, "Radar");
	Border(centerx - Size - 6, centery - Size - 22, 2 * Size + 11, 2 * Size + 28, 1, Color(mColor[0] * 255.0f, mColor[1] * 255.0f, mColor[2] * 255.0f, 255));
	Rectangle(X + (Size / 2) - 2, Y + (Size / 2) - 2, 5, 5, 1, Color(mColor[0] * 255.0f, mColor[1] * 255.0f, mColor[2] * 255.0f, 255));
	DrawRect(X + (Size / 2) - 1, Y + (Size / 2) - 1, 3, 3, Color(255, 255, 255, 255));
}

void D::DrawRadarEntity(float X, float Y, float RadarSize, Color _Color, Vector RadarX, Vector RadarY, QAngle Angle, int HP, bool IsFriend)
{

	float dx = RadarX.x - RadarY.x;
	float dy = RadarX.y - RadarY.y;
	float flAngle = Angle.y + 180;
	float yaw = flAngle * (M_PI / 180.0);
	float mainViewAngles_CosYaw = cos(yaw);
	float mainViewAngles_SinYaw = sin(yaw);
	float x = dy*(-mainViewAngles_CosYaw) + dx*mainViewAngles_SinYaw;
	float y = dx*(-mainViewAngles_CosYaw) - dy*mainViewAngles_SinYaw;
	float range = RadarSize * Vars.Visuals.Radar.range;
	if (fabs(x)>range || fabs(y)>range)
	{
		if (y>x)
		{
			if (y>-x) {
				x = range*x / y;
				y = range;
			}
			else {
				y = -range*y / x;
				x = -range;
			}
		}
		else {
			if (y>-x) {
				y = range*y / x;
				x = range;
			}
			else {
				x = -range*x / y;
				y = -range;
			}
		}
	}
	int ScreenX = X + (RadarSize / 2) + int(x / range*float(RadarSize));
	int ScreenY = Y + (RadarSize / 2) + int(y / range*float(RadarSize));
	DrawRect(ScreenX - 2, ScreenY - 2, 4, 4, Color(200, 130, 0, 255));

		DrawOutlinedRect(ScreenX - 4, ScreenY + 4, HP * 8 / 100, 2, _Color);
	}