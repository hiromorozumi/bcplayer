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
//		7) Add bindSFX class + SFX class property to "BC/MPlayer.h" (at bottom)
//
//		8) In MPlayer's getMix method - add snipet ... sfx.getOutput(channel);
//
//		9) MPlayer.h file - #include <portaudio> to #include "BC/portaudio"


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////
// Sound class implementation //////////////////////////////

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "BC/sndfile.h"
#include "BC/Sound.h"

using namespace std;

// constructor - initializes all variables at once
Sound::Sound()
{
	stereo = false; // monoral by default
	error = "(no error yet)";
	leftData.push_back(0.0); // have a dummy value to start
	rightData.push_back(0.0);
	dataSize = min(leftData.size(), rightData.size());
	gain = 0.85f;
	leftGain = 0.85f;
	rightGain = 0.85f;
	panning = 0.5f;
	pos = 0;
	playing = false;
}

bool Sound::isStereo()
{ return stereo; }

std::string Sound::getErrorText()
	{ return error; }

// load sound data from a 16bit WAV file
// it can be mono or stereo... but must be within 5 seconds
bool Sound::loadFile(const std::string &filename)
{	
	// cout << "beginning of Sound class - loadFile " << filename << endl;

	// open sound file
	SF_INFO sndInfo;
	SNDFILE *sndFile = sf_open(filename.c_str(), SFM_READ, &sndInfo);
	if(sndFile==NULL)
	{
		error = "Error reading file: " + filename;
		return false;
	}
	
	// check format
	if( (sndInfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16) ) &&
		(sndInfo.format != (SF_FORMAT_OGG | SF_FORMAT_VORBIS) ) )
		{
			error = "Wrong format: " + filename;
			return false;
		}
	
	// cout << sndInfo.frames << endl;
	// cout << "format okay\n";
		
	// clear the sound data entirely
	leftData.clear();
	rightData.clear();
	
	bool readDone = false;
	int framesRead;
	int totalRead = 0;
	bool mono = false;
	
	// if this is a mono file...
	if(sndInfo.channels==1)
		mono = true;
	
	// case - we have a mono file
	if(mono==true)
	{
		while(!readDone)
		{
			framesRead = sf_readf_float(sndFile, readBuffer, BLOCK_SIZE); // we'll read 4092 samples - mono

			for(int i=0; i<framesRead; i++)
			{
				leftData.push_back(readBuffer[i]); // just use left channel
			}
			totalRead += framesRead;
			// cout << "totalRead: " << totalRead << endl;
			
			if(framesRead < BLOCK_SIZE) // if this was should be the last time...
				readDone = true;
			
			if(framesRead > MAX_SECONDS*44100) // if file's too long, cut off and exit
				readDone = true;
		}
	}
	// we have a stereo file
	else
	{
		while(!readDone)
		{
			framesRead = sf_readf_float(sndFile, readBuffer, BLOCK_SIZE); // read 4096 samples - stereo interleaved
			
			for(int i=0; i<framesRead; i++) // if you read 4096 frames, 8192 floats in array are returned
			{
				leftData.push_back(readBuffer[i*2]); // interleaved...
				rightData.push_back(readBuffer[i*2+1]); // like (left, right), (left, right), ...
			}
			totalRead += framesRead;
			// cout << "totalRead: " << totalRead << endl;

			if(framesRead < BLOCK_SIZE) // if this was should be the last time...
				readDone = true;
				
			if(framesRead > MAX_SECONDS*44100)
				readDone = true;

		}
	}
	sf_close(sndFile);
	
		// cout << "read done.. total read = " << totalRead << "\n";
		// cout << "size of LEFT vector = " << leftData.size() << "\n";
		// cout << "size of RIGHT vector = " << rightData.size() << "\n";	
	
	// update the dataSize..
	dataSize = totalRead;
	error = "(no error)"; // was success!
	return true;
}

// set the main gain + left/right gain
void Sound::setGain(float g)
{
	g = min(1.0f, max(0.0f, g));
	gain = g;
	leftGain = (gain * 2) * panning;
	rightGain = (gain * 2) * (1.0f - panning);	
}

// returns the main channel gain
float Sound::getGain()
	{ return gain; }

// panning factor 0 to 1
// smaller << left -- bigger >> right
void Sound::setPanning(float p)
{
	p = min(1.0f, max(0.0f, p));
	panning = p;
	leftGain = (gain * 2.0f) * (1.0f - p);
	rightGain = (gain * 2.0f) * p;
}

// returns the panning factor
float Sound::getPanning()
	{ return panning; }

void Sound::refresh()
{  pos = 0; }	

void Sound::start()
{ 
	refresh();
	playing = true;
}

void Sound::stop()
{
	playing = false;
	refresh();
}

void Sound::pause()
	{ playing = false; }

void Sound::resume()
	{ playing = true; }
	
// returns the output at current pos
// channel: 0 - left, 1 - right
float Sound::update(int channel)
{
	if(!playing) // if this sound is not playing, return silence
		return 0.0;

	float output;
	if(channel==0) // left channel
	{
		output = leftData[pos] * leftGain;
	}
	else // right channel
	{
		// if stereo, use right channel..
		if(stereo)
			output = rightData[pos] * rightGain;
		else // if mono, just reuse left channel data
			output = leftData[pos] * rightGain;
			
		// advance frames ONLY after processing right channel!
		pos++;
	}

	if(pos >= dataSize) // reached end of the sound?
	{
		playing = false; // back to silence
		refresh();
	}
	return output;
}




////////////////////////////////////////////////////
// SFX class implementation ////////////////////////

#include <string>
#include <algorithm>
#include "BC/SFX.h"

using namespace std;

SFX::SFX()
{
	compThreshold = 0.6f;
	compRatio = 5.0f;
}

float SFX::getGain(int slot)
{
	if(slot >= 0 && slot < N_SLOTS)
		return sound[slot].getGain();
}

void SFX::setGain(int slot, float g)
{
	if(slot >= 0 && slot < N_SLOTS)
		sound[slot].setGain(g);	
}

float SFX::getPanning(int slot)
{
	if(slot >= 0 && slot < N_SLOTS)
		return sound[slot].getPanning();	
}

void SFX::setPanning(int slot, float p)
{
	if(slot >= 0 && slot < N_SLOTS)
		sound[slot].setPanning(p);	
}

std::string SFX::getErrorText(int slot)
{ 
	if(slot >= 0 && slot < N_SLOTS)
		return sound[slot].getErrorText();
}
	
bool SFX::loadSound(int slot, const std::string &filename)
{
	if(slot >= 0 && slot < N_SLOTS)
		return sound[slot].loadFile(filename);
	else
		return false;
}

void SFX::start(int slot)
{
	if(slot >= 0 && slot < N_SLOTS)
		sound[slot].start();
}

void SFX::stop(int slot)
{
	if(slot >= 0 && slot < N_SLOTS)
		sound[slot].stop();
}

void SFX::pause(int slot)
{
	if(slot >= 0 && slot < N_SLOTS)
		sound[slot].pause();	
}

void SFX::resume(int slot)
{
	if(slot >= 0 && slot < N_SLOTS)
		sound[slot].resume();
}

// 0 - left, 1 - right
float SFX::getOutput(int channel)
{
	float output = 0.0;
	
	// mix all sounds for this channel...
	for(int i=0; i<N_SLOTS; i++)
	{
		output += sound[i].update(channel);
	}
	
	// and compress... limit and send away
	return min(0.99f, compress(output));
}

// compress mixed SFX signal
float SFX::compress(float input)
{
	float output;

	// if positive
	if(input >= 0)
	{
		if(input < compThreshold)
			return input;
		else
		{
			output = compThreshold + ((input - compThreshold) / compRatio);
		}
	}

	// if negative
	if(input < 0)
	{
		if(input > -compThreshold)
			return input;
		else
		{
			output = -compThreshold + ((input + compThreshold) / compRatio);
		}
	}
	
	return output;
}




// MData.cpp ////////////////////////////////////////////
// MData Class - Implementation /////////////////////////

#include <vector>
#include <algorithm>
#include "BC/MData.h"

using namespace std;

const int MData::SAMPLE_RATE = 44100;

MData::MData()
{
	sampleRate = SAMPLE_RATE;
	totalFrames = 0;
}

MData::MData(int sRate)
{
	sampleRate = sRate;
	totalFrames = 0;
}

MData::~MData()
{
}

// makes all data empty for this MData
void MData::clear()
{
	freqNote.clear();
	freqNote.resize(0);
	len.clear();
	len.resize(0);
	param.clear();
	param.resize(0);
	totalFrames = 0;
}

// returns the size of MData array
int MData::getSize()
{
	return min( min( freqNote.size(), len.size() ), param.size() );
}




// DData.cpp ////////////////////////////////////////////
// DData Class - Implementation /////////////////////////

#include <vector>
#include <algorithm>
#include "BC/DData.h"

using namespace std;

const int DData::SAMPLE_RATE = 44100;

DData::DData()
{
	sampleRate = SAMPLE_RATE;
	totalFrames = 0;
}

DData::DData(int sRate)
{
	sampleRate = sRate;
	totalFrames = 0;
}

DData::~DData()
{
}

// makes all data empty for this DData
void DData::clear()
{
	drumNote.clear();
	drumNote.resize(0);
	len.clear();
	len.resize(0);
	param.clear();
	param.resize(0);
	totalFrames = 0;
}

// returns the size of DData array
int DData::getSize()
{
	return min(   min( drumNote.size(), len.size() ),  param.size()  );
}




// Astro.cpp /////////////////////////////////////////////
// Astro class - implementation //////////////////////////

#include <algorithm>
#include "BC/Astro.h"

using namespace std;

const int Astro::ASTRO_SAMPLE_RATE = 44100;

Astro::Astro()
{
	frameCount = 0;
	processedFrequency = 440.0;
	oneCycleFrames = 4410;
	setSpeed(12); // default - 10 cycles per second
	statusChanged = false;
}

Astro::~Astro()
{}

void Astro::setSpeed(int nCyclesPerSecond)
{
	nCyclesPerSecond = min(100, nCyclesPerSecond);
	nCyclesPerSecond = max(1, nCyclesPerSecond);
	oneCycleFrames = ASTRO_SAMPLE_RATE / nCyclesPerSecond;
	middlePoint = oneCycleFrames / 2;
}

double Astro::process(double freq)
{
	if( frameCount == middlePoint ) // at middle of range - go to octave higher
	{
		processedFrequency = freq * 2.0;
		statusChanged = true;
	}
	else if(frameCount == 0 ) // back to beginning - go to base octave
	{
		processedFrequency = freq;
		statusChanged = true;
	}
	else
		statusChanged = false;
	
	// update frame count
	frameCount++;
	if(frameCount >= oneCycleFrames)
		frameCount = 0;
	
	return processedFrequency;
}

// whenever Astro changes the frequency, statusChanged flag turns on
bool Astro::stateChanged()
	{ return statusChanged; }

// forces the Astro effect cycle to begin from beginning (frameCount = 0)
void Astro::refresh()
	{ frameCount = 0;}




// LFO.cpp ////////////////////////////////////////
// LFO Class - Implementation /////////////////////

#include <iostream>
#include <math.h>
#include <vector>
#include "BC/LFO.h"

using namespace std;

const double LFO::LFO_SAMPLE_RATE = 44100.0;
const int LFO::LFO_TABLE_SIZE = 4096;
const double LFO::LFO_TWO_PI = 6.28318530718;

LFO::LFO()
{
	// set table size, then set table to sine wave table
	table.resize(LFO_TABLE_SIZE);
	setTable(0); // default table - sine wave
	
	// initialize variables with default settings
	phase = 0;
	waitPos = 0;
	setWaitTime(200); // default wait - n milliseconds
	setRange(24); // default LFO range - n cents of semitone
	setSpeed(6.0); // defalt LFO speed - n cycles per second 
}

LFO::~LFO()
{}
	
void LFO::setTable(int type)
{
	switch(type)
	{
		// 0 - sine wave
		case 0:
		
			for(int i=0; i<LFO_TABLE_SIZE; i++)
			{
				double radian = ( static_cast<double>(i) / static_cast<double>(LFO_TABLE_SIZE) ) * LFO_TWO_PI;
				table[i] = sin(radian);
			}

			break;
			
		case 1:
		
			break;
			
		default:
			break;
	}
	
}

void LFO::setWaitTime(int milliseconds)
{
	waitTimeMSec = milliseconds;
	waitFrames = LFO_SAMPLE_RATE * milliseconds / 1000.0;
}

void LFO::setRange(int cents)
{
	rangeCents = static_cast<double>(cents);
}

void LFO::setSpeed(double cyclesPerSeconds)
{
	cyclesPerSec = cyclesPerSeconds;
	increment = static_cast<double>(LFO_TABLE_SIZE) * cyclesPerSec / LFO_SAMPLE_RATE;
}

// restart LFO mechanism - at start of every note
void LFO::refresh()
{
	phase = 0;
	waitPos = 0;
}

// takes the original frequency value from oscillator
// then returns the value processed with LFO
double LFO::process(double originalFreq)
{
	double freqToReturn;
	
	// if still in 'wait' stage - no LFO processing
	if(waitPos < waitFrames)
	{
		waitPos++;
		freqToReturn = originalFreq;
	}
	// wait stage is over - will return LFO-processed output
	else
	{
		double LFOfactor = pow(2.0, (static_cast<double>(table[(int)phase]) * rangeCents / 100.0 / 12.0 ) );
		freqToReturn = originalFreq * LFOfactor;

		// advance phase - let LFO move through table
		phase += increment;
		if(phase >= LFO_TABLE_SIZE)
		phase -= LFO_TABLE_SIZE;		
	}
	
	return freqToReturn;
}




/// OSC class - Implementation /////////////////////////

#include <iostream>
#include <math.h>
#include "BC/OSC.h"

using namespace std;

const int OSC::OSC_TABLE_SIZE = 8192;
const int OSC::ENV_TABLE_SIZE = 1024;
const double OSC::OSC_SAMPLE_RATE = 44100.0;
const float OSC::TWO_PI = 6.283185307;

OSC::OSC()
{
	table.resize(OSC_TABLE_SIZE); // wave table (vector)
	
	setTable(1); // default - set up a square table
	phase = 0.0;
	increment = 0.0;
	freq = 10.0; // not to set to zero to safeguard
	adjustedFreq = 0;
	detune = 0;
	gain = 0.5f; // default gain
	
	resting = false;
	
	nAttackFrames = 1000;
	nPeakFrames = 1000;
	nDecayFrames = 9600;
	nEnvFrames = nAttackFrames + nPeakFrames + nDecayFrames;
	decayStartPos = nAttackFrames + nPeakFrames;
	
	peakLevel = 0.9f;
	sustainLevel = 0.5f;
	decayAmount = peakLevel - sustainLevel;
	envPos = 0;
	envADfinished = false;
	
	nReleaseFrames = 2000;
	releasePos = 0;
	envRfinished = true;
	
	astroEnabled = false;
	lfoEnabled = false;
	
	// initialize history table
	clearHistory();
}

