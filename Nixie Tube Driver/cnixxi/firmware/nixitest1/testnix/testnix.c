#include <stdio.h>

#define CNFG_IMPLEMENTATION
#include "rawdraw_sf.h"

#include "../../ch32v003fun/minichlink/minichlink.h"
struct MiniChlinkFunctions * MCFO;

char serial_port[128] = "COM4";
char specific_programmer[128] = "linke";
init_hints_t hints = { .serial_port = serial_port, .specific_programmer = specific_programmer};

//#define ENABLE_TUNING

int targetnum = 0;
int debugregs = 0;
int lastsettarget = -1;
#define VOLTAGE_SCALE 2.01

const char * targdisp[] = { "D", "F", " ", "0", "9", "8", "7", "6", "5", "4", "3", "2", "1", ".", "N" };
void HandleKey( int keycode, int bDown )
{
	if( bDown )
	{
	switch( keycode )
	{
		case '~': case '`': case 192: targetnum = 0; break;
		case '1': targetnum = 10; break;
		case '2': targetnum = 9; break;
		case '3': targetnum = 8; break;
		case '4': targetnum = 7; break;
		case '5': targetnum = 6; break;
		case '6': targetnum = 5; break;
		case '7': targetnum = 4; break;
		case '8': targetnum = 3; break;
		case '9': targetnum = 2; break;
		case '0': targetnum = 1; break;
		case '-': case '_': case 189: targetnum = 11; break;
		case '=': case '+': targetnum = 12; break;
		case 'f': case 'F': targetnum = -1; break;
		case 'd': case 'D': targetnum = -2; break;
		case 'R': case 'r': debugregs = !debugregs; break;
	}
	}
}

int do_set = 0;
int sety = 0, setx = 0;
void HandleButton( int x, int y, int button, int bDown ) { if( bDown ) setx = x;  }
void HandleMotion( int x, int y, int mask ) { sety = y; do_set = mask; }
void HandleDestroy() { }

#define VOLTHISTSIZE 2048
float volthist[VOLTHISTSIZE];
float volthistvdd[VOLTHISTSIZE];
int volthisthead = 0;

