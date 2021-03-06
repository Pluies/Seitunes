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


void printw_color(char* str, int color){
	attron(COLOR_PAIR(color));
	printw(str);
	attroff(COLOR_PAIR(color));
}

int main( int argc, char** argv )
{
	// Curses initialization
	initscr();
	nocbreak();
	noecho();
	nonl();
	start_color();
	intrflush(stdscr, TRUE);
	keypad(stdscr, TRUE);
	halfdelay(10);
	// Curses colors
	use_default_colors();
	init_color(COLOR_CYAN, 750, 700,750);
	init_pair(1, COLOR_RED, -1);
	init_pair(2, COLOR_YELLOW, -1);
	init_pair(3, COLOR_GREEN, -1);
	init_pair(4, COLOR_MAGENTA, -1);
	init_pair(5, COLOR_BLUE, -1);
	init_pair(6, COLOR_CYAN, -1);
	
	// Variables initialization
	int end = 0;
	int input = 0;
	int refresh = 1;
	int printDocumentation = 0;
	int volume = 0;
	int state = getiTunesState();
	int shuffle = 0;
	int rating = 0;
	time_t lastRefresh = 0, now;
	char artist[COLS];
	char song[COLS];
	char album[COLS];
	
	/* Main loop : printing data and waiting for user input */
	while( !end ){

		/* Gathering data on current state, current artist and current track */

		now = time(NULL);
		if( refresh || difftime(now, lastRefresh) > 4 ){
			// Refreshes if forced, or at least every 4 seconds
			state = getiTunesState();
			if ( state == sei_ERROR ) {
				printw("Error: can't retrieve iTunes state!");
			}
			if( state != sei_NOT_RUNNING ){
				volume = getVolume();
				if( state != sei_STOPPED_ON_NOTHING ){
					shuffle = getShuffle();
					getSongName( song );
					getArtistName( artist );
					if(strcmp("", artist) == 0)
						strcpy(artist, "Unknown artist");
					getAlbumName( album );
					if(strcmp("", album) == 0)
						strcpy(album, "Unknown album");
				}
			}
			refresh = 0;
			lastRefresh = time(NULL);
		}

		clearok(stdscr, true);
		erase();

		printw("Seitunes, a command-line interface for iTunes.\n");
		printw("Press 'h' for a list of available commands.\n");
		
		// Print data
		if( state == sei_NOT_RUNNING )
			printw_color("\niTunes is not running. Press 's' to Start iTunes.", 1);
		else if( state == sei_STOPPED_ON_NOTHING )
			printw_color("\niTunes is stopped. Press 'l' to play a song from your Music library.", 2);
		else {
			printw("\n%s\n", song);
			if( strcmp(artist, "Unknown artist") == 0)
				printw_color(artist, 2);
			else
				printw("%s", artist);
			printw(" - ");
			if( strcmp(album, "Unknown album") == 0 )
				printw_color(album, 2);
			else
				printw("%s", album);

			printw("\n\n");
			if( state == sei_PAUSED )
				printw_color("Paused", 2);
			if( state == sei_STOPPED_ON_SONG )
				printw_color("Stopped", 1);
			if( state == sei_PLAYING )
				printw_color("Playing", 3);

			printw(", vol ");
			printw("%d | ", volume);

			if( shuffle ) printw("Shuffle" );
			else printw("No shuffle" );
		}
		
		if( printDocumentation )
			printw("\n\n   Spacebar  :\tPlay/pause\n     Arrows  :\tchange song and change iTunes volume\n     0 to 5  :\tSet rating\n        +/-  :\tSystem Volume\n          h  :\tShow help\n          r  :\tRandom (toggle shuffle)\n          z  :\tQuit iTunes\n          q  :\tQuit Seitunes\n");
		
		refresh();

		refresh = 0;

		
		/* Wait for user input. */
		
		// Actually we're going to wait for a second then proceed to loop anyway (halfdelay()
		// value is in tenth of a second) if there's no input
		input = getch();
		
		switch ( input ) {
			
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
				if( state == sei_PAUSED) state = sei_PLAYING;
				else if( state == sei_PLAYING) state = sei_PAUSED;
				break;

			/* Shuffle (R is for random) */
			case 'R':
			case 'r':
				if( shuffle )
					setShuffle(0);
				else
					setShuffle(1);
				shuffle = !shuffle;
				break;
				
			/* Help */
			case 'h':
			case 'H':
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
				
			
			/* System volume up */				
			case '+':
				changeSystemVolume(+5);
				break;
			
			/* System volume down*/
			case '-':
				changeSystemVolume(-5);
				break;

			/* iTunes volume up */
			case KEY_UP:
				volume = changeVolume(volume, +10);
				break;
			
			/* iTunes volume down */
			case KEY_DOWN:
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
				break;
			
			default:
				printw("\n%d key pressed is not recognized.", input);
				break;
		}
	}
	
	endwin();
	
	return 0;
}