OSC::~OSC()
{}

void OSC::setTable(int type)
{
	switch(type)
	{
		float maxAmp;
		int oneCycleFrames;
		
		// sine table
		case 0:
			maxAmp = 0.99f;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = sin( TWO_PI * (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE) ) ) * maxAmp;
			}
			break;
			
		// square table
		case 1:
		
			for(int i=0; i<OSC_TABLE_SIZE/2; i++)
			{
				table[i] = 0.85f;
			}
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = -0.85f;
			}
			break;
		
		// sawthooth wave
		case 2:
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] = -0.99f  + (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE)) * 1.98f;
			break;

		// triangle wave
		case 3:
			for(int i=0; i<OSC_TABLE_SIZE/2; i++)
				table[i] = -0.99f  + (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE/2)) * 1.98f;	
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
				table[i] = 0.99f  - (static_cast<float>(i-OSC_TABLE_SIZE/2) / static_cast<float>(OSC_TABLE_SIZE/2)) * 1.98f;
			break;
		
		// sine wave with 3rd, 6th, 9th, 12th harmonics
		case 4:
		
			maxAmp = 0.90f;
			
			// first order sine as base
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] = sin( TWO_PI * (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE) ) ) * maxAmp;
			
			// then add 3rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 3;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 3.0f);
			
			// then add 6rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 6;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 6.0f);
							
			// then add 9rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 9;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 9.0f);			

			// then add 12th harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 12;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 12.0f);		
	
		break;
	
		// sine wave with 2nd, 3rd, 4th harmonics
		case 5:
		
			maxAmp = 0.68f;
			
			// first order sine as base
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] = sin( TWO_PI * (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE) ) ) * maxAmp;
			
			// then add 2rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 2;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 2.0f);
			
			// then add 3rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 3;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 3.0f);
							
			// then add 4rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 4;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 4.0f);			

			// then add 5rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 5;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 5.0f);								
			
			break;

		// default is sine wave...
		default:
			maxAmp = 0.99f;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] = sin( TWO_PI * (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE) ) ) * maxAmp;
			
			break;
			
	}
	
	// limit...
	for(int i=0; i<OSC_TABLE_SIZE; i++)
	{
		if(table[i]>0.99f) table[i] = 0.99f;
		else if(table[i] < -0.99f) table[i] = -0.99f;
	}	
	
}

void OSC::setGain(float g)
	{ gain = g; }

float OSC::getGain()
	{ return gain; }

void OSC::advanceEnvelope()
{
	if(!resting) // currently playing a note
	{
		if(!envADfinished)
		{
			envPos++;
			if(envPos >= nEnvFrames)
				envADfinished = true;
		}
	}
	else // you are on a rest now
	{
		if(!envRfinished)
		{
			releasePos++;
			if(releasePos >= nReleaseFrames)
			{
				envRfinished = true;
			}
		}
	}
}

// go back to the beginning of envelope
void OSC::refreshEnvelope()
{
	envPos = 0;
	envADfinished = false;
	releasePos = 0;
	envRfinished = false;
}

float OSC::getEnvelopeOutput()
{
	float output;
	
	if(!resting) // when you're not on a rest - playing a note now
	{
		if(envPos < nAttackFrames) // in attack stage
		{
			output = peakLevel * 
			(static_cast<float>(envPos) / static_cast<float>(nAttackFrames));
		}
		else if(envPos < (nAttackFrames + nPeakFrames)) // in peak stage
		{
			output = peakLevel;
		}
		else if(envPos < nEnvFrames) // in decay stage
		{
			output = peakLevel - decayAmount * ( static_cast<float>(envPos - decayStartPos) / static_cast<float> (nDecayFrames) );
		}
		else if(envADfinished) // in sustain stage
		{
			output = sustainLevel;
		}
	}
	
	// if resting flag is on, means you're in release stage
	if(resting)
	{
		if(!envRfinished)
			output = sustainLevel * ( static_cast<float>(nReleaseFrames - releasePos) / static_cast<float>(nReleaseFrames) );
		else
			output = 0.0f;
	}
	
	return output;
}

void OSC::setToRest()
{
	resting = true;
}

void OSC::advance()
{
	// advance on the sample table
	phase += increment;
	while(phase >= OSC_TABLE_SIZE)
	{
		phase -= OSC_TABLE_SIZE;
	}
	
	// if astro is enabled, process and adjust frequency
	if(astroEnabled)
	{
		adjustedFreq = astro.process(freq);
		if(astro.stateChanged())
			setIncrement(adjustedFreq);
	}
	// if LFO is enabled, process and adjust frequency
	else if(lfoEnabled)
	{
		adjustedFreq = lfo.process(freq);
		if(adjustedFreq < 10.0)
			adjustedFreq = 10.0;
		setIncrement(adjustedFreq);
	}
	
	// advance envelope also
	advanceEnvelope();
}

void OSC::setNewNote(double newFreq)
{
	setFrequency(newFreq);
	initializePhase();
	refreshEnvelope();
	resting = false;
}

// set the frequency and phase increment at once
void OSC::setFrequency(double noteFreq)
{
	freq = noteFreq;
	adjustedFreq = freq;
	setIncrement(freq);
	
	// refresh LFO so it starts from beginning
	if(lfoEnabled)
		lfo.refresh();
	
	// refresh Astro effect so it starts from beginning
	if(astroEnabled)
		astro.refresh();
}

// set the phase increment to travel across wave table every frame
void OSC::setIncrement(double noteFreq)
{
	// calculate with detune
	adjustedFreq = noteFreq + detune;

	// finally, set the phase increment
	increment = ( static_cast<double>(OSC_TABLE_SIZE) / ( OSC_SAMPLE_RATE / adjustedFreq ) );
	
	if(increment < 0)
		increment = 0;
}

void OSC::enableAstro()
	{ astroEnabled = true; }

void OSC::disableAstro()
	{ astroEnabled = false; }
	
void OSC::setAstroSpeed(int nCyclesPerSecond)
	{ astro.setSpeed(nCyclesPerSecond); }

void OSC::enableLFO()
	{ lfoEnabled = true; }

void OSC::disableLFO()
	{ lfoEnabled = false; }
	
void OSC::setLFOwaitTime(int milliseconds)
	{ lfo.setWaitTime(milliseconds); }

void OSC::setLFOrange(int cents)
	{ lfo.setRange(cents); }

void OSC::setLFOspeed(double cyclePerSeconds)
	{ lfo.setSpeed(cyclePerSeconds); }

void OSC::setAttackTime(int attackTimeMS)
{
	nAttackFrames = static_cast<int> (OSC_SAMPLE_RATE * attackTimeMS / 1000.0);
	readjustEnvParams();
}

void OSC::setPeakTime(int peakTimeMS)
{
	nPeakFrames = static_cast<int> (OSC_SAMPLE_RATE * peakTimeMS / 1000.0);
	readjustEnvParams();
}

void OSC::setDecayTime(int decayTimeMS)
{
	nDecayFrames = static_cast<int> (OSC_SAMPLE_RATE * decayTimeMS / 1000.0);
	readjustEnvParams();
}

void OSC::setReleaseTime(int releaseTimeMS)
{ 
	nReleaseFrames = static_cast<int> (OSC_SAMPLE_RATE * releaseTimeMS / 1000.0);
	readjustEnvParams();
}
	
void OSC::setPeakLevel(float peakLV)
{
	peakLevel = peakLV;
	readjustEnvParams();
}

void OSC::setSustainLevel(float sustainLV)
{
	sustainLevel = sustainLV;
	readjustEnvParams();
}	

void OSC::setEnvelope(int attackTimeMS, int peakTimeMS, int decayTimeMS, int releaseTimeMS,
						float peakLV, float sustainLV)
{
	setAttackTime(attackTimeMS);
	setPeakTime(peakTimeMS);
	setDecayTime(decayTimeMS);
	setReleaseTime(releaseTimeMS);
	setPeakLevel(peakLV);
	setSustainLevel(sustainLV);
	readjustEnvParams();
}

void OSC::readjustEnvParams()
{
	nEnvFrames = nAttackFrames + nPeakFrames + nDecayFrames;
	decayStartPos = nAttackFrames + nPeakFrames;
	decayAmount = peakLevel - sustainLevel;	
}
	
void OSC::initializePhase()
{
	phase = 0;
}

float OSC::getOutput()
{
	/*
	// trying out linear interpolation...
	deviation = modf(phase, &dblPhaseIntPart);
	intPhase = static_cast<int>(dblPhaseIntPart);
	intPhaseNext = intPhase++;
	if(intPhaseNext >= OSC_TABLE_SIZE)
		intPhaseNext = 0;
	
	float out = (table[intPhase] + (table[intPhaseNext] - table[intPhase]) * deviation) 
		* getEnvelopeOutput() * gain;
	*/
	
	int ph = static_cast<int> (phase);
	// cout << "phase=" << phase << "..";
	
	float out = table[ph] * getEnvelopeOutput() * gain;
	
	historyWriteWait++;
	if(historyWriteWait >= 8)
	{
		pushHistory(out);
		historyWriteWait = 0;
	}
	
	return out;
}

// push current data to table that keeps an array of historical data
// (used for meter visualization)
void OSC::pushHistory(float g)
{
	if(g < 0) // flip sign when negative
		g = g * -1.0;
	history[historyWriteIndex] = g; // store value
	historyWriteIndex++;
	if(historyWriteIndex >= OSC_HISTORY_SIZE)
		historyWriteIndex = 0;
}

// get the current averaged output
// (average derived from (HISTORY_SIZE * historyWriteWait) frames)
float OSC::getHistoricalAverage()
{
	float sum = 0;
	for(int i=0;i<OSC_HISTORY_SIZE; i++)
		sum += history[i];
	float avg = static_cast<float>(sum / OSC_HISTORY_SIZE);
	return avg;
}

void OSC::clearHistory()
{
	for(int i=0;i<OSC_HISTORY_SIZE; i++)
		history[i] = 0.0f;
	historyWriteWait = 0;
	historyWriteIndex = 0;
}




// NOSC.cpp ////////////////////////////////////////////
// NOSC class - Implementation /////////////////////////

#include <algorithm>
#include <iostream>
#include <cstdlib>
//#?include <windows.h>
#include <vector>
#include "BC/NOSC.h"

using namespace std;

const int NOSC::NOSC_NTABLE_SIZE = 4096;
const int NOSC::NOSC_PTABLE_SIZE = 4096;
const double NOSC::NOSC_SAMPLE_RATE = 44100.0;

NOSC::NOSC()
{	
	// set up a noise wave nTable
	nTable.resize(NOSC_NTABLE_SIZE);
	pTable.resize(NOSC_PTABLE_SIZE);
	setTable();
	
	// initialize NOSC variables
	phase = 0.0;
	pPhase = 0.0;
	increment = 0.0;
	gain = 0.5f; // default gain
	resting = true;
	drumType = 0;
	pLevel = 1.0f;
	pPitchFall = 0;
	
	// set default envelope for each drum
	setDrumTone(0, 3, 30,  20,  0.8, 200.0,  50  , 0.9, 2.0); // kick
	setDrumTone(1, 3, 20, 120, 0.95, 720.0,  100 , 0.85, 1.2); // snare
	setDrumTone(2, 3, 17,  3,  0.5, 2400.0, 1   , 0.2, 2.0); // hat
	
	setDrumTone(3, 3, 30,  20,  0.4, 200.0,  50  , 0.5, 2.0); // kick - quiet
	setDrumTone(4, 3, 20, 120, 0.5, 720.0,  100 , 0.45, 1.2); // snare - quiet
	setDrumTone(5, 3, 17,  3,  0.2, 2400.0, 1   , 0.1, 2.0); // hat - quiet

	envPos = 0;	
	
	envFinished = false;
	
	// initialize history table
	clearHistory();
}

NOSC::~NOSC()
{}

void NOSC::setTable()
{
	// fill nTable with random numbers ranging from -1 to 1
	for(int i=0; i<NOSC_NTABLE_SIZE; i++)
		nTable[i] = ( (float) rand() / RAND_MAX ) * 2.0f - 1.0f;
	
	int midTablePoint = NOSC_PTABLE_SIZE / 2;
	
	for(int i=0; i<midTablePoint; i++)
		pTable[i] = 0.95f;
	for(int i=midTablePoint; i<NOSC_PTABLE_SIZE; i++)
		pTable[i] = -0.95f;
}

void NOSC::setGain(float g)
	{ gain = g; }

float NOSC::getGain()
	{ return gain; }

void NOSC::rest()
	{ resting = true; }
	
void NOSC::unrest()
	{ resting = false; }
	
// set envelope for a drum
//
//	param:	drum type
//			attack time, peak time, decay time (in seconds, double)
//			peak volume (float)
//			frequency (double)
void NOSC::setDrumTone(int dType, double nMilSecAttack, double nMilSecPeak, double nMilSecDecay, float peakVol, 
	double freq, double nMilSecPTime, float pBeginningLevel, double pFallRatio)
{
	nAttackFrames[dType] = static_cast<int> (NOSC_SAMPLE_RATE * nMilSecAttack / 1000.0);
	nPeakFrames[dType] = static_cast<int> (NOSC_SAMPLE_RATE * nMilSecPeak / 1000.0);
	nDecayFrames[dType] = static_cast<int> (NOSC_SAMPLE_RATE * nMilSecDecay / 1000.0);
	nEnvFrames[dType] = nAttackFrames[dType] + nPeakFrames[dType] + nDecayFrames[dType];
	peakLevel[dType] = peakVol;
	frequency[dType] = freq;
	pitchFallDelta[dType] = (freq / pFallRatio) / (NOSC_SAMPLE_RATE * nMilSecPTime / 1000.0);
	pitchFallLimit[dType] = freq / pFallRatio;
	pStartLevel[dType] = pBeginningLevel;
	levelFallDelta[dType] = peakLevel[dType] / static_cast<float>(NOSC_SAMPLE_RATE * nMilSecPTime/1000.0);
	
	cout << "attack=" << nAttackFrames[dType] << " peakTime=" << nPeakFrames[dType] << " decayTime" << nDecayFrames[dType] << " envFrames=" << nEnvFrames[dType] << endl;
}

