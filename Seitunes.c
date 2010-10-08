/* ---------------------------------------------------------------------
 *
 *  Seitunes is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Seitunes is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Seitunes.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------
 *
 *  Seitunes.c
 *  
 *  A command-line utility to control iTunes.
 *
 *  Copyright 2010 Florent Delannoy.
 *
 */

#include "Seitunes.h"


int main( int argc, char** argv )
{
	// Curses initialization
	initscr();
	nocbreak();
	noecho();
	nonl();
	intrflush(stdscr, TRUE);
	keypad(stdscr, TRUE);
	halfdelay(10);
	
	// Variables initialization
	int end = 0;
	int input = 0;
	int refresh = 1;
	int printDocumentation = 0;
	int printPlaylists = 0;
	int volume = 0;
	int state = 0;
	int shuffle = 0;
	int rating = 0;
	int decay = 0;
	int i = 0, j = 0;
	int gotPlaylists = 0;
	int isInputing = 0;
	FILE* p;
	char artist[COLS];
	char song[COLS];
	char playlist[COLS];
	char newPlaylistName[COLS];
	char* playlists = NULL;
	
	newPlaylistName[0] = '\0';
	
	/* Main loop : printing data and waiting for user input */
	while( !end ){
		
		/* Gathering data on current state, current artist and current track */
		
		state = getiTunesState();
		if ( state == sei_ERROR ) {
			printw("Error: can't retrieve iTunes state!");
		}
		if( (state != sei_NOT_RUNNING) && (refresh == 1) ){
			if( !gotPlaylists ){
				playlists = getPlaylistsNames();
				gotPlaylists = 1;
			}
			volume = getVolume();
			getCurrentPlaylist( playlist );
			if( state != sei_STOPPED_ON_NOTHING ){
				shuffle = getShuffle();
				getSongName( song );
				getArtistName( artist );
				rating = getRating( );
			}
			decay = 0;
		}
		
		clearok(stdscr, true);
		erase();

		printw("Seitunes, a command-line interface for iTunes.\n");
		printw("Press 'h' for a list of available commands.\n");
		
		// Print data
		if( state == sei_NOT_RUNNING )
			printw("\niTunes is not running. Press 's' to Start iTunes.");
		else if( state == sei_STOPPED_ON_NOTHING )
			printw("\niTunes is stopped. Press 'l' to play a song from your Music library.");
		else {
			if( state == sei_PAUSED )
				printw("\n%s - %s (paused, vol %d)", artist, song, volume);
			if( state == sei_STOPPED_ON_SONG )
				printw("\n%s - %s (stopped, vol %d)", artist, song, volume);
			if( state == sei_PLAYING )
				printw("\n%s - %s (playing, vol %d)", artist, song, volume);

			printw("\nRating: ");
			for( i=0 ; i<rating ; i++) printw("*");
			for( ; i<5 ; i++) printw(".");
			
			printw("\nPlaylist: %s, ", playlist );
			if( shuffle ) printw("shuffle" );
			else printw("no shuffle" );
			
			if ( isInputing )
				printw("\n\nEnter (part of) the name of the playlist:\n%s", newPlaylistName);
			
			if ( printPlaylists )
				printw("\n\nAvailable playlists: %s", playlists);
		}
		
		if( printDocumentation )
			printw("\n\n   Spacebar  :\tPlay/pause\n     Arrows  :\tchange song, change volume\n     0 to 5  :\tSet rating\n        +/-  :\tVolume\n          h  :\tShow help\n          r  :\tRandom (toggle shuffle)\n          p  :\tShow all playlists\n          /  :\tEnter-your-playlist prompt\n          z  :\tQuit iTunes\n          q  :\tQuit Seitunes\n");
		
		refresh();
		refresh = 0;

		
		/* Wait for user input. */
		
		// Actually we're going to wait for a second then proceed to loop - halfdelay()
		// value is in tenth of a second - if there's no input
		input = getch();
		
		if( isInputing && input != ERR && input != 27 && j < COLS-1 ){
			if( input != '/' && input != 13 ){ // 13 being the enter key
				newPlaylistName[j++] = input;
				newPlaylistName[j] = '\0';
				continue;
			}
		}
		
		switch ( input ) {
			
			case 13 :
			case '/':
				if( !isInputing )
					isInputing = 1;
				else {
					playInPlaylist( newPlaylistName );
					isInputing = 0;
					j = 0;
					refresh = 1;
				}
				break;
			
			/* Launch iTunes */
			case 'S':
			case 's':
				startiTunes();
				refresh = 1;
				break;
			
			/* Start playing a song */
			case 'L':
			case 'l':
				playInPlaylist("Music");
				refresh = 1;
				break;
				
			/* Previous Song */
			case KEY_LEFT:
				previousSong();
				refresh = 1;
				break;
				
			/* Next Song */
			case KEY_RIGHT:
				nextSong();
				refresh = 1;
				break;
				
			/* Play / Pause */
			case ' ':  // Space key
				playpause();
				break;

			/* Print Playlists */
			case 'P':
			case 'p':
				printPlaylists = !printPlaylists;
				break;
			
			/* Shuffle */
			case 'R':
			case 'r':
				if( shuffle )
					setShuffle(0);
				else
					setShuffle(1);
				shuffle = !shuffle;
				break;
				
			/* Help */
			case 'h':  // 'h' key
			case 'H':  // 'H' key
				printDocumentation = !printDocumentation;
				break;
				
			/* Quit iTunes */
			case 'Z':
			case 'z':
				quitiTunes();
				sleep( 3 ); // iTunes takes time to quit.
				// If AppleScript queries are sent before iTune quits, it generates weird AppleScript errors
				break;
				
			/* Ratings */
			case '0':
				setRating(0); rating = 0;
				break;
			case '1':
				setRating(1); rating = 1;
				break;
			case '2':
				setRating(2); rating = 2;
				break;
			case '3':
				setRating(3); rating = 3;
				break;
			case '4':
				setRating(4); rating = 4;
				break;
			case '5':
				setRating(5); rating = 5;
				break;
				
				
			/* Volume up */
			case KEY_UP:
			case '+':
				volume = changeVolume(volume, +10);
				break;
			
			/* Volume down */
			case KEY_DOWN:
			case '-':
				volume = changeVolume(volume, -10);
				break;
				
			case 27:  // Escape key : can be an arrow or a real escape
				if( getch() == 79 ) // Second key of the arrow sequence
					switch ( getch() ) {
						case 65: // 'A' : is for Arrow Up
							volume = changeVolume(volume, +5);
							break;
						case 66: // 'B' : is for Arrow Down
							volume = changeVolume(volume, -5);
							break;
						case 67: // 'C' : is for Arrow Right
							nextSong();
							refresh = 1;
							break;
						case 68: // 'D' : is for Arrow Left
							previousSong();
							refresh = 1;
							break;
					}
				else {
					if ( isInputing ) {
						isInputing = 0;
						j = 0;
						continue;
					}
					endwin();
					return 0;
				}
				break;
			
			/* Exit Seitunes */
			case 'Q':
			case 'q':
				end = 1;
				break;
			
			/* Nothing entered */
			case ERR:
				if( (state == sei_PAUSED) || (state == sei_STOPPED_ON_SONG) || (state == sei_STOPPED_ON_NOTHING) ){
					refresh = 0; // If the player is paused or stopped, we don't need to get information at all
					usleep(50000); // Let's get a bit of rest while we're at it.
				}
				else{
					refresh = 0;
					// Problem is: when we don't refresh, we're at risk to have outdated information.
					// The decay value is here to counter that: after 3 seconds (halfdelay is 1 second)
					// without new input (getch() ERR case), we refresh the values anyway.
					decay += 1;
					if( decay > 3 ){
						refresh = 1;
						decay = 0;
					}
				}					
				break;
			
			default:
				printw("\n%d key pressed is not recognized.", input);
				break;
		}
	}
	
	endwin();
	
	if( gotPlaylists ) free( playlists );
	
	return 0;
}

