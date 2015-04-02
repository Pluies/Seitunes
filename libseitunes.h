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
 *  libseitunes.h
 *  
 *  A C API to control iTunes through AppleScript calls.
 *
 *  Copyright 2010 Florent Delannoy.
 *
 */

#ifndef __libseitunes_h__
#define __libseitunes_h__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

// iTunes states
#define sei_NOT_RUNNING 0
#define sei_STOPPED 1 // Deprecated - stopped has two meanings. Cf states 4 and 5
#define sei_PLAYING 2
#define sei_PAUSED 3
#define sei_STOPPED_ON_SONG 4
#define sei_STOPPED_ON_NOTHING 5
#define sei_ERROR -1

// Functions declaration
int getiTunesState();
char* getPlaylistsNames();
int getSongName( char* name );
int getArtistName( char* name );
int getAlbumName( char* name );
int getVolume();
int getShuffle();
int getCurrentPlaylist( char* name );
int changeVolume( int currentVolume, int variation );
int getRating();
void setRating( int rating );
void playInPlaylist( const char* newPlaylist );
void playpause();
void previousSong();
void nextSong();
void changeSystemVolume(int n);
void setShuffle(int boolShuffle);
void startiTunes();
void quitiTunes();

int executeRawApplescript( const char* command );
void executeRawApplescriptAsync( const char* command );
int executeApplescript( const char* application, const char* command );
void executeApplescriptAsync( const char* application, const char* command );
int executeApplescriptToGetString( const char* application, const char* command, char* result );
int executeApplescriptFileToGetString( const char* command, char* result );
int executeApplescriptToGetInt( const char* application, const char* command, int* result );

#endif
