/////////////////////////////////////////////////////////
//
//	BCPlayer class - BeepComp player engine module
//
/////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include "BC/BCPlayer.h"
#include "BC/SFX.h"

using namespace std;

// constructor - will initialize at once
// to make it ready to rock immediately!
BCPlayer::BCPlayer()
{
	initialize();
}

// initializes the BCPlayer
bool BCPlayer::initialize()
{
	bool result = true;
	
	// initialize MPlayer
	mplayer.initialize();
	
	// initialize our MML - this will be our MML music source parser
	mml.initialize(44100, 120.0); // use default sample rate and tempo

	// set to default source with empty data
	string defaultSource = " ";
	mml.setSource(defaultSource);
	mml.parse(&mplayer);
	mplayer.goToBeginning();	

	mplayer.pause();
	mplayer.resetForNewSong();
	
	// bind SFX object to mplayer
	mplayer.bindSFX(&sfx);
	
	return result;
}

// close BCPlayer - call this method before you end app
void BCPlayer::terminate()
{
	mplayer.close();
}

// re-initialize audio device
// if you don't get any audio anymore, call this method
void BCPlayer::resetAudioDevice()
{
	mplayer.pause();
	mplayer.close();
	mplayer.initialize();
}

// load a BeepComp source file and parse
// gets BCPlayer ready to play the song immediately
bool BCPlayer::loadMusic(const std::string &fileName)
{
	bool result = true;
	mplayer.pause();
	mplayer.resetForNewSong();
	string response = mml.loadFile(fileName, &mplayer); // must pass a c++ string
	
	// see if loading resulted in error
	if(response=="Error")
		result = false;
	else
	{
		mml.parse(&mplayer);
		mplayer.goToBeginning();
	}
	
	return result;
}

// load a BeepComp source file and return the source from that file as std::string
// you can use this std::string to play without loading a file
// - by using loadString(std::string) function
std::string BCPlayer::loadFileToString(const std::string &fileName)
{
	mplayer.pause();
	mplayer.resetForNewSong();
	std::string result = mml.loadFile(fileName, &mplayer); // must pass a c++ string
	return result;
}

// takes a std::string and set up the player to play that string
// after loading you can start() to play
void BCPlayer::loadString(const std::string &source)
{
	mplayer.pause();
	mplayer.cleanUpForNewFile();
	mplayer.resetForNewSong();
	mml.setSource(source);
	mml.parse(&mplayer);
	mplayer.goToBeginning();
}

// starts playing the loaded song from the top
void BCPlayer::startMusic()
{
	mplayer.goToBeginning();
	mplayer.start();
}

// pauses the song
void BCPlayer::pauseMusic()
{
	mplayer.pause();
}

// restarts the song from paused location
void BCPlayer::restartMusic()
{
	mplayer.restart();
}

// set the music to track looping
void BCPlayer::enableLooping()
{
	mplayer.enableLooping();
}

// disable track looping
void BCPlayer::disableLooping()
{
	mplayer.disableLooping();
}

// check if the song has officially finished or not
bool BCPlayer::musicFinished()
{
	if( mplayer.reachedSongLastFramePure() || mplayer.isSongFinished() )
		return true;
	else
		return false;
}

// takes a percent value in float (0.0 to 100.0)
// sets the player volume
void BCPlayer::setMusicVolume(float percent)
{
	float newGain = static_cast<float>(percent) / 100.0f;
	if(newGain > 1.0f) newGain = 1.0;
	else if(newGain < 0.0f) newGain = 0.0;
	mplayer.setMasterGain(newGain);
}

// returns the current player volume
// as a percent value in float (0.0 to 100.0)
float BCPlayer::getMusicVolume()
{
	// get master gain - float 0.0 to 1.0
	return static_cast<int>(mplayer.getMasterGain() * 100.0f);
}

// takes a float value (0 to 100) as a percent value
// sets up the position in the song for the player to start playing 
void BCPlayer::seek(float percent)
{
	if(percent < 0.0) percent = 0.0;
	else if(percent > 100.0) percent = 100.0;
	float ratio = percent / 100.0;
	long songLength = mplayer.getSongLastFramePure();
	long seekTo = static_cast<long>(songLength * ratio);
	mplayer.seek(seekTo);
}


//
//
//   SFX related functions...
//
//

// load up a sound effect WAV file - must be 16bit - 44100hz
// can be mono or stereo. but length must be within 5 seconds
std::string BCPlayer::loadSFX(int slot, std::string filename)
{
	string strToReturn = "OK";
	bool result = sfx.loadSound(slot, filename);
	if(!result)
		strToReturn = sfx.getErrorText(slot);
	return strToReturn;
}

// set the volume of a specific SFX slot (0 to 100)
void BCPlayer::setSFXVolume(int slot, int volumePercent)
{
	float g = static_cast<float>(volumePercent) / 100.0f;
	sfx.setGain(slot, g);
}

// returns the current volume (0 - 100) of a specific SFX slot
int BCPlayer::getSFXVolume(int slot)
{
	return static_cast<int>(sfx.getGain(slot) * 100.0f);
}

// set the stereo panning factor to a specific SFX slot
// 0 << left-most ... right-most >> 100 
void BCPlayer::setSFXPanning(int slot, int panningPercent)
{
	float p = static_cast<float>(panningPercent) / 100.0f;
	sfx.setPanning(slot, p);
}

// get the current stereo panning factor of a specific SFX slot
// 0 << left-most ... right-most >> 100 
int BCPlayer::getSFXPanning(int slot)
{
	return static_cast<int>(sfx.getPanning(slot) * 100.0f);
}

// play the specified SFX slot from beginning
void BCPlayer::startSFX(int slot)
{ sfx.start(slot); }

// stop the specified SFX slot - will not remember where you cut it off
void BCPlayer::stopSFX(int slot)
{ sfx.stop(slot); }

// pause the specified SFX slot - you can resume from that point later
void BCPlayer::pauseSFX(int slot)
{ sfx.pause(slot); }

// resume the specified SFX slot from where you paused
void BCPlayer::resumeSFX(int slot)
{ sfx.resume(slot); }


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//
//		Note to myself...
//		In the section below, the contents of following cpp files should go 
//
//
//		Sound, SFX
//		MData, DData, Astro, LFO, OSC, NOSC, DelayLine MML MPlayer
//
//				... all .cpp files for above -> assimilate to this BCPlayer.cpp
//				... all .h files for above -> put in include/BC folder
//
//		Then make following changes:
//
//		1) include directives - rename all the header files to "BC/.."
//
//		2) comment out mp3 and libsndfile header inclusion in MPlayer
//
//		3) comment out exportToFile function in MPlayer class
//
//		4) make sure all <windows.h> includes are eliminated
//
//		5) make sure all windows.h related commands are erased
//
//		6) MPlayer class needs to know about SFX class #include "SFX.h"
//
//		7) Add bindSFX class + SFX class property to "MPlayer.h" (at bottom)
//
//		8) In MPlayer's getMix method - add snipet ... sfx.getOutput(channel);
//
//		9) MPlayer.h file - #include <portaudio> to #include "BC/portaudio"


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
