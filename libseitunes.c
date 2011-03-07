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
 *  libseitunes.c
 *  
 *  A C API to control iTunes through AppleScript calls.
 *
 *  Copyright 2010 Florent Delannoy.
 *
 */

#include "libseitunes.h"


/* iTunes via Applescript wrapper functions */


/* int getiTunesState( )
 * Returns an integer that shows the state iTunes is in.
 * States are coded as integers, defined in Seitunes.h
 */
int getiTunesState()
{
	char result[COLS];
	// We'll first check if iTunes is running or not
	executeApplescriptToGetString("System Events", "(name of processes) contains \"iTunes\"", result); // Launch the Applescript and get its standard output into result
	if( strncmp( "tru", result, 3 ) != 0 )
		return sei_NOT_RUNNING;
	// If we're here, then iTunes is running. We'll check the player state (stopped, playing or paused).
	executeApplescriptToGetString( "iTunes", "player state as string", result );
	if( strncmp( "play", result, 4 ) == 0 )
		return sei_PLAYING;
	else{
		if( strncmp( "stop", result, 4 ) == 0 ){
			if( executeApplescript( "iTunes", "name of current track as string" ) == 0 )
				return sei_STOPPED_ON_SONG;
			else
				return sei_STOPPED_ON_NOTHING;
		}
		else
			return sei_PAUSED;
	}
	
	// If we're here, we have an indetermined state - iTunes running but neither playing, paused or stopped? Shouldn't happen.
	return sei_ERROR;
}

/* int getSongName( char* name)
 * Puts the song name into the char* passed as argument
 * and returns that string's length.
 */
int getSongName( char* name )
{
	executeApplescriptToGetString("iTunes", "name of current track as string", name);
	return strlen(name);
}

/* int getAlbumName( char* name)
 * Puts the Album name into the char* passed as argument
 * and returns that string's length.
 */
int getAlbumName( char* name )
{
	executeApplescriptToGetString("iTunes", "album of current track as string", name);
	return strlen(name);
}

/* char* getPlaylistsNames( )
 * Puts all the playlists into a dynamically allocated string returned.
 */
char* getPlaylistsNames()
{
	FILE* p;
	char* playlists = malloc( 1024*sizeof(char) ); // We'll assume less than 1024 char for all playlists
	p = popen( "osascript -e 'tell application \"iTunes\" to name of playlists'", "r");
	fgets( playlists, 1024, p );
	pclose(p);
	return playlists;
}

/* int getArtistName( char* name)
 * Puts the artist name into the char* passed as argument
 * and returns that string's length.
 */
int getArtistName( char* name )
{
	executeApplescriptToGetString("iTunes", "artist of current track as string", name);
	return strlen(name);
}

/* int getVolume()
 * Returns the current volums as an int between 0 and 100
 */
int getVolume()
{
	int volume = 0;
	executeApplescriptToGetInt("iTunes", "sound volume as integer", &volume);
	return volume;
}

/* int getShuffle()
 * Returns 1 if shuffle is active, 0 if it isn't.
 */
int getShuffle()
{
	int shuffle = 0;
	char result[COLS];
	executeApplescriptToGetString("iTunes", "shuffle of current playlist as string", result);
	if( strncmp( "true", result, 4 ) == 0 )
		shuffle = 1;
	return shuffle;
}

/* int getCurrentPlaylist( char* name )
 * Puts the current playlist name into the char* passed as argument
 * and returns that string's length.
 */
int getCurrentPlaylist( char* name )
{
	executeApplescriptToGetString("iTunes", "name of current playlist as string", name);
	return strlen(name);
}

/* int changeVolume()
 * Set the volume to current volume + variation.
 * Variation can be either positive or negative.
 * returns the new volume between 0 and 100.
 */
int changeVolume( int currentVolume, int variation )
{
	char command[COLS];
	int newVolume = currentVolume + variation;
	if( newVolume > 100 )
		newVolume = 100;
	if( newVolume < 0 )
		newVolume = 0;
	sprintf(command, "set sound volume to %3d", newVolume);
	executeApplescriptAsync( "iTunes", command );
	return newVolume;
}

/* int getRating( )
 * Get the rating of the current song in stars (zero to 5).
 */
int getRating()
{
	int rating = 0;
	executeApplescriptToGetInt("iTunes", "rating of current track", &rating);
	return (rating/20);
}

/* setRating( int rating )
 * Set the rating of the current song to the rating
 * integer passed in parameter (zero to 5 stars).
 */
void setRating( int rating )
{
	char command[COLS];
	if ( rating < 0 ) rating = -rating;
	rating = (rating % 6); // We make sure the input is correctly between 0 and 5
	rating = 20 * rating; // Then we multiply by 20: actual internal ratings in iTunes range from 0 to 100
	sprintf(command, "set rating of current track to %3d", rating);
	executeApplescriptAsync( "iTunes", command );
}

/* playInPlaylist( const char* newPlaylist )
 * Tries to find a playlist containing the string newPlaylist.
 * If found, starts reading from here.
 * If not found or playlist empty, returns with no side effect.
 */