int main()
{
	char cts[128];
	void * dev = MiniCHLinkInitAsDLL( &MCFO , &hints);
	if( !dev )
	{
		fprintf( stderr, "Error: Couldn't find programmer\n" );
		return -9;
	}

	MCFO->SetupInterface( dev );

	if( MCFO->HaltMode )   MCFO->HaltMode( dev, 0 );
	if( MCFO->Control5v )  MCFO->Control5v( dev, 1 );
	if( MCFO->Control3v3 ) MCFO->Control3v3( dev, 1 );
	if( MCFO->HaltMode )   MCFO->HaltMode( dev, 2 );

	MCFO->WriteReg32( dev, DMABSTRACTAUTO, 0 );

	printf( "DEV: %p\n", dev );
	CNFGSetup( "nixitest1 debug app", 640, 570 );
	while(CNFGHandleInput())
	{
		const uint32_t GLOW = 0xFFD010FF;
		const uint32_t GLOWDIM = 0x806008FF;
		const uint32_t BLUEGLOW = 0x2080d0ff;

		short w, h;
		int x, y;
		CNFGClearFrame();
		CNFGGetDimensions( &w, &h );

		static int set_period = 96;
		static int set_max_duty = 48;
		static int last_set_v = 0;
		static int aux_value = 0;
		{
			CNFGColor( 0x303030ff );
			CNFGTackSegment( w-100, 45, w-100, h );
			CNFGTackSegment( w-200, 45, w-200, h );
#ifdef ENABLE_TUNING
			CNFGTackSegment( w-300, 45, w-200, h );
			CNFGTackSegment( w-400, 45, w-300, h );
#endif
			CNFGColor( 0xD0D0D0FF );
			CNFGPenX = w-100+2; CNFGPenY = 47; sprintf( cts, "VTG %d", last_set_v ); CNFGDrawText( cts, 2 );
			CNFGPenX = w-200+2; CNFGPenY = 47; sprintf( cts, "AUX %d", aux_value ); CNFGDrawText( cts, 2 );
#ifdef ENABLE_TUNING
			CNFGPenX = w-300+2; CNFGPenY = 47; sprintf( cts, "Per %d", set_period ); CNFGDrawText( cts, 2 );
			CNFGPenX = w-400+2; CNFGPenY = 47; sprintf( cts, "Duty %d", set_max_duty ); CNFGDrawText( cts, 2 );
#endif
		}

		uint32_t rmask = 0;

		if( do_set )
		{
			do_set = 0;
			float set_v = (450 - sety)/2;
			if( setx > w - 100 )
			{
				if( set_v >= 0 && set_v < 220 )
				{
					last_set_v = (uint32_t)(set_v);
					rmask = ( last_set_v << 16 ) | 0x41;
				}
			}
			else if( setx > w - 200 )
			{
				if( set_v >= 0 )
				{
					aux_value = set_v;
					rmask = (aux_value<<16) | 0xaa45;
				}
			}
#ifdef ENABLE_TUNING
			else if( setx > w - 300 )
			{
				set_period = set_v;
				rmask = (set_period<<16) | (set_max_duty<<24) | 0xaa44;
			}
			else if( setx > w - 400 )
			{
				set_max_duty = set_v;
				rmask = (set_period<<16) | (set_max_duty<<24) | 0xaa44;
			}
#endif
		}
		else if( targetnum == -1 )
		{
			// Fade Demo
			static int fadeplace;
			fadeplace+=1;
			int fadegroup = (fadeplace)>>8;
			int timeinfade = fadeplace&0xff;
			int time0 = timeinfade;
			int time1 = 255;
			int disp0 = 10-((fadegroup+1)%11);
			int disp1 = 10-((fadegroup+0)%11);
			rmask = (time1<<24)|(time0<<16)|(disp1<<12)|(disp0<<8)|0x43;
			lastsettarget = targetnum;
		}
		else if( lastsettarget != targetnum )
		{
			if( targetnum == -2 )
			{
				static int fadeplace;
				fadeplace+=1;
				int fadegroup = (fadeplace)>>8;
				int timeinfade = fadeplace&0xff;
				int time0 = 60;
				int time1 = 120;
				int disp0 = 3;
				int disp1 = 4;
				rmask = (time1<<24)|(time0<<16)|(disp1<<12)|(disp0<<8)|0x43;
				lastsettarget = targetnum;
			}
			else if( targetnum == -1 )
			{
				// Do nothing
			}
			else
			{
				rmask = 0x00000042 | (targetnum<<16);
			}
			lastsettarget = targetnum;
		}
		else
		{
			rmask = 0x00000040;
			MCFO->WriteReg32( dev, DMDATA0, 0x00000040 );
		}

		MCFO->WriteReg32( dev, DMDATA0, rmask );

		uint32_t status = 0xffffffff;
		int r;
		
		if( debugregs )
		{
			#define MONREGS 20
			uint32_t monitor_regs[MONREGS] = {
				DMDATA0, DMDATA1, DMCONTROL, DMSTATUS, 
				DMHARTINFO, DMABSTRACTCS, DMCOMMAND, DMABSTRACTAUTO,
				DMPROGBUF0, DMPROGBUF1, DMPROGBUF2, DMPROGBUF3,
				DMPROGBUF4, DMPROGBUF5, DMPROGBUF6, DMPROGBUF7,
				DMCPBR, DMCFGR, DMSHDWCFGR, 0x06 };
			const char * monitor_names[MONREGS] = {
				"data0", "data1", "dmcontrol", "dmstatus",
				"hartinfo", "abstractcs", "command", "abstractauto",
				"progbuf0", "progbuf1", "progbuf2", "progbuf3", 
				"progbuf4", "progbuf5", "progbuf6", "progbuf7", 
				"cpbr", "cfgr", "shdwcfgr", "0x06" };
			static uint32_t regv[MONREGS];
			static uint32_t lastr[MONREGS];
			static int lastreg;
			lastreg++;
			if( lastreg >= MONREGS ) lastreg = 0;

			lastr[lastreg] = MCFO->ReadReg32( dev, monitor_regs[lastreg], &regv[lastreg] );
			int i;
			for( i = 0; i < MONREGS; i++ )
			{
				CNFGColor( 0xc0c0c0ff );
				CNFGPenX = 1 + (i%2) * 200;
				CNFGPenY = (460+(i/2)*10);
				sprintf( cts, "%12s: %08x%s", monitor_names[i], regv[i], lastr[i]?" ERROR":"" );
				CNFGDrawText( cts, 2 );
			}
		}
		else
		{
			CNFGColor( 0xc0c0c0ff );
			CNFGPenX = 1;
			CNFGPenY = 460;
			CNFGDrawText( "Press R to enable reg debug.", 2 );
		}

		int timeout;
		timeout = 0;
		const int maxtimeout = 30;
		retry:
		status = 0xffffffc0;
		r = MCFO->ReadReg32( dev, DMDATA0, &status );

		if( ( ( status & 0xc0 ) == 0x40 || status == 0 || status == 0xffffffff )  && timeout++ < maxtimeout ) goto retry;
		if( r && timeout++ < maxtimeout ) { printf( "R: %d\n", r ); status = 0; goto retry; }
		
		if( timeout >= maxtimeout )
		{
			CNFGColor( 0xc0c0c0ff );
			CNFGPenX = 200;
			CNFGPenY = 199;
			CNFGDrawText( "Timeout on command.", 5 );
		}
		
		if( status == 0xffffffff || status == 0x00000000 )
		{
			MCFO->SetupInterface( dev );
			if( MCFO->HaltMode )   MCFO->HaltMode( dev, 2 );
		}
		else
		{
			CNFGColor( 0xc0c0c0ff );
			CNFGPenX = 590;
			CNFGPenY = 1;
			sprintf( cts, "%08x", status );
			CNFGDrawText( cts, 2 );

			float voltvdd = 1.20/(((status>>22)&0x3ff)/1023.0f); // vref = 2.2v
			float voltage = ((((float)((status>>12)&0x3ff))/1023.0f)*101.0)*voltvdd; //101 because it's 10k + 1M
			// Measured @ 176 reported here, but 180 in reality if ref is 1.2.  But 1.21 fixes it.
			volthist[volthisthead] = voltage;
			volthistvdd[volthisthead] = voltvdd;
			volthisthead = (volthisthead + 1) % VOLTHISTSIZE;
			CNFGColor( (voltage > 198)?0xff0000ff:GLOW ); 
			CNFGPenX = 1;
			CNFGPenY = 1;
			sprintf( cts, "HV Line: %3.0f V\nRStatus: %d", voltage, r );
			CNFGDrawText( cts, 4 );	

			for( y = 0; y < 2; y++ ) for( x = 0; x < 2; x++ )
			{
				CNFGPenX = 200+x;
				CNFGPenY = 1+y;
				CNFGDrawText( targdisp[targetnum+2], 10 );
			}

			CNFGColor( BLUEGLOW );
			CNFGPenX = 300;
			CNFGPenY = 1;
			sprintf( cts, "VDD: %3.3f V\n", voltvdd );
			CNFGDrawText( cts, 4 );

			int i;

			CNFGColor( 0xff0000ff );
			CNFGTackSegment( 0, 450-200*2-6, w, 450-200*2-6 );
			CNFGPenX = w - 250; CNFGPenY = 450-200*2-10-6;
			CNFGDrawText( "WARNING: DO NOT EXCEED THIS LINE (200)", 2 );

			for( i = 0; i < 10; i++ )
			{
				CNFGColor( (i == 0 )?0xD0D0D0FF:0x303030ff );
				CNFGPenX = 1;
				CNFGPenY = 450 - 10 - i * 40;
				sprintf( cts, "%d volts", i * 20 );
				CNFGDrawText( cts, 2 );
				CNFGTackSegment( 0,450 - i * 40, w, 450 - i * 40 );
			}

			{
				CNFGColor( BLUEGLOW ); 
				int vhp = (volthisthead - 1 + VOLTHISTSIZE*100)%VOLTHISTSIZE;
				float vl = voltvdd*10;
				for( i = 0; i < w*2; i++ )
				{
					float v = volthistvdd[vhp]*10;
					CNFGTackSegment( i/2, 450 - vl*2, (i+1)/2, 450 - v*2 );
					vhp = (vhp - 1 + VOLTHISTSIZE*100)%VOLTHISTSIZE;
					//printf( "%f\n", v );
					vl = v;
				}
			}

			int vhp = (volthisthead - 1 + VOLTHISTSIZE*100)%VOLTHISTSIZE;
			float vl = voltage;
			CNFGColor( GLOW ); 

			for( i = 0; i < w*2; i++ )
			{
				float v = volthist[vhp];
				CNFGTackSegment( i/2, 450 - vl*2, (i+1)/2, 450 - v*2 );
				vhp = (vhp - 1 + VOLTHISTSIZE*100)%VOLTHISTSIZE;
				//printf( "%f\n", v );
				vl = v;
			}
		}

		CNFGSwapBuffers();
	}

	return 0;
}