void NOSC::setNewDrum(int dType)
{
	if(dType >= 65535) // means rest
		resting = true;
	else
	{
		resting = false;
		drumType = dType;
		pPitchFall = 0;
		pLevel = pStartLevel[drumType];
		setIncrement(dType);
		initializePhase();
		refreshEnvelope();
	}
}
	
void NOSC::advance()
{
	// advance on the sample nTable
	phase += 1.0;
	if(phase >= NOSC_NTABLE_SIZE)
		phase -= NOSC_NTABLE_SIZE;
	
	// advance on the sample pTable
	pPhase += increment;
	while(pPhase >= NOSC_PTABLE_SIZE)
	{
		pPhase -= NOSC_PTABLE_SIZE;
	}
	
	// keep adjusting increment for falling pitch part
	setIncrement(drumType);
	
	// decrease the pitch + pitch pt level
	if(pPitchFall > -pitchFallLimit[drumType])
		pPitchFall -= pitchFallDelta[drumType];
	
	pLevel -= levelFallDelta[drumType];
	if(pLevel <= 0.0f)
		pLevel = 0.0f;
		
	// advance envelope also
	advanceEnvelope();
}

void NOSC::setIncrement(int dType)
{
	double adjustedFrequency = frequency[dType] + pPitchFall;
	increment = static_cast<double>(NOSC_PTABLE_SIZE) /  (static_cast<double>(NOSC_SAMPLE_RATE) / adjustedFrequency);
	if(increment < 0)
		increment = 0;
}

void NOSC::initializePhase()
	{ phase = 0; }

// advance envelope frame by one frame
void NOSC::advanceEnvelope()
{
	if(!envFinished)
		envPos++;
	if(envPos >= nEnvFrames[drumType])
		envFinished = true;
}

void NOSC::refreshEnvelope()
{
	envPos = 0;
	envFinished = false;
}

float NOSC::getEnvelopeOutput()
{
	float output = 0.0f;
	
	if(resting) // currently playing a "REST", output just zero
	{
		output = 0.0f;
	}
	else if(envPos < nAttackFrames[drumType]) // in attack stage
	{
		output = ( (float) envPos / (float) nAttackFrames[drumType] ) * peakLevel[drumType];
	}
	else if( envPos < (nAttackFrames[drumType] + nPeakFrames[drumType]) ) // in peak stage
	{
		output = peakLevel[drumType];
	}
	else if( envPos < (nAttackFrames[drumType] + nPeakFrames[drumType] + nDecayFrames[drumType]) ) // in decay stage
	{
		int decayPos = envPos - (nAttackFrames[drumType] + nPeakFrames[drumType]);
		output = peakLevel[drumType] - ((float) decayPos / (float) nDecayFrames[drumType]) * peakLevel[drumType];
	}
	else if(envFinished)
	{
		output = 0.0f;
	}
	
	return output;
}

float NOSC::getPitchOutput()
{
	float pOut = 0.0f;
	int intPhase = (int)pPhase;
	pOut = pTable[ intPhase ] * pLevel;
	return pOut;
}

// get the summed output from wave nTable and envelope
float NOSC::getOutput()
{
	int ph = (int) phase;
	float noiseOut = nTable[ph];
	float pitchOut = getPitchOutput();
	float out = ( noiseOut * gain + pitchOut * gain) * getEnvelopeOutput();
	
	historyWriteWait++;
	if(historyWriteWait >= 8)
	{
		pushHistory(out);
		historyWriteWait = 0;
	}	
	
	return out;
	// return ( noiseOut * gain + pitchOut * gain) * getEnvelopeOutput();
}

// push current data to table that keeps an array of historical data
// (used for meter visualization)
void NOSC::pushHistory(float g)
{
	if(g < 0) // flip sign when negative
		g = g * -1.0;
	history[historyWriteIndex] = g; // store value
	historyWriteIndex++;
	if(historyWriteIndex >= NOSC_HISTORY_SIZE)
		historyWriteIndex = 0;
}

// get the current averaged output
// (average derived from (HISTORY_SIZE * historyWriteWait) frames)
float NOSC::getHistoricalAverage()
{
	float sum = 0;
	for(int i=0;i<NOSC_HISTORY_SIZE; i++)
		sum += history[i];
	float avg = static_cast<float>(sum / NOSC_HISTORY_SIZE);
	return avg;
}

void NOSC::clearHistory()
{
	for(int i=0;i<NOSC_HISTORY_SIZE; i++)
		history[i] = 0.0f;
	historyWriteWait = 0;
	historyWriteIndex = 0;
}




// DelayLine class - implementation //////////////////////////////////////

#include "BC/DelayLine.h"

const int DelayLine::DELAY_TABLE_SIZE = 88200;
const int DelayLine::DELAY_SAMPLE_RATE = 44100;

DelayLine::DelayLine()
{
	// set up tables (vector)
	buffer1.resize(DELAY_TABLE_SIZE);
	buffer2.resize(DELAY_TABLE_SIZE);

	// set max buffer lengths as default
	buffer1len = DELAY_TABLE_SIZE;
	buffer2len = DELAY_TABLE_SIZE;
	totalDelayFrames = 0;
	readIndex1 = 1;
	writeIndex1 = 0;
	readIndex2 = 1;
	writeIndex2 = 0;
	out1 = 0.0f;
	out2 = 0.0f;
	outGain1 = 0.5f;
	outGain2 = 0.2f;
}

DelayLine::~DelayLine()
{}

//
// function to set delay parameters
// firstDelayTime / delayTime in milliseconds, delayGain in float (about 0.2f is recommended)
//
void DelayLine::setParameters(int firstDelayTime, int delayTime, float delayGain)
{
	if(firstDelayTime>=0 && delayTime>=0) // if negative was passed, don't make change
	{
		buffer1len = (DELAY_SAMPLE_RATE * firstDelayTime) / 1000;
		buffer2len = (DELAY_SAMPLE_RATE * delayTime) / 1000;

		totalDelayFrames = buffer1len + buffer2len * 2;

		// reset the indexes, too, while you're at it (just to be safe)
		readIndex1 = 1;
		writeIndex1 = 0;
		readIndex2 = 1;
		writeIndex2 = 0;
	}

	if(delayGain > -0.001f) // if negative was passed, don't make change
	{
		outGain1 = delayGain;
		outGain2 = delayGain * 0.4f;
	}
}

// call to update delayline every frame
// returns the delay output for current frame
float DelayLine::update(float input)
{
	// handle buffer1 write & read
	buffer1[writeIndex1] = input;
	out1 = buffer1[readIndex1];

	// handle buffer2 write & read
	buffer2[writeIndex2] = out1;
	out2 = buffer2[readIndex2];

	// update buffer1 + buffer2 indexes
	writeIndex1++;
	if(writeIndex1 >= buffer1len)
		writeIndex1 = 0;
	readIndex1++;
	if(readIndex1 >= buffer1len)
		readIndex1 = 0;
	writeIndex2++;
	if(writeIndex2 >= buffer2len)
		writeIndex2 = 0;
	readIndex2++;
	if(readIndex2 >= buffer2len)
		readIndex2 = 0;

	// return the summed output for the whole delay line
	return min(out1 * outGain1, 1.0f) + min(out2 * outGain2, 1.0f);
}

// erase all buffer tables
void DelayLine::clearBuffer()
{
	for(int i=0; i<DELAY_TABLE_SIZE; i++)
		buffer1[i] = 0;
	for(int i=0; i<DELAY_TABLE_SIZE; i++)
		buffer2[i] = 0;

	// reset the indexes, too
	readIndex1 = 1;
	writeIndex1 = 0;
	readIndex2 = 1;
	writeIndex2 = 0;
}




/// MML Class - Implementation ////////////////

#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <string>
//#?include <windows.h>
#include "BC/MML.h"
#include "BC/MData.h"
#include "BC/DData.h"
#include "BC/MPlayer.h"

MML::MML()
{}

MML::MML(double sampleRate, double tempo)
{
	initialize(sampleRate, tempo);
}

MML::~MML()
{}

void MML::initialize(double sampleRate, double tempo)
{
	this->sampleRate = sampleRate;
	this->tempo = tempo;
	calculateTiming();

	octave = 4;						// default octave is 4
	noteLength = baseLength * 2;	// set default to 8th notes

	semitoneRatio = pow(2, 1.0/12.0);
	middleC = 220.0 * pow(semitoneRatio, 3); // middle C is C4
	cZero = middleC * pow(0.5, 4.0);

	for(int i=0; i<9; i++)
	{
		source[i] = "   ";
	}
	dsource = "    ";
	gsource = "    ";
}

void MML::calculateTiming()
{
	baseLength = static_cast<int>(this->sampleRate * 7.5 / tempo); // n of frames for 32nd note
	measureLength = baseLength * 32;
	halfMeasureLength = baseLength * 16;
	quarterNoteLength = baseLength * 8;
}

string MML::setSource(string masterStr)
{
	// save untouched original source
	originalSource = masterStr;

	masterStr += "   "; // safeguarding...
	size_t found;

	// take out all comments from the string
	masterStr = takeOutComments(masterStr);
	masterStr = takeOutSpaces(masterStr);

	// initialize all source strings

	for(int i=0; i<9; i++)
	{
		source[i] = "    ";
	}

	dsource = "    ";
	gsource = "    ";

	int masterStrLen = masterStr.length();
	for(int i=0; i<masterStrLen; i++)
	{
		if(masterStr.at(i)=='@')
		{
			char nextCh = masterStr.at(i+1); // get the char after '@'

			if(nextCh >= '1' && nextCh <= '9') // if a number is found, music source
			{
				int channel = nextCh - '1'; // @ number (1 to 9) to channel number (0 to 8)

				source[channel] = masterStr.substr(i+2);
				found = source[channel].find('@');  // try to search for next '@'
												 // - and extract part just before it
				if(found!=string::npos)
					source[channel] = source[channel].substr(0, found);

				/*
				// DEBUG
				cout << "channel source extracted - " << channel << "\n";
				cout << source[channel] << "\n\n";
				while(!GetAsyncKeyState(VK_SPACE)){}
				while(GetAsyncKeyState(VK_SPACE)){}
				*/
			}
			else if(nextCh=='d' || nextCh=='D') // drum channel source
			{
				dsource = masterStr.substr(i+2);
				found = dsource.find('@');
				if(found!=string::npos)
					dsource = dsource.substr(0, found);
			}
			else if(nextCh=='g' || nextCh=='G') // global definition source found
			{
				gsource = masterStr.substr(i+2);
				found = gsource.find('@');
				if(found!=string::npos)
					gsource = gsource.substr(0, found);
			}
		}
	}

	return "success";
}

string MML::getSource()
{
	return originalSource;
}

// function to take out all comments from source string
string MML::takeOutComments(string masterStr)
{
	string str = masterStr + "  $$$$$$$$";
	string comment;
	string comments = "";
	char ch, ch2;
	bool done = false;
	int i=0;
	size_t found, found2, found3;

	while(!done)
	{
		ch = str.at(i);
		ch2 = str.at(i+1);

		if(ch=='$') // now at last char
		{
			done = true;
		}
		else if(ch==47 && ch2==47) // two backslashes found
		{
			comment = str.substr(i);
			found = comment.find('\n');
			found2 = comment.find('$');
			found3 = comment.find('\r');
			if(found!=string::npos) // '\n' was found
				comment = comment.substr(0, found);
			else if(found2!=string::npos) // '$' was found
				comment = comment.substr(0, found2);
			else if(found3!=string::npos) // '\r' was found
				comment = comment.substr(0, found3);
			else
				comment = "";

			// take out the comment that was just found
			str.replace(i, comment.length(), " ");
			comments += comment + '\n';
			i++;
		}
		else // nothing found, move on to next
		{
			i++;
		}
	}

	// cout << "Eliminated comments:" << endl << comments << endl << endl;

	return str;
}

string MML::takeOutSpaces(string str)
{
	bool done = false;
	size_t found;
	while(!done)
	{
		found = str.find(' ');
		if(found != string::npos)
			str.erase(found, 1);
		else
			done = true;
	}

	// DEBUG
	// cout << "After taking out spaces... resulting string:" << endl << endl << str << endl;

	// take out RETURN char (\n) now 
	done = false;
	while(!done)
	{
		found = str.find('\n');
		if(found != string::npos)
			str.erase(found, 1);
		else
			done = true;
	}
	
	// DEBUG
	// cout << "After taking out RET '\n' chars... resulting string:" << endl << endl << str << endl;
	
	return str;
}

double MML::getFrequency(int toneNum)
{
	return cZero * pow(semitoneRatio, toneNum);
}

// hub function to parse ALL MML source strings
string MML::parse(MPlayer* player)
{

	// initialize parameters back to default
	tempo = 120.0;
	octave = 4;						// default octave is 4
	noteLength = baseLength * 2;	// set default to 8th notes
	
	// player's bookmarked start is of course off in default
	player->setBookmark(0);

	// parse global source
	parseGlobalSource(player);

	// initialize all existing MData
	for(int i=0; i<9; i++)
	{
		player->data[i].clear(); // clear regular channel data
	}
	player->ddata.clear(); // clear drum channel data

	// now parse music channel data
	for(int i=0; i<9; i++)
	{
		parseChannelSource(player, i);
	}

	// parse drum channel data
	parseDrumSource(player);

	return "success";
}