void playInPlaylist( const char* newPlaylist )
{
	char command[512];
	sprintf(command, "if thisPlaylist contains \"%s\" and duration of playlist thisPlaylist is not 0 then play playlist thisPlaylist", newPlaylist);
	if( ! fork() ) execlp("osascript", "osascript",
						  "-e", "tell application\"iTunes\" to set allPlaylists to name of playlists",
						  "-e", "tell application\"iTunes\" ",
						  "-e", "repeat with thisPlaylist in allPlaylists",
						  "-e", command,
						  "-e", "end repeat",
						  "-e", "end tell", NULL);
	wait(NULL);
}

/* playpause()
 * Plays the song if it is paused, pause the song if it is played.
 */
void playpause()
{
	executeApplescriptAsync("iTunes", "playpause" );
}

/* nextSong()
 * Tells iTunes to play the next song
 */
void nextSong()
{
	executeApplescriptAsync( "iTunes", "next track" );
}

/* changeSystemVolume(int n)
 * Modifies the system volume by n%
 */
void changeSystemVolume(int modif)
{
	char command[512];
	if (modif>=0)
		sprintf(command, "set volume output volume (output volume of (get volume settings)) + %d", modif);
	else{
		modif = -modif;
		sprintf(command, "set volume output volume (output volume of (get volume settings)) - %d", modif);
	}
	executeRawApplescriptAsync(command);
}

/* previousSong()
 * Tells iTunes to play the previous song.
 * More precisely, this has the same effect as the "back" button in iTunes:
 * it goes back to the beginning of the song if you're advanced into the song,
 * or to the previous one if you're at the beginning (first x seconds ; 3 maybe ?).
 */
void previousSong()
{
	executeApplescriptAsync( "iTunes", "back track" );
}

/* setShuffle(int boolShuffle)
 * Sets shuffle of the current playlist to the value of boolShuffle according to C
 * standard interpretation (0 = false ; anything else = true)
 */
void setShuffle(int boolShuffle)
{
	if( boolShuffle )
		executeApplescriptAsync( "iTunes", "set shuffle of current playlist to true" );	
	else
		executeApplescriptAsync( "iTunes", "set shuffle of current playlist to false" );	
}

/* startiTunes()
 * Starts, well, iTunes.
 */
void startiTunes()
{
	executeApplescript( "iTunes", "activate" );	
}

/* quitiTunes()
 * It does indeed quit iTunes.
 */
void quitiTunes()
{
	executeApplescript("iTunes", "quit" );
}




/* Helper functions */

/* int executeApplescriptToGetString( const char* application, const char* command, char* result )
 * Tells application "application" to "command", and stores the result in result as a string.
 * No more than COLS characters will be written.
 * The returned value is the result length.
 */
int executeApplescriptToGetString( const char* application, const char* command, char* result )
{
	FILE* p;
	int resultSize = 0;
	char toExec[1024];
	sprintf(toExec, "osascript -e 'tell application \"%s\" to %s'", application, command);
	p = popen( toExec, "r");
	if( fgets( result, COLS, p ) != NULL ){
		resultSize = strlen(result)-1;
		result[ resultSize ] = '\0';
	}
	pclose(p);
	return resultSize;
}

/* int executeApplescriptToGetInt( const char* application, const char* command, char* result )
 * Tells application "application" to "command", and stores the result in result as an int.
 * The returned value is the result length.
 */
int executeApplescriptToGetInt( const char* application, const char* command, int* result )
{
	char resultString[COLS];
	int returnValue = executeApplescriptToGetString(application, command, resultString);
	*result = atoi( resultString );
	return returnValue;
}

/* int executeApplescript( const char* application, const char* command )
 * Tells application "application" to "command", without considering the stdout.
 * The returned value is the return value of the call (0 if everything's alright).
 */
int executeApplescript( const char* application, const char* command )
{
	char toExec[1024];
	sprintf(toExec, "tell application \"%s\" to %s", application, command);
	return executeRawApplescript(toExec);
}

/* int executeApplescriptAsync( const char* application, const char* command )
 * Asynchronously tells application "application" to "command", without considering the stdout.
 */
void executeApplescriptAsync( const char* application, const char* command )
{
	char toExec[1024];
	sprintf(toExec, "tell application \"%s\" to %s", application, command);
	executeRawApplescriptAsync(toExec);
}

/* int executeRawApplescript( const char* command )
 * Execute the raw Applescript command provided.
 * The returned value is the return value of the call (0 if everything's alright).
 */
int executeRawApplescript( const char* command )
{
	int childExitStatus;
	if( ! fork() )
		execlp("osascript", "osascript", "-e", command, NULL);
	wait( &childExitStatus );
	return childExitStatus;
}

/* int executeRawApplescriptAsync( const char* command )
 * Execute the provided Applescript command without waiting
 * for a response.
 */
void executeRawApplescriptAsync( const char* command )
{
	if( ! fork() )
		execlp("osascript", "osascript", "-e", command, NULL);
}