// function to parse an MML string for a particular channel
// will fill the MData object with the data that is read
string MML::parseChannelSource(MPlayer* player, int channel)
{
	// choose the MData object to write to.
	if(channel >= 0 && channel <= 8)
		output = &player->data[channel]; // gets pointer to MData object
	else
		return "Error - choose valid channel!";

	// channel source string to work on
	string str = source[channel];

	//
	// first parse the configuration part
	//
	
	// start by resetting... wavetable = 1 (just to safeguard!)
	player->osc[channel].setTable(1); // square wave	

	bool configDone = false; // when all config statements are parsed, this gets set to true
	size_t found;

	while(!configDone)
	{
		configDone = true;
		
		// follow format -> setEnvelope(int attackTimeMS, int peakTimeMS, int decayTimeMS, 
		//								int releaseTimeMS, float peakLV, float sustainLV)

		// reset tone... to default
		found = str.find("DEFAULTTONE");
		if(found != string::npos)
		{
			configDone = false;
			player->osc[channel].setEnvelope(0, 0, 0, 0, 0.99f, 0.99f);
			str.erase(found, 11); // erase this statement
		}
		
		// sets up a preset tone... pure beep!
		found = str.find("PRESET=BEEP");
		if(found != string::npos)
		{
			configDone = false;
			player->osc[channel].setTable(1); // square wave
			player->osc[channel].setEnvelope(0, 0, 0, 0, 0.65f, 0.65f);
			str.erase(found, 11); // erase this statement
		}
		
		found = str.find("WAVEFORM=");
		if(found != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+9,2); // get 2 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(99, max(0, value)); // floor + ceil the value
			player->osc[channel].setTable(value); // set wavetable for this value
			str.erase(found, 9+valueDigits); // erase this statement
		}

		found = str.find("ATTACKTIME=");
		if(str.find("ATTACKTIME=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+11,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(9999, max(1, value)); // floor + ceil the value

			player->osc[channel].setAttackTime(value); // set attack time to this value
			str.erase(found, 11+valueDigits); // erase this statement
		}

		found = str.find("PEAKTIME=");
		if(str.find("PEAKTIME=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+9,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(9999, max(1, value)); // floor + ceil the value

			player->osc[channel].setPeakTime(value); // set peak time to this value
			str.erase(found, 9+valueDigits); // erase this statement
		}

		found = str.find("DECAYTIME=");
		if(str.find("DECAYTIME=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+10,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(9999, max(1, value)); // floor + ceil the value

			player->osc[channel].setDecayTime(value); // set decay time to this value
			str.erase(found, 10+valueDigits); // erase this statement
		}

		found = str.find("RELEASETIME=");
		if(str.find("RELEASETIME=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+12,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(9999, max(1, value)); // floor + ceil the value

			player->osc[channel].setReleaseTime(value); // set release time to this value
			str.erase(found, 12+valueDigits); // erase this statement
		}

		found = str.find("PEAKLEVEL=");
		if(str.find("PEAKLEVEL=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+10,8); // get 8 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			double value = static_cast<double>( atoi(strValue.c_str()) );
			value = min(100.0, max(0.01, value)); // floor + ceil the value
			float valuef = static_cast<float>(value / 100.0);

			player->osc[channel].setPeakLevel(valuef); // set peak level to this value
			str.erase(found, 10+valueDigits); // erase this statement
		}

		found = str.find("SUSTAINLEVEL=");
		if(str.find("SUSTAINLEVEL=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+13,8); // get 8 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			double value =  static_cast<double>( atoi(strValue.c_str()) );
			value = min(100.0, max(0.01, value)); // floor + ceil the value
			float valuef = static_cast<float>(value / 100.0);

			player->osc[channel].setSustainLevel(valuef); // set sustain level to this value
			str.erase(found, 13+valueDigits); // erase this statement
		}

		found = str.find("ASTRO=");
		if(str.find("ASTRO=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+6,3); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(100, max(1, value) ); // floor + ceil the value

			player->setAstro(channel, value); // set astro to this value
			str.erase(found, 6+valueDigits); // erase this statement
		}

		found = str.find("LFO=ON");
		if(found != string::npos)
		{
			configDone = false;
			player->osc[channel].enableLFO();
			str.erase(found, 6); // erase this statement
		}
		
		found = str.find("LFO=OFF");
		if(found != string::npos)
		{
			configDone = false;
			player->osc[channel].disableLFO();
			str.erase(found, 7); // erase this statement
		}

		found = str.find("LFORANGE=");
		if(str.find("LFORANGE=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+9,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(2400, max(1, value)); // floor + ceil the value

			player->osc[channel].setLFOrange(value); // set LFO range to this value
			str.erase(found, 9+valueDigits); // erase this statement
		}

		found = str.find("LFOSPEED=");
		if(str.find("LFOSPEED=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+9,8); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			double value = static_cast<double>( atof(strValue.c_str()) );
			value = min(100.0, max(0.1, value)); // floor + ceil the value

			player->osc[channel].setLFOspeed(value); // set LFO speed to this value
			str.erase(found, 9+valueDigits); // erase this statement
		}

		found = str.find("LFOWAIT=");
		if(str.find("LFOWAIT=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+8,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(3000, max(1, value)); // floor + ceil the value

			player->osc[channel].setLFOwaitTime(value); // set LFO wait time to this value
			str.erase(found, 8+valueDigits); // erase this statement
		}

	}

	// cout << "after parsing the configuration..." << endl;
	// cout << "channel " << channel << " string is now.." << endl << str << endl;

	//
	// parse the music part
	//

	// first, parse the repeat signs
	// (any repeated parts will be duplicated)

	str = str + "$$$$$$"; // to signal end of string
	bool done = false;

	int i = 0;
	vector<int> leftBraces;
	vector<int> repeatTimes;
	char ch = ' ';
	int leftBracePos = 0;
	string strToCopy = "";
	int nCharsToCopy = 0;

	while(!done)
	{
		ch = str.at(i);

		if(ch=='{') // if left brace is found
		{
			leftBraces.push_back(i); // push this position into stack
			str.erase(i,1); // go ahead and erase this '{'
			
			char chNext = str.at(i); // this should be the char right after '{'
			int numberRead = 0;
			
			// check if a number is followed...
			if(chNext >= '0' && chNext <= '9')
			{
				numberRead = chNext - '0'; // set the num of times to duplicate at right brace
				str.erase(i,1); // go ahead and erase this digit
				if(numberRead==0) numberRead = 1;
				
				// make sure there aren't any more digits after this
				while( (str.at(i)>='0'&&str.at(i)<='9') )
					str.erase(i,1); // erase this digit
			}
			else
				numberRead = 2; // repeat times not specified -> set to twice
			
			// push this number... it'll be popped when right brace is found
			repeatTimes.push_back(numberRead);

		}
		else if (ch=='}') // right brace to close repeat
		{
			str.erase(i,1); // go ahead and erase this '}'
			if(leftBraces.size() > 0) // if stack is empty, ignore
			{
				// pop last element from stack - gets the nearest pos of '{'
				leftBracePos = leftBraces.back();
				leftBraces.pop_back();
				
				// and get the number of times we should duplicate
				int timesToDuplicate = repeatTimes.back();
				repeatTimes.pop_back();
				timesToDuplicate -= 1; // if repeat is x3, duplicate twice :)
				
				nCharsToCopy = i - leftBracePos; // n of chars to duplicate
				strToCopy = str.substr(leftBracePos, nCharsToCopy); // str to be duplicated
				
				if(timesToDuplicate >=1 && timesToDuplicate <=8)
				{
					for(int i=0; i<timesToDuplicate; i++)
						str.insert(leftBracePos+nCharsToCopy, strToCopy);
				}
			}
		}
		else if (ch=='$') // end of parse string
		{
			done = true;
		}
		else
		{
			i++; // irrelevant char, so go to next char
		}
	}

	// DEBUG
	// cout << "took out all repeats. str is now: \n" << str << "\n\n";
	// while(!GetAsyncKeyState(VK_SPACE)){}
	// while(GetAsyncKeyState(VK_SPACE)){}

	done = false;
	string result = "";
	ch = ' ';
	i = 0;
	// int len = str.length();
	long framesWritten = 0;

	if(str.empty() || str.length() <= 1)
	{
		done = true;
	}

	octave = 4;						// default octave is 4
	noteLength = baseLength * 2;	// set default to 8th notes

	while(!done)
	{
		ch = str.at(i);
		// cout << "Read = " << ch << " at " << i << endl;

		// if the next token is a note
		if(ch=='C'||ch=='D'||ch=='E'||ch=='F'||ch=='G'||ch=='A'||ch=='B')
		{
			int toneNum = 0;
			int j = 0;
			int noteLengthToAssign = noteLength;
			string search = "C D EF G A B";
			while(j<12)
			{
				if(ch == search.at(j))
					toneNum = j + octave * 12;
				j++;
			}

			// advance index
			i++;

			// peak into next char

			// bool peakDone = false;

			if(str.at(i)=='#') // sharp
			{
				toneNum++;
				i++;
			}
			else if(str.at(i)=='b') // flat
			{
				toneNum--;
				i++;
			}
			
			if(str.at(i)=='~') // tie to another note unit
			{
				noteLengthToAssign += noteLength;
				i++;
				while(str.at(i)=='~')
				{
					noteLengthToAssign += noteLength;
					i++;
				}
			}

			// get the frequency

			// push to mData now (frequency and length of this note)
			double freqToWrite = getFrequency(toneNum);
			output->freqNote.push_back(freqToWrite);
			output->len.push_back(noteLengthToAssign);
			output->param.push_back(0);
			output->totalFrames += noteLengthToAssign;
			framesWritten += noteLengthToAssign;

			// DEBUG
			// cout << ch << " tone=" << toneNum << " octave=" << octave << " length=" << noteLengthToAssign << endl;

		}

		else if(ch=='<') // octave down
		{
			octave--;
			if(octave < 0)
				octave = 0;
			i++;
		}

		else if(ch=='>') // octave up
		{
			octave++;
			if(octave > 9)
				octave = 9;
			i++;
		}

		else if(ch=='L') // change note length
		{
			i++;

			if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set note length
			{
				int numberRead = str.at(i) - '0';
				i++;
				if(str.at(i)>='0' && str.at(i)<='9') // if 2nd digit exists
				{
					numberRead = numberRead * 10 + (str.at(i) - '0');
					i++;
						while(str.at(i)>='0' && str.at(i)<='9') // 3rd digits and after - ignore
							i++;
				}

				// now set the new note length
				noteLength = measureLength / numberRead;

				// cout << "noteLength is now = " << noteLength << endl;
			}
		}

		else if(ch=='O') // change octave
		{
			i++;

			if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set octave
			{
				int numberRead = static_cast<int>(str.at(i) - '0');
				i++;

				octave = numberRead;
				// cout << "octave is now = " << octave << endl;
			}

		}

		else if(ch=='[') // tuplets
		{
			i++;

			bool tupletReadDone = false;
			int notes[20] = {0};
			int tie[20] = {0};
			int nNotes = 0;
			int nTied = 0;
			int tupletIndex = 0;
			int wholeLength = noteLength; // default length to set for now

			while(!tupletReadDone)
			{
				// cout << "read: " << i << " ";

				if( str.at(i)=='@' ) // safeguard for infinite loop
					tupletReadDone = true;

				if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set length for whole
				{
					int numberRead = str.at(i) - '0';
					i++;
					if(str.at(i)>='0' && str.at(i)<='9') // if 2nd digit exists
					{
						numberRead = numberRead * 10 + (str.at(i) - '0');
						i++;
							while(str.at(i)>='0' && str.at(i)<='9') // 3rd digits and after - ignore
								i++;
					}

					// now set the length for the whole
					wholeLength = measureLength / numberRead;
				}

				else if(str.at(i)>='A' && str.at(i)<='G') // now we have a note
				{
					int toneNum = 0;
					ch=str.at(i);

					// get the tone number
					string search = "C D EF G A B";
					int k=0;
					while(k<12)
					{
						if(ch == search.at(k))
							toneNum = k + (octave * 12);
						k++;
					}

					// advance index...
					i++;

					if(str.at(i)=='#') // sharp
					{
						toneNum++;
						i++;
					}
					else if(str.at(i)=='b') // flat
					{
						toneNum--;
						i++;
					}

					notes[tupletIndex] = toneNum;

					nNotes++;
					tupletIndex++;
				}
				
				else if(str.at(i)==':') // we have a rest...
				{
					i++; // advance index...
					notes[tupletIndex] = 65535; // freq 65535 for rest
					nNotes++;
					tupletIndex++;
				}

				else if(str.at(i)=='~') // tie last note
				{
					tie[tupletIndex] = 1;
					nTied++;
					i++;
				}

				else if(str.at(i)=='<') // oct down
				{
					octave--;
					i++;
				}

				else if(str.at(i)=='>') // oct up
				{
					octave++;
					i++;
				}

				else if (str.at(i) == ']') // closing brace - finalize tupletDone
				{
					if( (nNotes + nTied) > 0) // if we have empty braces - skip altogether! (avoid div by 0)
					{
						int division = nNotes + nTied;
						int eachTupletLength = wholeLength / division;
						int remainder = wholeLength % division;
	
						/*
						cout << "TUPLETS" << endl << "wholeLength=" << wholeLength << " division=" << division << endl;
						cout << " nNotes=" << nNotes << " nTied =" << nTied << endl;
						for(int itr=0; itr<nNotes; itr++)
							cout << " note=" << notes[itr] << " tie=" << tie[itr] << " / ";
						cout << endl;
						*/
	
						// push tuplet data to mData
						for(int j=0; j<nNotes; j++)
						{
							// get
							int lengthToWrite = eachTupletLength;
							lengthToWrite += tie[j] * eachTupletLength;
							if(j==0)
								lengthToWrite += remainder;
							// cout << "Writing length = " << lengthToWrite << endl;
							
							// get frequency of the note...
							double freqToWrite;
							if(notes[j]==65535) // then we have a rest
								freqToWrite = notes[j]; // use 65535 as freq, to signify a rest
							else
								freqToWrite = getFrequency(notes[j]);

							// cout << "Writing freq = " << freqToWrite << endl;
	
							// push this note data to mData object
							output->freqNote.push_back(freqToWrite);
							output->len.push_back(lengthToWrite);
							output->param.push_back(0);
							output->totalFrames += lengthToWrite;
							framesWritten += lengthToWrite;
						}
					}

					i++;
					tupletReadDone = true;
				}

				else // something else - advance index anyway
				{
					i++;
				}

			}

		}

		else if(str.at(i)==':') // rest, ':' colon
		{
			int lengthToWrite = noteLength;
			double freqToWrite = 65535;

			// cout << "Rest, length = " << lengthToWrite << endl;

			// push this note data to mData object
			output->freqNote.push_back(freqToWrite);
			output->len.push_back(lengthToWrite);
			output->param.push_back(0);
			output->totalFrames += lengthToWrite;
			framesWritten += lengthToWrite;

			i++;
		}

		else if(str.at(i)=='V') // Volume change request
		{
			// read the next 2 characters
			string strValue = str.substr(i+1,2); // get 3 digits following 'V'
			// int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(10, max(1, value)); // floor + ceil the value

			// push this 'event' data to mData object
			output->freqNote.push_back(70000); // event# 70000 is 'specify volume'
			output->len.push_back(0);
			output->param.push_back(value);	 // 10-scaled value is passed as param
			i++;
		}

		else if(str.at(i)=='^') // Volume increment request
		{
			// push this 'event' data to mData object
			output->freqNote.push_back(71000); // event# 71000 is 'increment volume'
			output->len.push_back(0);
			output->param.push_back(0);	 // no param needed
			i++;
		}

		else if(str.at(i)=='_') // Volume decrement request
		{
			// push this 'event' data to mData object
			output->freqNote.push_back(72000); // event# 72000 is 'decrement volume'
			output->len.push_back(0);
			output->param.push_back(0);	 // no param needed
			i++;
		}
		
		// '%%' is for bookmarking
		else if(str.at(i)=='%')
		{
			i++;
			if(str.at(i)=='%')
			{
				// if requested place (totalFrames at current parsing position)
				// is later than already bookmarked place, set it as new bookmark
				if(framesWritten > player->getBookmark())
				{
					player->setBookmark(framesWritten);
					
					cout << "Bookmarked! at ... " << player->getBookmark() << endl;
				}
				i++;
			}
		}

		else if(ch=='$') // end of string
		{
			// insert data to signify end
			output->freqNote.push_back(-1.0);
			output->len.push_back(-1);
			output->param.push_back(0);

			// integrity check

			// int r = framesWritten % quarterNoteLength;

			// cout << "End of parsing a channel... num of framesWritten=" << framesWritten << endl;
			// cout << "Dividing by quarter note length, remainder=" << r << endl << endl;

			// record the total frame length for this channel
			// output->totalFrames = framesWritten;

			done = true;
		}
		else	// default case - move pointer anyway
		{
			i++;
		}

		// DEBUG
		// cout << endl;

	}

	return result;
}

// function to parse an MML string for a drum channel
// will fill the DData object with the data that is read
string MML::parseDrumSource(MPlayer* player)
{
	dOutput = &player->ddata; // gets pointer to MData object

	// channel source string to work on
	string str = dsource;

	// first, parse the repeat signs
	// (any repeated parts will be duplicated)

	str = str + "$$$$$$"; // to signal end of string
	bool done = false;

	int i = 0;
	vector<int> leftBraces;
	vector<int> repeatTimes;
	char ch = ' ';
	int leftBracePos = 0;
	string strToCopy = "";
	int nCharsToCopy = 0;

	while(!done)
	{
		ch = str.at(i);

		if(ch=='{') // if left brace is found
		{
			leftBraces.push_back(i); // push this position into stack
			str.erase(i,1); // go ahead and erase this '{'
			
			char chNext = str.at(i); // this should be the char right after '{'
			int numberRead = 0;
			
			// check if a number is followed...
			if(chNext >= '0' && chNext <= '9')
			{
				numberRead = chNext - '0'; // set the num of times to duplicate at right brace
				str.erase(i,1); // go ahead and erase this digit
				if(numberRead==0) numberRead = 1;
				
				// make sure there aren't any more digits after this
				while( (str.at(i)>='0'&&str.at(i)<='9') )
					str.erase(i,1); // erase this digit
			}
			else
				numberRead = 2; // repeat times not specified -> set to twice
			
			// push this number... it'll be popped when right brace is found
			repeatTimes.push_back(numberRead);
		}
		else if (ch=='}') // right brace to close repeat
		{
			str.erase(i,1); // go ahead and erase this '}'
			if(leftBraces.size() > 0) // if stack is empty, ignore
			{
				// pop last element from stack - gets the nearest pos of '{'
				leftBracePos = leftBraces.back();
				leftBraces.pop_back();
				
				// and get the number of times we should duplicate
				int timesToDuplicate = repeatTimes.back();
				repeatTimes.pop_back();
				timesToDuplicate -= 1; // if repeat is x3, duplicate twice :)

				nCharsToCopy = i - leftBracePos; // n of chars to duplicate
				strToCopy = str.substr(leftBracePos, nCharsToCopy); // str to be duplicated
				
				if(timesToDuplicate >=1 && timesToDuplicate <=8)
				{
					for(int i=0; i<timesToDuplicate; i++)
						str.insert(leftBracePos+nCharsToCopy, strToCopy);
				}
			}
		}
		else if (ch=='$') // end of parse string
		{
			done = true;
		}
		else
		{
			i++; // irrelevant char, so go to next char
		}
	}

	// DEBUG
	// cout << "took out all repeats. str is now: \n" << str << "\n\n";
	// while(!GetAsyncKeyState(VK_SPACE)){}
	// while(GetAsyncKeyState(VK_SPACE)){}

	done = false;
	string result = "";
	ch = ' ';
	i = 0;
	// int len = str.length();
	long framesWritten = 0;

	while(!done)
	{
		ch = str.at(i);
		// cout << "Read = " << ch << " at " << i << endl;

		// if the next token is a drum note
		if(ch=='K'||ch=='S'||ch=='H'||ch=='k'||ch=='s'||ch=='h')
		{
			int drumNote = 0;
			int noteLengthToAssign = noteLength;
			string search = "KSHksh";
			size_t found = search.find(ch);
			if(found != string::npos)
				drumNote = found;

			// advance index
			i++;

			// peak into next char

			// bool peakDone = false;

			if(str.at(i)=='~') // tie to another note unit
			{
				noteLengthToAssign += noteLength;
				i++;
				while(str.at(i)=='~')
				{
					noteLengthToAssign += noteLength;
					i++;
				}
			}

			// push to mData now (frequency and length of this note)
			dOutput->drumNote.push_back(drumNote);
			dOutput->len.push_back(noteLengthToAssign);
			dOutput->param.push_back(0);
			dOutput->totalFrames += noteLengthToAssign;
			framesWritten += noteLengthToAssign;

			// DEBUG
			// cout << ch << " tone=" << toneNum << " octave=" << octave << " length=" << noteLengthToAssign << endl;

		}

		else if(ch=='L') // change note length
		{
			i++;

			if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set note length
			{
				int numberRead = str.at(i) - '0';
				i++;
				if(str.at(i)>='0' && str.at(i)<='9') // if 2nd digit exists
				{
					numberRead = numberRead * 10 + (str.at(i) - '0');
					i++;
						while(str.at(i)>='0' && str.at(i)<='9') // 3rd digits and after - ignore
							i++;
				}

				// now set the new note length
				noteLength = measureLength / numberRead;

				// cout << "noteLength is now = " << noteLength << endl;
			}
		}

		else if(ch=='[') // tuplets
		{
			i++;

			bool tupletReadDone = false;
			int notes[20] = {0};
			int tie[20] = {0};
			int nNotes = 0;
			int nTied = 0;
			int tupletIndex = 0;
			int wholeLength = noteLength; // default length to set for now

			while(!tupletReadDone)
			{
				// cout << "read: " << i << " ";

				ch = str.at(i);

				if( str.at(i)=='$' ) // safeguard for infinite loop
					tupletReadDone = true;

				if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set length for whole
				{
					int numberRead = str.at(i) - '0';
					i++;
					if(str.at(i)>='0' && str.at(i)<='9') // if 2nd digit exists
					{
						numberRead = numberRead * 10 + (str.at(i) - '0');
						i++;
							while(str.at(i)>='0' && str.at(i)<='9') // 3rd digits and after - ignore
								i++;
					}

					// now set the length for the whole
					wholeLength = measureLength / numberRead;
				}

				else if(ch=='K'||ch=='S'||ch=='H'||ch=='k'||ch=='s'||ch=='h') // now we have a note
				{
					int drumNote = 0;
					ch=str.at(i);

					// get the tone number
					string search = "KSHksh";
					size_t found = search.find(ch);
					if(found != string::npos)
						drumNote = found;

					// advance index...
					i++;

					notes[tupletIndex] = drumNote;

					nNotes++;
					tupletIndex++;
				}

				else if(str.at(i)=='~') // tie last note
				{
					tie[tupletIndex] = 1;
					nTied++;
					i++;
				}
				
				else if(str.at(i)==':') // we have a rest...
				{
					i++; // advance index...
					notes[tupletIndex] = 65535; // freq 65535 for rest
					nNotes++;
					tupletIndex++;
				}

				else if (str.at(i) == ']') // closing brace - finalize tupletDone
				{
					if( (nNotes + nTied) > 0) // if we have a empty set of braces - skip altogether!
					{
						int division = nNotes + nTied;
						int eachTupletLength = wholeLength / division;
						int remainder = wholeLength % division;
	
						/*
						cout << "TUPLETS" << endl << "wholeLength=" << wholeLength << " division=" << division << endl;
						cout << " nNotes=" << nNotes << " nTied =" << nTied << endl;
						for(int itr=0; itr<nNotes; itr++)
							cout << " note=" << notes[itr] << " tie=" << tie[itr] << " / ";
						cout << endl;
						*/
	
						// push tuplet data to dData
						for(int j=0; j<nNotes; j++)
						{
							// get
							int lengthToWrite = eachTupletLength;
							lengthToWrite += tie[j] * eachTupletLength;
							if(j==0)
								lengthToWrite += remainder;
							// cout << "Writing length = " << lengthToWrite << endl;
	
							// get frequency of the note
							int noteToWrite = notes[j];
							// cout << "Writing freq = " << notesToWrite << endl;
	
							// push this note data to mData object
							dOutput->drumNote.push_back(noteToWrite);
							dOutput->len.push_back(lengthToWrite);
							dOutput->param.push_back(0);
							dOutput->totalFrames += lengthToWrite;
							framesWritten += lengthToWrite;
						}
					}

					i++;
					tupletReadDone = true;
				}

				else // something else - advance index anyway
				{
					i++;
				}

			}

		}

		else if(str.at(i)==':') // rest, ':' colon
		{
			int lengthToWrite = noteLength;
			int noteToWrite = 65535;

			// cout << "Rest, length = " << lengthToWrite << endl;

			// push this note data to mData object
			dOutput->drumNote.push_back(noteToWrite);
			dOutput->len.push_back(lengthToWrite);
			dOutput->param.push_back(0);
			dOutput->totalFrames += lengthToWrite;
			framesWritten += lengthToWrite;

			i++;
		}

		else if(str.at(i)=='V') // Volume change request
		{
			// read the next 2 characters
			string strValue = str.substr(i+1,2); // get 3 digits following 'V'
			// int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(10, max(1, value)); // floor + ceil the value

			// push this 'event' data to mData object
			dOutput->drumNote.push_back(70000); // event# 70000 is 'specify volume'
			dOutput->len.push_back(0);
			dOutput->param.push_back(value);	 // 10-scaled value is passed as param
			i++;
		}

		else if(str.at(i)=='^') // Volume increment request
		{
			// push this 'event' data to mData object
			dOutput->drumNote.push_back(71000); // event# 71000 is 'increment volume'
			dOutput->len.push_back(0);
			dOutput->param.push_back(0);	 // no param needed
			i++;
		}

		else if(str.at(i)=='_') // Volume decrement request
		{
			// push this 'event' data to mData object
			dOutput->drumNote.push_back(72000); // event# 72000 is 'decrement volume'
			dOutput->len.push_back(0);
			dOutput->param.push_back(0);	 // no param needed
			i++;
		}

		// '%%' is for bookmarking
		else if(str.at(i)=='%')
		{
			i++;
			if(str.at(i)=='%')
			{				
				// if requested place (totalFrames at current parsing position)
				// is later than already bookmarked place, set it as new bookmark
				if(framesWritten > player->getBookmark())
				{
					player->setBookmark(framesWritten);
					
					cout << "Dr channel - Bookmarked! at ... " << player->getBookmark() << endl;
					// while(!GetAsyncKeyState(VK_SPACE)){} // DEBUG
				}
				i++;
			}
		}

		else if(ch=='$') // end of string
		{
			// insert data to signify end
			dOutput->drumNote.push_back(-1);
			dOutput->len.push_back(0);
			dOutput->param.push_back(0);

			// integrity check

			// int r = framesWritten % quarterNoteLength;

			// cout << "End of parsing a channel... num of framesWritten=" << framesWritten << endl;
			// cout << "Dividing by quarter note length, remainder=" << r << endl << endl;

			// write the total frame length written
			// output->totalFrames = framesWritten;

			done = true;
		}
		else	// default case - move pointer anyway
		{
			i++;
		}

		// DEBUG
		// cout << endl;

	}

	return result;
}

void MML::parseGlobalSource(MPlayer* player)
{
	bool done = false;
	int i=0;
	char ch, ch2;
	string strValue;
	int value;
	float gain[9] = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};

	// default values
	double tpo = 120.0;
	float gainD = 0.5f;

	string str = gsource + "    $$$$";
	size_t fpos;

	// DEBUG
	// cout << "Now let's parse global source:\n\n" << str << "\n\n";
	// while(!GetAsyncKeyState(VK_SPACE)){}
	// while(GetAsyncKeyState(VK_SPACE)){}

	// search for following items
	//		DELAY, DELAYTIME, LOOP, TEMPO

	while(!done)
	{
		done = false;

		if(str.find("TEMPO=") != string::npos)
		{
			fpos = str.find("TEMPO=");
			string strValue = str.substr(fpos+6,3); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(400, max(40, value)); // floor + ceil the value

			// set tempo to the value that was read
			tempo = static_cast<double>(value);
			tpo = tempo;
			str.erase(fpos, 6+valueDigits);

		}
		else if(str.find("LOOP=ON") != string::npos)
		{
			player->loopEnabled = true; // enable loop
			str.erase(str.find("LOOP=ON"), 7);
		}
		else if(str.find("LOOP=OFF") != string::npos)
		{
			player->loopEnabled = false; // disable loop
			str.erase(str.find("LOOP=OFF"), 8);
		}
		else if(str.find("DELAY=ON") != string::npos)
		{
			player->delayEnabled = true; // turn delay on
			str.erase(str.find("DELAY=ON"), 8);
		}
		else if(str.find("DELAY=OFF") != string::npos)
		{
			player->delayEnabled = false;// turn delay off
			str.erase(str.find("DELAY=OFF"), 9);
		}
		else if(str.find("DELAYTIME=") != string::npos)
		{
			fpos = str.find("DELAYTIME=");
			string strValue = str.substr(fpos+10,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(999, max(10, value)); // floor + ceil the value

			// set delay parameters - first delay, delay time, gain (negative for no change)
			player->delay[0].setParameters(value, value, -0.1f); // -> LEFT channel = 0
			player->delay[1].setParameters(value*3/2, value, -0.1f); // -> RIGHT channel = 1
			str.erase(fpos, 10+valueDigits);
		}
		else if(str.find("DELAYLEVEL=") != string::npos)
		{
			fpos = str.find("DELAYLEVEL=");
			string strValue = str.substr(fpos+11,3); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(99, max(1, value)); // floor + ceil the value
			float valuef = static_cast<float>(value) / 100.0f;

			// set delay parameters - first delay, delay time, gain (negative for no change)
			player->delay[0].setParameters(-1, -1, valuef); // -> LEFT channel = 0
			player->delay[1].setParameters(-1, -1, valuef); // -> RIGHT channel = 1
			str.erase(fpos, 11+valueDigits);
		}
		else if(str.find("MASTERVOLUME=") != string::npos)
		{
			fpos = str.find("MASTERVOLUME=");
			string strValue = str.substr(fpos+13,3); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(99, max(1, value)); // floor + ceil the value
			float valuef = static_cast<float>(value) / 100.0f;

			// set master gain
			player->setMasterGain(valuef);
			str.erase(fpos, 13+valueDigits);
		}
		// if none of above can be found anymore - finally done!
		else
			done = true;

		// DEBUG
		// cout << "Now our global source is..:\n\n" << str << "\n\n";
		// while(!GetAsyncKeyState(VK_SPACE)){}
		// while(GetAsyncKeyState(VK_SPACE)){}

	}

	done = false;

	// check for older-style declarations...
	while(!done)
	{
		ch = str.at(i);
		if(ch=='T') // tempo definition found
		{
			i++;
			ch = str.at(i);
			if(ch=='=')
			{
				strValue = str.substr(i+1, 3); // read tempo value
				tpo = static_cast<double>(atoi(strValue.c_str()));
				tpo = min(320.0, max(40.0, tpo));
			}
		}
		else if(ch=='V') // volume definition found
		{
			i++;
			ch = str.at(i);
			ch2 = str.at(i+1);

			if(ch>='1' && ch<='9' && ch2=='=')
			{
				int targetChannel = ch - '1';
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value)); // takes a value from 0 to 10
				gain[targetChannel] =
					static_cast<float>(value) / 20.0f; // convert to scale 0 to 0.5f
			}

			/*
			if(ch=='1' && ch2=='=') // channel 1
			{
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value));
				gain1 = (float) value / 20.0f;
			}
			else if(ch=='2' && ch2=='=') // channel 2
			{
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value));
				gain2 = (float) value / 20.0f;
			}
			else if(ch=='3' && ch2=='=') // channel 3
			{
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value));
				gain3 = (float) value / 20.0f;
			}
			*/

			else if( (ch=='d' || ch=='D') && ch2=='=') // drum channel
			{
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value)); // takes a value from 0 to 10
				gainD = static_cast<float>(value) / 20.0f; // scale 0 to 0.5f
			}
		}
		else if(ch=='$') // end signal found
		{
			done = true;
		}
		else // all else - just advance index
		{
			i++;
		}
	}

	// set player according to values that were read
	player->setAllChannelGain(gain[0], gain[1], gain[2], gain[3], gain[4], gain[5],
		gain[6], gain[7], gain[8], gainD);
	tempo = tpo;
	calculateTiming(); // recalculate base note lengths
}

// this function will load a MML file and then parse
// returns the loaded string
string MML::loadFile(string filename, MPlayer* player)
{
	// try to open file
	ifstream inFile;
	inFile.open(filename.c_str(), ifstream::in);

	// if read error - return false
	if(!inFile)
	{
		errLog("Error loading file: ", filename);
		return "Error";
	}

	string fileContent = "";
	char ch = inFile.get();
	fileContent += ch;

	while(inFile.good())
	{
		ch = inFile.get();
		fileContent += ch;
	}
	inFile.close();

	// check if the text has EOF char = 255
	// if not, add it to end
	char chEOF = 255;
	size_t found = fileContent.find(chEOF);
	string strEOF = "\xFF";
	if(found == string::npos)
		fileContent += strEOF;
		// fileContent.append<char>(1,0xFF);

	// reset source MML
	setSource(fileContent);

	// clear all MData
	for(int i=0; i<9; i++)
		{ player->data[i].clear(); }

	// parse the new source MML
	parse(player);

	return fileContent;
}

// this function will save the original MML source stored to a file
string MML::saveFile(string filename, MPlayer* player)
{
	string result;
	string strToWrite = originalSource;

	// erase ALL EOF chars before saving...
	char chEOF = 255;
	size_t found = strToWrite.find(chEOF);
	while(found!=string::npos) // if chEOF found..
	{
		strToWrite.erase(found,1); // erase that chEOF
		found = strToWrite.find(chEOF);
	}
	
	// try to open file
	ofstream outFile(filename.c_str());
	if (outFile.is_open())
	{
		outFile << strToWrite;
		outFile.close();
		result = "File written successfully.";
	}
	else
	{
		cout << "Unable to open file";
		errLog("Error opening file: ", filename);
		result = "Error";
	}

	return result;
}

// helper function to count the number of consecutive digits in the string
int MML::countDigits(string snippet)
{
	bool done = false;
	int pos = 0;
	int count = 0;
	snippet += "$$$";
	while(!done)
	{
		if(snippet.at(pos) >= '0' && snippet.at(pos) <= '9')
			count++;
		// else if(snippet.at(pos) == '.') // maybe we don't need decimals at all!
			// count++;
		else if(snippet.at(pos)=='$')
			done = true;
		else
			done = true;

		pos++;
	}
	return count;
}

// prints an error message to a log file
void MML::errLog(std::string errText1, std::string errText2)
{
	std::ofstream ofs;
	ofs.open("_errors_mml.txt", std::ofstream::out | std::ofstream::app);
	ofs << errText1 << errText2 << endl;
	ofs.close();
}




// MPlayer.cpp ////////////////////////////////////////
// MPlayer Class - Implementation /////////////////////

// include dependencies

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <cstdio>
//#?include <windows.h> // DEBUG

/*----------

#include <sndfile.hh>
#include <lame/lame.h>

----------*/

#include "BC/MPlayer.h"

const int MPlayer::SAMPLE_RATE = 44100;
const int MPlayer::FRAMES_PER_BUFFER = 256;

using namespace std;

// real port audio callback function
// (redirected from 'paCallback')

int MPlayer::playerCallback (
		const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags )
{
	float* out = static_cast<float*>(outputBuffer);
	// static_cast<int>(framesPerBuffer); // since this has no effect..
	static_cast<void>(timeInfo);
	static_cast<void>(statusFlags);
	static_cast<void>(inputBuffer);

	float soundAmplitudeLeft;
	float soundAmplitudeRight;

	for(unsigned long i=0; i<framesPerBuffer; i++)
	{
		if(!playing) // if player is not playing or finished playing, just pass 0
		{
			soundAmplitudeLeft = 0.0f;
			soundAmplitudeRight = 0.0f;
		}
		// player IS playing... get mix - output - advance frame - check for end of channel
		else
		{
			// get mix of all 9 channels (plus drums) at current position
			soundAmplitudeLeft = getMix(0); // get mix for LEFT channel
			soundAmplitudeRight = getMix(1); // get mix for RIGHT channel

			// if reached end of note, go to next index (for regular channels ch0 - 2)
			for(int i=0; i<9; i++)
			{
				if(!channelDone[i])
				{
					remainingFrames[i]--;
					if(remainingFrames[i] <= 0)
					{
						noteIndex[i]++;

						// if there are event requests (freq >= 70000), digest those first
						bool eventsDone = false;

						while(!eventsDone)
						{
							int readValue = static_cast<int>(data[i].freqNote[noteIndex[i]]);
							if(readValue==70000.0) // 'specify volume'
							{
								// convert the passed value(1-10) to float (0.0 to 0.5f)
								float gainToSet
									= static_cast<float>(data[i].param[noteIndex[i]]) / 20.0f;
								setChannelGain(i, gainToSet);
								noteIndex[i]++;
							}
							else if(readValue==71000.0) // 'increment volume'
							{
								float gainToSet = min(0.5f, getChannelGain(i)+0.05f);
								setChannelGain(i, gainToSet);
								noteIndex[i]++;
							}
							else if(readValue==72000.0) // 'decrement volume'
							{
								float gainToSet = max(0.001f, getChannelGain(i)-0.05f);
								setChannelGain(i, gainToSet);
								noteIndex[i]++;
							}
							else // next freq value is not any event request, so we're done here
								eventsDone = true;
						}

						// and if you get to the end of MML signal (freq = -1.0), set flag
						if(data[i].freqNote[noteIndex[i]] < 0)
						{
							channelDone[i] = true;
							setToRest(i); // set to rest.. and let delay finish
							// disableChannel(i); // disable this channel
						}
						else
						{
							remainingFrames[i] = data[i].len[noteIndex[i]];
							freqNote[i] = data[i].freqNote[noteIndex[i]];

							// if this is a rest (freq = 65535), set this channel to rest
							if(freqNote[i]==65535.0)
								setToRest(i);
							// otherwise, this is a valid note - so set this note
							else
								setNewNote(i, freqNote[i]);
						}
					}
				}
			}

			// now handle drum channel!
			if(!dChannelDone)
			{
				dRemainingFrames--;
				if(dRemainingFrames <= 0)
				{
					dNoteIndex++; // move onto the next drum note index

						// if there are event requests (freq >= 70000), digest those first
						bool eventsDone = false;

						while(!eventsDone)
						{
							int readValue = ddata.drumNote[dNoteIndex];
							if(readValue==70000) // 'specify volume'
							{
								// convert the passed value(1-10) to float (0.0 to 0.5f)
								float gainToSet
									= static_cast<float>(ddata.param[dNoteIndex]) / 20.0f;
								setDChannelGain(gainToSet);
								dNoteIndex++;
							}
							else if(readValue==71000) // 'increment volume'
							{
								float gainToSet = min(0.5f, getDChannelGain()+0.05f);
								setDChannelGain(gainToSet);
								dNoteIndex++;
							}
							else if(readValue==72000) // 'decrement volume'
							{
								float gainToSet = max(0.001f, getDChannelGain()-0.05f);
								setDChannelGain(gainToSet);
								dNoteIndex++;
							}
							else // next freq value is not any event request, so we're done here
								eventsDone = true;
						}

					// and if you get to the end of MML signal (drumNote = -1.0), set flag
					if(ddata.drumNote[dNoteIndex] < 0 || dNoteIndex >= ddata.getSize())
					{
						dChannelDone = true;
						restDrum(); // rest.. and let delay effect finish off
						// disableDrumChannel(); // disable this channel
					}
					else // not at end yet.. set new drum hit
					{
						dRemainingFrames = ddata.len[dNoteIndex];
						currentDrumNote = ddata.drumNote[dNoteIndex];
						setNewDrumHit(currentDrumNote);

						// if this is a rest (freq = 65535), set flag
						if(currentDrumNote == 65535)
						{
							restDrum();
						}
					}
				}
			}

			// if all channels have reached end... and loop is enabled, go back to beginning
			if(	channelDone[0] && channelDone[1] && channelDone[2] &&
				channelDone[3] && channelDone[4] && channelDone[5] &&
				channelDone[6] && channelDone[7] && channelDone[8] && dChannelDone)
			{
				if(loopEnabled)
				{
					// enable channels again
					enableChannels(true, true, true, true, true, true, true, true, true, true);

					// enable drum channel
					enableDrumChannel();

					// go back to the beginning
					goToBeginning();
				}
			}

			// if song is not finished, update frame position - advance player
			if(!songFinished)
			{
				// update player position
				framePos++;
				advance();

				// if you have reached the absolute last frame position of the song
				// (including last delay effects) - only then end the track officially
				if(framePos >= songLastFrame)
				{
					songFinished = true;
					playing = false;
					
					/*----------
					
					// DEBUG
					cout << "song officially finished!\n";
					cout << "framePos = " << framePos << endl;
					cout << "getSongLastFrame() = " << getSongLastFrame() << endl;

					for(int i=0; i<9; i++)
						cout << "channel " << i << " length = " << data[i].totalFrames << "\n";
					cout << "d channel length = " << ddata.totalFrames << "\n";
					
					----------*/
				}
			}


		}

		//
		//	output to port audio buffer - variable 'sampleAmplitude' (float value for one frame)
		//

		*out = soundAmplitudeLeft; // write LEFT channel mix to buffer
		out++; // move buffer pointer
		*out = soundAmplitudeRight; // write RIGHT channel mix to buffer
		out++; // move buffer pointer
	}

	return paContinue;
}

////////////////////////////////////////////////////////

MPlayer::MPlayer()
{
	for(int i=0;i<9;i++)
		silenced[i] = false;

	dSilenced = false;
	enableChannels(true, true, true, true, true, true, true, true, true, true);
	enableDrumChannel();

	for(int i=0;i<9;i++)
		osc[0].detune = 0;

	tableType = 1; // OSC's default type - square table
	masterGain = 0.8f;
	masterOutCap = 0.85f;

	compThreshold = 0.5f;
	compRatio = 8;

	framePos = 0;
	songLastFrame = 0;
	songLastFramePure = 0;
	bookmark = 0;

	// call this function once to set various parameters settings to default
	resetForNewSong();
}

MPlayer::~MPlayer()
{}

void MPlayer::handlePaError(PaError e)
{
	cout << "PortAudio Error - " << Pa_GetErrorText( e ) << endl;
}

void MPlayer::initialize()
{
	// initialize variables
	framePos = 0;	// the index for the audio data frame
	songLastFrame = 0;
	songLastFramePure = 0;
	playing = false;

	// initialize each channel
	setAllChannelGain(0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);

	//
	// get portaudio ready now...
	//

	// initialize portaudio
    err = Pa_Initialize();
	if( err != paNoError ) handlePaError( err );

	// set up output parameters
    outputParameters.device = Pa_GetDefaultOutputDevice(); // use default device
    if (outputParameters.device == paNoDevice)
	{
      cout << "Error: No default output device." << endl;
    }
    outputParameters.channelCount = 2; // stereo
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

	// open port audio stream
    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              paCallback,	// the name of port audio callback function
              this // pass this class to callback
		);
	if( err != paNoError ) handlePaError( err );

	// start port audiostream
    err = Pa_StartStream( stream );
}

// function to set back to default before loading new song (or 'play' current song again)
// reset all oscillator + delay settings etc. - before parsing source
void MPlayer::resetForNewSong()
{
	// loop is enabled by default
	loopEnabled = true;
	
	// reset frame info for now (in case you're starting a new empty file)
	songLastFrame = 0;
	songLastFramePure = 0;
	songFinished = true;

	// clear delay buffer
	delay[0].clearBuffer(); // left delay
	delay[1].clearBuffer(); // right delay

	// reset delay time + gain parameters
	// ... first delay / delay time (in milliseconds) + gain
	delayEnabled = true;
	delay[0].setParameters(500, 500, 0.20f); // -> LEFT channel = 0
	delay[1].setParameters(750, 500, 0.20f); // -> RIGHT channel = 1

	// enable all channels and reset Envelope / Astro / LFO setting
	for(int i=0; i<9; i++)
	{
		// back to default envelope setting
		osc[i].setEnvelope(22, 18, 250, 40, 0.9f, 0.5f);
		enableChannel(i);
		disableAstro(i);
		osc[i].disableLFO();
		osc[i].detune = 0;
	}
	enableDrumChannel();

	// clear all osc history data used for meter visualization
	for(int i=0; i<9; i++)
		osc[i].clearHistory();
	nosc.clearHistory();
	
	for(int i=0; i<9; i++)
		channelDone[i] = false;
	dChannelDone = false;
}

void MPlayer::close()
{
	// close port audio stream and terminate

	err = Pa_StopStream( stream );
	if( err != paNoError ) handlePaError( err );

	cout << "pa stream stopped...";

	err = Pa_CloseStream( stream );
	if( err != paNoError ) handlePaError( err );

	cout << "pa stream closed...";

    Pa_Terminate();
}

void MPlayer::start()
{
	goToBeginning();

	// set enabler flag
	playing = true;
}

void MPlayer::pause()
	{ playing = false; }

void MPlayer::restart()
	{ playing = true; }

void MPlayer::cleanUpForNewFile()
{
	songLastFrame = 0;
	songLastFramePure = 0;
	songFinished = true;
}
	
void MPlayer::goToBeginning()
{
	// get the total song frame length
	songLastFrame = getSongLastFrame();
	songLastFramePure = getSongLastFramePure();
	songFinished = false;

	// back to very beginning... initialize player variables
	framePos = 0;
	for(int i=0; i<9; i++)
	{
		channelDone[i] = false;
		remainingFrames[i] = 0;
		freqNote[i] = 0;
		noteIndex[i] = 0;
	}

	// for drum channel
	dChannelDone = false;
	dRemainingFrames = 0;
	currentDrumNote = 0;
	dNoteIndex = 0;

	// set the starting note for each music channel (ch 1 to 9)
	for(int i=0; i<9; i++)
	{
		// if there are event requests (freq >= 70000), digest those first
		bool eventsDone = false;

		while(!eventsDone)
		{
			int readValue = static_cast<int>(data[i].freqNote[noteIndex[i]]);
			if(readValue==70000.0) // 'specify volume'
			{
				// convert the passed value(1-10) to float (0.0 to 0.5f)
				float gainToSet
					= static_cast<float>(data[i].param[noteIndex[i]]) / 20.0f;
				setChannelGain(i, gainToSet);
				noteIndex[i]++;
			}
			else if(readValue==71000.0) // 'increment volume'
			{
				float gainToSet = min(0.5f, getChannelGain(i)+0.05f);
				setChannelGain(i, gainToSet);
				noteIndex[i]++;
			}
			else if(readValue==72000.0) // 'decrement volume'
			{
				float gainToSet = max(0.001f, getChannelGain(i)-0.05f);
				setChannelGain(i, gainToSet);
				noteIndex[i]++;
			}
			else // next freq value is not any event request, so we're done here
				eventsDone = true;
		}

		remainingFrames[i] = data[i].len[noteIndex[i]];
		freqNote[i] = data[i].freqNote[noteIndex[i]];
		setNewNote(i, freqNote[i]);

		// if this is a rest (freq = 65535), silence channel
		if(freqNote[i]==65535.0)
		{
			setToRest(i); // set this channel to rest
		}
		else if(freqNote[i]<0) // if first note is already end flag (empty MML data)
		{
			channelDone[i] = true;
			disableChannel(i);
		}
	}

	// handle drum channel!
	//
	// set starting note for drum channel

	// if there are event requests (freq >= 70000), digest those first
	bool eventsDone = false;

	while(!eventsDone)
	{
		int readValue = ddata.drumNote[dNoteIndex];
		if(readValue==70000) // 'specify volume'
		{
			// convert the passed value(1-10) to float (0.0 to 0.5f)
			float gainToSet
				= static_cast<float>(ddata.param[dNoteIndex]) / 20.0f;
			setDChannelGain(gainToSet);
			dNoteIndex++;
		}
		else if(readValue==71000) // 'increment volume'
		{
			float gainToSet = min(0.5f, getDChannelGain()+0.05f);
			setDChannelGain(gainToSet);
			dNoteIndex++;
		}
		else if(readValue==72000) // 'decrement volume'
		{
			float gainToSet = max(0.001f, getDChannelGain()-0.05f);
			setDChannelGain(gainToSet);
			dNoteIndex++;
		}
		else // next freq value is not any event request, so we're done here
			eventsDone = true;
	}

	dRemainingFrames = ddata.len[dNoteIndex];
	currentDrumNote = ddata.drumNote[dNoteIndex];
	activateDrumChannel();
	setNewDrumHit(currentDrumNote);

	// if this is a rest (note = 65535), rest drum channel
	if(currentDrumNote == 65535)
	{
		restDrum();
	}
	else if(currentDrumNote < 0) // if first note is already end flag (empty MML data)
	{
		dChannelDone = true;
		disableDrumChannel();
	}
}

// calculates the song's frame length - including time for last delay effects
long MPlayer::getSongLastFrame()
{
	// find the max length of all MData and DData entities
	long maxLen = 0;
	for(int i=0; i<9; i++)
	{
		if(data[i].totalFrames > maxLen)
			maxLen = data[i].totalFrames;
	}
	if(ddata.totalFrames > maxLen)
		maxLen = ddata.totalFrames;

	// add time for delay effect to complete
	int totalDelayTime = max( delay[0].totalDelayFrames, delay[1].totalDelayFrames );

	return maxLen + totalDelayTime + 1000; // add safeguarding frames :)
}

// calculates the song's frame length - including time for last delay effects
// EXCLUDING up to 2-second delay cushion at the end
long MPlayer::getSongLastFramePure()
{
	// find the max length of all MData and DData entities
	long maxLen = 0;
	for(int i=0; i<9; i++)
	{
		if(data[i].totalFrames > maxLen)
			maxLen = data[i].totalFrames;
	}
	if(ddata.totalFrames > maxLen)
		maxLen = ddata.totalFrames;

	return maxLen;
}

// checks if player is currently playing
bool MPlayer::isPlaying()
	{ return playing; }

// switches all oscillators' table type
void MPlayer::setTableType(int type)
{
	tableType = type;
	for(int i=0; i<9; i++)
		osc[i].setTable(type);
}

void MPlayer::setAstro(int channel, int nCyclesPerSecond)
{
	osc[channel].enableAstro();
	osc[channel].setAstroSpeed(nCyclesPerSecond);
}

void MPlayer::disableAstro(int channel)
	{ osc[channel].disableAstro(); }

void MPlayer::setNewNote(int channel, double freq)
{
	osc[channel].setNewNote(freq);
}

void MPlayer::setToRest(int channel)
{
	osc[channel].setToRest();
}

void MPlayer::setNewDrumHit(int drumType)
{
	nosc.setNewDrum(drumType);
}

void MPlayer::restDrum()
{
	nosc.rest();
}

void MPlayer::setAllChannelGain(float gain1, float gain2, float gain3, float gain4,
	float gain5, float gain6, float gain7, float gain8, float gain9, float gainD)
{
	osc[0].setGain(gain1); osc[1].setGain(gain2); osc[2].setGain(gain3);
	osc[3].setGain(gain4); osc[4].setGain(gain5); osc[5].setGain(gain6);
	osc[6].setGain(gain7); osc[7].setGain(gain8); osc[8].setGain(gain9);
	nosc.setGain(gainD);
}

void MPlayer::setChannelGain(int channel, float gain)
	{ osc[channel].setGain(gain); }

float MPlayer::getChannelGain(int channel)
	{ return osc[channel].getGain(); }

void MPlayer::setDChannelGain(float gain)
	{ nosc.setGain(gain); }

float MPlayer::getDChannelGain()
	{ return nosc.getGain(); }

void MPlayer::enableChannels(bool enable1, bool enable2, bool enable3, bool enable4,
	bool enable5, bool enable6, bool enable7, bool enable8, bool enable9, bool enableD)
{
	enabled[0] = enable1; enabled[1] = enable2; enabled[2] = enable3;
	enabled[3] = enable4; enabled[4] = enable5; enabled[5] = enable6;
	enabled[6] = enable7; enabled[7] = enable8; enabled[8] = enable9;
	dEnabled = enableD;
}

void MPlayer::enableChannel(int channel)
	{ enabled[channel] = true; }

void MPlayer::disableChannel(int channel)
	{ enabled[channel] = false; }

void MPlayer::enableDrumChannel()
	{ dEnabled = true; }

void MPlayer::disableDrumChannel()
	{ dEnabled = false; }

void MPlayer::silenceChannel(int channel)
	{ silenced[channel] = true; }

void MPlayer::silenceDrumChannel()
	{ dSilenced = true; }

void MPlayer::activateChannel(int channel)
	{ silenced[channel] = false; }

void MPlayer::activateDrumChannel()
	{ dSilenced = false; }

void MPlayer::enableLooping()
	{ loopEnabled = true; }

void MPlayer::disableLooping()
	{ loopEnabled = false; }
	
void MPlayer::advance()
{
	// advance each music oscillator
	for(int i=0; i<9; i++)
		osc[i].advance();

	// advance noise (drum) oscillator
	nosc.advance();
}

// returns one frame float value of the mix of all channels
// at current framePos
//
// param - LEFT channel = 0, RIGHT channel = 1
float MPlayer::getMix(int channel)
{
	float mix = 0.0;

	// mix all 9 channels
	for(int i=0; i<9; i++)
	{
		if(enabled[i] && silenced[i] == false)
			mix += compress(osc[i].getOutput());
	}

	// mix drum channel, too
	if(dEnabled && dSilenced == false)
		mix += compress(nosc.getOutput());

	// update delay - delay output is returned - so add to mix
	if(delayEnabled)
		mix += delay[channel].update(mix);
	
	// place holder for BCPlayer...
	mix += sfx->getOutput(channel);
	
	// mix += sfx->getOutput(channel); // add the entire SFX mix
										// for BCPlayer only!	

	// apply master gain and compress
	mix = compress(mix * masterGain);

	// limit
	if(mix >= masterOutCap)
		mix = masterOutCap;
	if(mix <= -masterOutCap)
		mix = -masterOutCap;

	return mix;
}

// compress master mix signal
float MPlayer::compress(float input)
{
	float output;

	// if positive
	if(input >= 0)
	{
		if(input < compThreshold)
			return input;
		else
		{
			output = compThreshold + ((input - compThreshold) / compRatio);
		}
	}

	// if negative
	if(input < 0)
	{
		if(input > -compThreshold)
			return input;
		else
		{
			output = -compThreshold + ((input + compThreshold) / compRatio);
		}
	}

	return output;
}

// returns the table type currently set
int MPlayer::getTableType()
	{ return tableType; }

// delay related
bool MPlayer::delayIsEnabled()
	{ return delayEnabled; }

void MPlayer::enableDelay()
	{ delayEnabled = true; }

void MPlayer::disableDelay()
{ 
	delayEnabled = false; 
	// clear delay buffer
	delay[0].clearBuffer(); // left delay
	delay[1].clearBuffer(); // right delay
}

float MPlayer::getMasterGain()
	{ return masterGain; }

void MPlayer::setMasterGain(float g)
	{ masterGain = g; }


/*----------

std::string MPlayer::exportToFile(string filename)
{

	// get the extension part of filename
	string strExt = filename.substr(filename.find_last_of('.'));
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower); // to lowercase

	// set up info to pass to libsndfile
	SF_INFO info;
	info.channels = 2;
	info.samplerate = 44100;

	// number of total frames we need
	long songFrameLen = getSongLastFrame();

	// just to fill up with zeros...
	for(int i=0;i<88200;i++)
		sndBuffer[i] = 0;

	if(strExt==".mp3") // mp3 file - will use lame encoder
	{
		// try with 8192 in one chunk
		 // * mp3buf_size in bytes = 1.25 * num_samples + 7200
		 //    1.25 x 8192 + 7200 = 17440
		int writeChunkSize = 8192;
		const int MP3_SIZE = 17440;
		unsigned char mp3_buffer[MP3_SIZE];

		FILE *mp3 = fopen(filename.c_str(), "wb");

		// initialize lame object
		lame_t lame = lame_init();
		lame_set_in_samplerate(lame, 44100);
		lame_set_VBR(lame, vbr_default);
		lame_init_params(lame);

		int read, write;
		long currentSongFrame = 0;

		// go to the beginning of the song
		goToBeginning();

		bool done = false;

		while(!done)
		{
			// let's fill the sound buffer with 8192 samples
			read = fillExportBuffer(sndBuffer, writeChunkSize, currentSongFrame, songFrameLen);
			currentSongFrame += read;

			// if reached end of song, will get out of loop and finish
			if(currentSongFrame >= songFrameLen)
				done = true;

			write = lame_encode_buffer_interleaved_ieee_float
					(lame, sndBuffer, read, mp3_buffer, MP3_SIZE);
			fwrite(mp3_buffer, write, 1, mp3);
		}

		// finish and close lame
		write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
		lame_close(lame);
		fclose(mp3);

		return "Finished writing mp3: " + filename;
	}

	else if(strExt==".wav" || strExt==".ogg") // for wav and ogg, will use libsndfile
	{
		if(strExt==".wav")
			info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		if(strExt==".ogg")
			info.format = SF_FORMAT_OGG | SF_FORMAT_VORBIS;

		// open sound file for writing...
		SNDFILE *sndFile = sf_open(filename.c_str(), SFM_WRITE, &info);
		if(sndFile==NULL)
		{
			// delete [] sndBuffer; // release memory
			string errMsg = "Error opening sound file: ";
			errMsg += sf_strerror(sndFile);
			return errMsg;
		}

		// go to the beginning of the song
		goToBeginning();
		// songFrameLen -> total frame length of song
		long currentFrame = 0;
		int writeChunkSize = 4096; // n of frames to write in each call
		bool done = false;

		while(!done)
		{
			// fill up sound buffer with a chunk of music data
			int nFramesWritten = fillExportBuffer(sndBuffer, writeChunkSize, currentFrame, songFrameLen);

			// write to file just this much
			// int framesWrittenFile = sf_writef_float(sndFile, sndBuffer, writeChunkSize);

			currentFrame += nFramesWritten; // update current position
			if(currentFrame >= songFrameLen) // reached end...
				done = true;
		}

		sf_write_sync(sndFile);
		sf_close(sndFile);

		return "Finished writing file: " + filename;
	}
	else
	{
		return "Invalid file type";
	}
}

----------*/


// fill the export buffer with music data for exporting
// just a chunk at a time - from startFrame in the song
// returns the number of frames written
int MPlayer::fillExportBuffer(float* buffer, int framesToWrite, long startFrame, int songFrameLen)
{

	bool writeFinished = false;
	long writeIndex = 0;
	int framesWritten = 0;

	while(!writeFinished)
	{
		// get mix of all 9 channels (plus drums) at current position
		//soundAmplitudeLeft = getMix(0); // get mix for LEFT channel
		//soundAmplitudeRight = getMix(1); // get mix for RIGHT channel

		//
		//	output to export buffer
		//

		buffer[writeIndex] = getMix(0); // write LEFT channel mix to buffer
		writeIndex++; // move buffer pointer
		buffer[writeIndex] = getMix(1); // write RIGHT channel mix to buffer
		writeIndex++; // move buffer pointer

		framesWritten++;

		// if reached end of note, go to next index (for regular channels ch0 - 2)
		for(int i=0; i<9; i++)
		{
			if(!channelDone[i])
			{
				remainingFrames[i]--;
				if(remainingFrames[i] <= 0)
				{
					noteIndex[i]++;

					// cout << "Frame " << framePos << " channel " << i << " - note index = " <<  noteIndex[i];

					// if there are event requests (freq >= 70000), digest those first
					bool eventsDone = false;

					while(!eventsDone)
					{
						int readValue = static_cast<int>(data[i].freqNote[noteIndex[i]]);
						if(readValue==70000.0) // 'specify volume'
						{
							// convert the passed value(1-10) to float (0.0 to 0.5f)
							float gainToSet
								= static_cast<float>(data[i].param[noteIndex[i]]) / 20.0f;
							setChannelGain(i, gainToSet);
							noteIndex[i]++;
						}
						else if(readValue==71000.0) // 'increment volume'
						{
							float gainToSet = min(0.5f, getChannelGain(i)+0.05f);
							setChannelGain(i, gainToSet);
							noteIndex[i]++;
						}
						else if(readValue==72000.0) // 'decrement volume'
						{
							float gainToSet = max(0.001f, getChannelGain(i)-0.05f);
							setChannelGain(i, gainToSet);
							noteIndex[i]++;
						}
						else // next freq value is not any event request, so we're done here
							eventsDone = true;
					}

					// and if you get to the end of MML signal (freq = -1.0), set flag
					if(data[i].freqNote[noteIndex[i]] < 0)
					{
						channelDone[i] = true;
						setToRest(i); // set to rest.. and let delay finish
						// disableChannel(i); // disable this channel
					}
					else
					{
						remainingFrames[i] = data[i].len[noteIndex[i]];
						freqNote[i] = data[i].freqNote[noteIndex[i]];

						// if this is a rest (freq = 65535), set this channel to rest
						if(freqNote[i]==65535.0)
							setToRest(i);
						// otherwise, this is a valid note - so set this note
						else
							setNewNote(i, freqNote[i]);
					}
				}
			}
		}

		// now handle drum channel!
		if(!dChannelDone)
		{
			dRemainingFrames--;
			if(dRemainingFrames <= 0)
			{
				dNoteIndex++; // move onto the next drum note index

					// if there are event requests (freq >= 70000), digest those first
					bool eventsDone = false;

					while(!eventsDone)
					{
						int readValue = ddata.drumNote[dNoteIndex];
						if(readValue==70000) // 'specify volume'
						{
							// convert the passed value(1-10) to float (0.0 to 0.5f)
							float gainToSet
								= static_cast<float>(ddata.param[dNoteIndex]) / 20.0f;
							setDChannelGain(gainToSet);
							dNoteIndex++;
						}
						else if(readValue==71000) // 'increment volume'
						{
							float gainToSet = min(0.5f, getDChannelGain()+0.05f);
							setDChannelGain(gainToSet);
							dNoteIndex++;
						}
						else if(readValue==72000) // 'decrement volume'
						{
							float gainToSet = max(0.001f, getDChannelGain()-0.05f);
							setDChannelGain(gainToSet);
							dNoteIndex++;
						}
						else // next freq value is not any event request, so we're done here
							eventsDone = true;
					}

				// and if you get to the end of MML signal (drumNote = -1.0), set flag
				if(ddata.drumNote[dNoteIndex] < 0 || dNoteIndex >= ddata.getSize())
				{
					dChannelDone = true;
					restDrum(); // rest.. and let delay effect finish off
					// disableDrumChannel(); // disable this channel
				}
				else // not at end yet.. set new drum hit
				{
					dRemainingFrames = ddata.len[dNoteIndex];
					currentDrumNote = ddata.drumNote[dNoteIndex];
					setNewDrumHit(currentDrumNote);

					// if this is a rest (freq = 65535), set flag
					if(currentDrumNote == 65535)
					{
						restDrum();
					}
				}
			}
		}

		// if song is not finished, update frame position - advance player
		if(!writeFinished)
		{
			// update player position
			framePos++;
			advance();

			// if you have reached the absolute last frame position of the song
			// (including last delay effects) - only then end the track officially
			if(framePos >= songFrameLen)
			{
				writeFinished = true;

				// DEBUG
				cout << "Export to write buffer - finished!\n";
				cout << "framePos = " << framePos << endl;
				cout << "getSongLastFrame() = " << getSongLastFrame() << endl;

				for(int i=0; i<9; i++)
					cout << "channel " << i << " length = " << data[i].totalFrames << "\n";
				cout << "d channel length = " << ddata.totalFrames << "\n";

				return framesWritten;
			}
		}

		// if you reached the size write chunk size, you can exit function
		if(framesWritten >= framesToWrite)
		writeFinished = true;
	}

	// returns how many float values have been written
	return framesWritten;

}

// used for meter visualization
// returns a particular channel's current output level
// based on a short history of gain changes (like 32*8 frames)
float MPlayer::getHistoricalAverage(int channel)
{
	if(!playing)
		return 0.0f;

	if(channel==9) // drum channel
	{
		if(dEnabled)
			return nosc.getHistoricalAverage();
		else
			return 0.0f;
	}
	else if(channel >= 0 && channel <= 8) // regular channel
	{
		if(enabled[channel])
			return osc[channel].getHistoricalAverage();
		else
			return 0.0f;
	}
	else
		return 0.0f;
}

// fast-forward (or rewind) the MPlayer position to a particular point
// in the track
void MPlayer::seek(long destination)
{
	// if requested destination is further than the last point of track
	// make it the last point of the track
	if(destination > songLastFrame)
		destination = songLastFrame;
	
	bool seekDone = false; // flag to keep track of task completion
	goToBeginning(); // go to beginning of track first
	
	//
	//  now seek to destination...
	//
	
	while(!seekDone)
	{
		for(int i=0; i<9; i++)
		{
			long seekPos = 0;
			
			// for each music channel...
			// zap through until very last note before the seekpoint
			// including events
			
			bool zappingDone = false;
			
			while(!zappingDone)
			{
					bool eventsAtThisPosDone = false;
					
					while(!eventsAtThisPosDone)
					{
						int readValue = static_cast<int>(data[i].freqNote[noteIndex[i]]);
						if(readValue==70000.0) // 'specify volume'
						{
							// convert the passed value(1-10) to float (0.0 to 0.5f)
							float gainToSet
								= static_cast<float>(data[i].param[noteIndex[i]]) / 20.0f;
							setChannelGain(i, gainToSet);
							noteIndex[i]++;
						}
						else if(readValue==71000.0) // 'increment volume'
						{
							float gainToSet = min(0.5f, getChannelGain(i)+0.05f);
							setChannelGain(i, gainToSet);
							noteIndex[i]++;
						}
						else if(readValue==72000.0) // 'decrement volume'
						{
							float gainToSet = max(0.001f, getChannelGain(i)-0.05f);
							setChannelGain(i, gainToSet);
							noteIndex[i]++;
						}
						else // next freq value is not any event request, so we're done here
							eventsAtThisPosDone = true;
					}

				// if the next note is end signal, finish this channel
				if(data[i].freqNote[noteIndex[i]] < 0)
				{
					channelDone[i] = true;
					setToRest(i);
					zappingDone = true;
				}
				
				// if not finished yet, advance note index.. til very last note before destination
				if(!channelDone[i])
				{
					// if adding next note will cause to go past destination, stop here
					if( ( seekPos + static_cast<long>(data[i].len[noteIndex[i]]) ) >= destination )
					{
						zappingDone = true;
						// DEBUG
						cout << "Zapping for ch " << i << " done at: " << seekPos << endl;
						
						// now set up the channel ready for this note
						remainingFrames[i] = data[i].len[noteIndex[i]];
						freqNote[i] = data[i].freqNote[noteIndex[i]];
						
						// if it's a rest, hadle accordingly
						if(freqNote[i]==65535.0)
							setToRest(i);
						else // otherwise we have a note
							setNewNote(i, freqNote[i]);
							
					}
					// otherwise it's good to add this next note length
					else
					{
						seekPos += data[i].len[noteIndex[i]];
						noteIndex[i]++; // advance to next note
						remainingFrames[i] = data[i].len[noteIndex[i]];
					}
				}				
			}
		
			// now process this last note
			bool lastNoteProcessingDone = false;
			
			while(!lastNoteProcessingDone)
			{
				// first off, if your channel is done for, exit!
				if(channelDone[i])
					lastNoteProcessingDone = true;
				// if you've reached destination, quit!
				else if(seekPos >= destination)
					lastNoteProcessingDone = true;
				// otherwise advance seekPos and process note
				else
				{
					seekPos++;
					osc[i].advance();
					remainingFrames[i]--;
				}
			}
			
			cout << "SEEK done: Channel " << i << " seekPos = " << seekPos << endl;
		}
		
		// for drum channel...
		// zap through until very last note before the seekpoint

			long seekPos = 0;
			
			// for each music channel...
			// zap through until very last note before the seekpoint
			// including events
			
			bool zappingDone = false;
			
			while(!zappingDone)
			{
					bool eventsAtThisPosDone = false;
					
					while(!eventsAtThisPosDone)
					{
						int readValue = static_cast<int>(ddata.drumNote[noteIndex[dNoteIndex]]);
						if(readValue==70000.0) // 'specify volume'
						{
							// convert the passed value(1-10) to float (0.0 to 0.5f)
							float gainToSet
								= static_cast<float>(ddata.param[dNoteIndex]) / 20.0f;
							setDChannelGain(gainToSet);
							dNoteIndex++;
						}
						else if(readValue==71000.0) // 'increment volume'
						{
							float gainToSet = min(0.5f, getDChannelGain()+0.05f);
							setDChannelGain(gainToSet);
							dNoteIndex++;
						}
						else if(readValue==72000.0) // 'decrement volume'
						{
							float gainToSet = max(0.001f, getDChannelGain()-0.05f);
							setDChannelGain(gainToSet);
							dNoteIndex++;
						}
						else // next freq value is not any event request, so we're done here
							eventsAtThisPosDone = true;
					}

				// if the next note is end signal, finish this channel
				if(ddata.drumNote[dNoteIndex] < 0 || dNoteIndex >= ddata.getSize())
				{
					dChannelDone = true;
					restDrum();
					zappingDone = true;
				}
				
				// if not finished yet, advance note index.. til very last note before destination
				if(!dChannelDone)
				{
					// if adding next note will cause to go past destination, stop here
					if( ( seekPos + static_cast<long>(ddata.len[dNoteIndex]) ) >= destination )
					{
						zappingDone = true;
						// DEBUG
						cout << "Zapping for drum ch done at: " << seekPos << endl;
						
						// now set up the channel ready for this note
						dRemainingFrames = ddata.len[dNoteIndex];
						currentDrumNote = ddata.drumNote[dNoteIndex];
						
						// if it's a rest, hadle accordingly
						if(currentDrumNote==65535.0)
							restDrum();
						else // otherwise we have a note
							setNewDrumHit(currentDrumNote);
							
					}
					// otherwise it's good to add this next note length
					else
					{
						seekPos += ddata.len[dNoteIndex];
						dNoteIndex++; // advance to next note
						dRemainingFrames = ddata.len[dNoteIndex];
					}
				}				
			}
		
			// now process this last note
			bool lastNoteProcessingDone = false;
			
			while(!lastNoteProcessingDone)
			{
				// first off, if your channel is done for, exit!
				if(dChannelDone)
					lastNoteProcessingDone = true;
				// if you've reached destination, quit!
				else if(seekPos >= destination)
					lastNoteProcessingDone = true;
				// otherwise advance seekPos and process note
				else
				{
					seekPos++;
					nosc.advance();
					dRemainingFrames--;
				}
			}
			
			cout << "SEEK done: Drum Channel seekPos = " << seekPos << endl;		
		
		// finally all our seeking tasks are done!
		seekDone = true;
	}
	
	framePos = destination;
	cout << "SEEK done: Player advanced to position " << framePos << endl;
}

// will seek to a particular position THEN START PLAYING
void MPlayer::seekAndStart(long destination)
{
	seek(destination); // first, seek...
	playing = true; // then start playing!
}

// get ratio of current framePos/songLastFrame (0 to 1.0)
float MPlayer::getProgressRatio()
{
	if(songFinished) // if song has finished... should always return 0
		return 0.0f;
	float r = static_cast<float>(framePos) / static_cast<float>(songLastFramePure);
	return r;
}

// get the position value of the next seek point relative to current framePos
long MPlayer::getNextSeekPoint()
{
	long blockSize = songLastFramePure / 16;
	long seekDestination = framePos + blockSize;
	if(seekDestination > songLastFramePure && loopEnabled)
		seekDestination = 0;
	
	// guard against last frame...
	if(seekDestination > songLastFrame)
		seekDestination = songLastFrame-16; // with a little safeguarding :)
	
	if(seekDestination < 0) seekDestination = 0;
	
	return seekDestination;
}

// get the previous seek point position relative to current framePos
long MPlayer::getPreviousSeekPoint()
{	
	long blockSize = songLastFramePure / 16;
	if(blockSize < 22050) // let's make it at least 0.5 second
		blockSize = 22050;
	long seekDestination = framePos - blockSize;
	if(seekDestination < 0)
		seekDestination = 0;
	return seekDestination;
}

// set the player's temp start point
void MPlayer::setBookmark(long bm)
{ bookmark = bm; }

// get the bookmark value - if 0, no bookmark
long MPlayer::getBookmark()
{ return bookmark; }

bool MPlayer::reachedSongLastFramePure()
{ return (framePos >= songLastFramePure); }

bool MPlayer::isSongFinished()
{ return songFinished; }

void MPlayer::bindSFX(SFX *sfxObj)
	{ sfx = sfxObj; }

//void MPlayer::bindSFX(SFX *sfxObj)
//{ sfx = sfxObj; }




