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
	nEvents = 0;
}

MData::MData(int sRate)
{
	sampleRate = sRate;
	totalFrames = 0;
	nEvents = 0;
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
	
	eventType.clear();
	eventType.resize(0);
	eventParam.clear();
	eventParam.resize(0);
	eventFrame.clear();
	eventFrame.resize(0);
	nEvents = 0;
	
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
	nEvents = 0;
}

DData::DData(int sRate)
{
	sampleRate = sRate;
	totalFrames = 0;
	nEvents = 0;
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
	
	eventType.clear();
	eventType.resize(0);
	eventParam.clear();
	eventParam.resize(0);
	eventFrame.clear();
	eventFrame.resize(0);
	nEvents = 0;

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




// Fall class //////////////////////////////////////////////////////////////
// Implementation //////////////////////////////////////////////////////////

#include <iostream>
#include <math.h>
#include "BC/Fall.h"

using namespace std;

const double Fall::FALL_SAMPLE_RATE = 44100.0;
const double Fall::FREQ_FLOOR = 20.0;

Fall::Fall()
{
	// set default params for now...
	enabled = false;
	fallSpeed = 600.0;
	fallFactor = 1.0;
	octTraveled = 0.0;
	octDeltaPerFrame = getDeltaPerFrame(fallSpeed);
	waitPos = 0;
	waitFrames = 0;
}

void Fall::setToDefault()
{
	// set default params for now...
	enabled = false;
	fallSpeed = 600.0;
	fallFactor = 1.0;
	octTraveled = 0.0;
	octDeltaPerFrame = getDeltaPerFrame(fallSpeed);
	waitPos = 0;
	waitFrames = 0;
}

double Fall::getDeltaPerFrame(double fSpeed)
{
	return (fSpeed / 1200.0) / FALL_SAMPLE_RATE;
}

double Fall::setSpeed(double fSpeed)
{
	fallSpeed = fSpeed;
	octDeltaPerFrame = getDeltaPerFrame(fSpeed);
}

double Fall::setWaitTime(double waitTimeMS)
{
	waitFrames = static_cast<int>(waitTimeMS / 1000.0 * FALL_SAMPLE_RATE);
}

void Fall::refresh()
{
	waitPos = 0;
	octTraveled = 0.0;
	fallFactor = 1.0;
}

void Fall::start()
{
	refresh();
	enabled = true;
}

void Fall::stop()
{
	enabled = false;
	refresh();
}

double Fall::process(double freq)
{
	if(waitPos < waitFrames)
	{
		waitPos++;
		return freq;
	}
	else
	{
		octTraveled += octDeltaPerFrame;
		if(octTraveled > 8.0) octTraveled = 8.0; // safeguard!
		freq *= pow(0.5, octTraveled);
		if(freq < FREQ_FLOOR) freq = FREQ_FLOOR; // safeguard!
		return freq;
	}
}

//////////////////////////////////////////////////////////////

const double Rise::RISE_SAMPLE_RATE = 44100.0;
const double Rise::FREQ_FLOOR = 20.0;

Rise::Rise()
{
	// set default params for now...
	enabled = false;
	pos = 0;
	riseSpeed = 1200.0;
	riseRange = 100.0;
	riseFactor = 1.0;
	octDeviation = 100.0 / 1200.0;
	octDeltaPerFrame = getDeltaPerFrame(riseSpeed);
}

void Rise::setToDefault()
{
	enabled = false;
	pos = 0;
	riseSpeed = 1200.0;
	riseRange = 100.0;
	riseFactor = 1.0;
	octDeviation = 100.0 / 1200.0;
	octDeltaPerFrame = getDeltaPerFrame(riseSpeed);	
}

double Rise::getDeltaPerFrame(double rSpeed)
{
	return (rSpeed / 1200.0) / RISE_SAMPLE_RATE;
}

void Rise::setSpeed(double rSpeed)
{
	riseSpeed = rSpeed;
	octDeltaPerFrame = getDeltaPerFrame(rSpeed);
}

void Rise::setRange(double rRange)
{
	riseRange = rRange;
	octDeviation = rRange / 1200.0;
}

void Rise::refresh()
{
	setRange(riseRange);
	pos = 0;
}

void Rise::start()
{
	refresh();
	enabled = true;
}

void Rise::stop()
{
	enabled = false;
	refresh();
}

double Rise::process(double freq)
{
	pos++;
	
	if(octDeviation == 0.0)
	{
		return freq;
	}
	else
	{
		octDeviation -= octDeltaPerFrame;
		if(octDeviation < 0.0) octDeviation = 0.0; // reached target freq!
		freq *= pow(0.5, octDeviation);
		if(freq < FREQ_FLOOR) freq = FREQ_FLOOR; // safeguard!
		return freq;
	}
}




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
	tableType = 0;
	phase = 0;
	waitPos = 0;
	setWaitTime(200); // default wait - n milliseconds
	setRange(24); // default LFO range - n cents of semitone
	setSpeed(6.0); // defalt LFO speed - n cycles per second 
}

LFO::~LFO()
{}

void LFO::initialize()
{
	// TODO - if LFO table type set to something else... re-init to sine wave type
	if(tableType!=0)
	{
		//
		
		
	}
	
	// initialize variables with default settings
	tableType = 0;
	phase = 0;
	waitPos = 0;
	setWaitTime(200); // default wait - n milliseconds
	setRange(24); // default LFO range - n cents of semitone
	setSpeed(6.0); // defalt LFO speed - n cycles per second 	
}
	
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

const int OSC::OSC_TABLE_SIZE = 4096;
const int OSC::ENV_TABLE_SIZE = 1024;
const double OSC::OSC_SAMPLE_RATE = 44100.0;
const float OSC::TWO_PI = 6.283185307;

OSC::OSC()
{
	table.resize(OSC_TABLE_SIZE); // wave table (vector)
	
	tableType = 0; // FORCE setTable() to rewrite wavetable
	setTable(1); // default - set up a square table
	tableType = 1;
	yFlip = 1.0f;
	phase = 0.0;
	increment = 0.0;
	freq = 10.0; // not to set to zero to safeguard
	adjustedFreq = 0;
	detune = 0;
	gain = 0.5f; // default gain
	
	resting = false;
	forceSilenceAtBeginning = false;
	
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
	
	fallActive = false;
	riseActive = false;
	
	beefUp = false;
	beefUpFactor = 1.0f;
	compRatio = 4.0f;
	compThreshold = 0.90f;
	
	popGuardCount = 0;
	lastAmp = 0.0f;
	
	// initialize history table
	clearHistory();
}

OSC::~OSC()
{}

void OSC::setTable(int type)
{
	// DEBUG
	//if(tableType==type)
	//	cout << "NO CHANGE TO WAVEFORM! YEY!!\n";
	
	// if requested type is the currently set type
	// don't have to make any change... skip
	if(tableType==type)
		return;
	
	tableType = type;
	
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
				table[i] = 0.80f;
			}
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = -0.80f;
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
			{
				int index = OSC_TABLE_SIZE/4 + i;
				table[index] = -0.99f  + (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE/2)) * 1.98f;
			}				
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
			{
				int index = OSC_TABLE_SIZE/4 + i;
				if(index>=OSC_TABLE_SIZE) index -= OSC_TABLE_SIZE;
				table[index] = 0.99f  - (static_cast<float>(i-OSC_TABLE_SIZE/2) / static_cast<float>(OSC_TABLE_SIZE/2)) * 1.98f;
			}
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

		// pulse wave 12.5%-87.5% ratio
		case 6:
			for(int i=0; i<OSC_TABLE_SIZE/8; i++)
			{
				table[i] = -0.80f;
			}
			for(int i=OSC_TABLE_SIZE/8; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = 0.80f;
			}			
			break;			
			
		// pulse wave 25%-75% ratio
		case 7:
		
			for(int i=0; i<OSC_TABLE_SIZE/4; i++)
			{
				table[i] = -0.80f;
			}
			for(int i=OSC_TABLE_SIZE/4; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = 0.80f;
			}		
			break;
			
		// pulse wave 33.3% ratio
		case 8:
			for(int i=0; i<OSC_TABLE_SIZE/3; i++)
			{
				table[i] = -0.80f;
			}
			for(int i=OSC_TABLE_SIZE/3; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = 0.80f;
			}				
			break;

		// default is SQUARE wave...
		default:
			for(int i=0; i<OSC_TABLE_SIZE/2; i++)
			{
				table[i] = -0.80f;
			}
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = 0.80f;
			}
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
		if(!envRfinished && !forceSilenceAtBeginning)
			output = sustainLevel * ( static_cast<float>(nReleaseFrames - releasePos) / static_cast<float>(nReleaseFrames) );
		else
		{
			output = 0.0f;
			phase = 0; // reset phase for next note!
		}
	}
	
	return output;
}

void OSC::setToRest()
{
	resting = true;
}

void OSC::confirmFirstNoteIsRest()
{
	forceSilenceAtBeginning = true;
}

void OSC::advance()
{
	// advance on the sample table
	phase += increment;
	
	adjustedFreq = freq;
	
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
		
		// if Fall is enabled, process and adjust frequency
		if(fallActive)
		{
			adjustedFreq = fall.process(adjustedFreq);
			setIncrement(adjustedFreq);
		}

		// if Rise is enabled, process and adjust frequency
		if(riseActive)
		{
			adjustedFreq = rise.process(adjustedFreq);
			setIncrement(adjustedFreq);
		}		
	}
	
	// if LFO is enabled, process and adjust frequency
	else if(lfoEnabled)
	{
		adjustedFreq = lfo.process(freq);
		if(adjustedFreq < 10.0)
			adjustedFreq = 10.0;
		setIncrement(adjustedFreq);
	}
	
	// if Fall is enabled, process and adjust frequency
	if(fallActive && !astroEnabled)
	{
		adjustedFreq = fall.process(freq);
		setIncrement(adjustedFreq);
	}
	
	// if Rise is enabled, process and adjust frequency
	if(riseActive && !astroEnabled)
	{
		adjustedFreq = rise.process(adjustedFreq);
		setIncrement(adjustedFreq);
	}
	
	// advance envelope also
	advanceEnvelope();
}

void OSC::setNewNote(double newFreq)
{
	forceSilenceAtBeginning = false;
	setFrequency(newFreq);
	// initializePhase();
	refreshEnvelope();
	resting = false;
	if(fallActive && fall.octTraveled > 0.0)
		stopFall();
	if(riseActive && rise.pos > 30)
		stopRise();
	
	// enable pop-guarding...
	popGuardCount = 60;
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
	
void OSC::initializeLFO()
	{ lfo.initialize();	}
	
void OSC::setLFOwaitTime(int milliseconds)
	{ lfo.setWaitTime(milliseconds); }

void OSC::setLFOrange(int cents)
	{ lfo.setRange(cents); }

void OSC::setLFOspeed(double cyclePerSeconds)
	{ lfo.setSpeed(cyclePerSeconds); }

void OSC::startFall()
{
	fallActive = true;
	fall.start();
}

void OSC::stopFall()
{
	fallActive = false;
	fall.stop();
}

void OSC::setFallSpeed(double fallSpeed)
{
	fall.setSpeed(fallSpeed);
}

void OSC::setFallWait(double waitTimeMS)
{
	fall.setWaitTime(waitTimeMS);
}

void OSC::setFallToDefault()
{
	stopFall();	
	fall.setToDefault();
}

void OSC::startRise()
{
	riseActive = true;
	rise.start();
}

void OSC::stopRise()
{
	riseActive = false;
	rise.stop();
}

void OSC::setRiseSpeed(double riseSpeed)
{
	rise.setSpeed(riseSpeed);
}

void OSC::setRiseRange(double riseRange)
{
	rise.setRange(riseRange);
}

void OSC::setRiseToDefault()
{
	stopRise();
	rise.setToDefault();
}

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

void OSC::refreshForSongBeginning()
{
	initializePhase();
	lastAmp = 0.0f;
	refreshEnvelope();
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
	
	float out;
	
	if(yFlip > 0)
		out = table[ph] * getEnvelopeOutput();
	else
		out = -table[ph] * getEnvelopeOutput();
	
	// if BeefUp is enabled... beef up and compress!
	if(beefUp)
		out = compress(out * beefUpFactor);
	
	out *= gain;
	
	// popguard - just for the first 2 frames...
	if(popGuardCount>0)
		out = popGuard(out);
	else
		lastAmp = out;
	
	historyWriteWait++;
	if(historyWriteWait >= 8)
	{
		pushHistory(out);
		historyWriteWait = 0;
	}
	
	return out;
}

float OSC::compress(float in)
{
	float out = in;
	if(in >= 0.0f && in > compThreshold) // positive value
	{
		float delta = in - compThreshold;
		delta = delta / compRatio;
		out = compThreshold + delta;
		if(out>=0.99f) out = 0.99f;
	}
	else if(in <= 0.0f && in < -compThreshold) // negative value
	{
		float delta = in + compThreshold;
		delta = delta / compRatio;
		out = -compThreshold + delta;
		if(out<=-0.99f) out = -0.99f;
	}
	return out;
}

float OSC::popGuard(float in)
{
	float inPositive = in + 1.0f;
	if(inPositive<0.0f) inPositive = 0.0f;
	float lastAmpPositive = lastAmp + 1.0f;
	if(lastAmpPositive<0.0f) lastAmpPositive = 0.0f;
	// float travelAmount = lastAmpPositive - inPositive;
	inPositive += (lastAmpPositive - inPositive) * (static_cast<float>(popGuardCount) / 60.0f);
	popGuardCount--;
	
	return (inPositive-1.0f);
}

void OSC::enableBeefUp()
{ beefUp = true; }

void OSC::disableBeefUp()
{ beefUp = false; }

void OSC::setBeefUpFactor(float factor)
{ beefUpFactor = factor; }

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

// set yFlip between 1.0 and -1.0
// determines if wavetable is read as is or vertically inverted
void OSC::flipYAxis()
{
	yFlip = -1.0f;
}

// reset yFlip to default normal 1.0 (table reading won't get vertically inverted)
void OSC::resetYFlip()
{
	yFlip = 1.0f;
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

const int NOSC::NOSC_NTABLE_SIZE = 9999;
const int NOSC::NOSC_PTABLE_SIZE = 4096;
const double NOSC::NOSC_SAMPLE_RATE = 44100.0;

NOSC::NOSC()
{	
	// set up a noise wave nTable
	nTable.resize(NOSC_NTABLE_SIZE);
	nPinkTable.resize(NOSC_NTABLE_SIZE);
	pTable.resize(NOSC_PTABLE_SIZE);
	setTable();

	for(int i=0; i<6; i++)
		noiseType[i] = 0; // set all to use white noise to begin
	
	squareLevel = 1.0f;
	noiseLevel = 1.0f;
	
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
	setDrumTone(0, 1, 25,  15,  0.8, 200.0,  50  , 0.9, 2.0); // kick
	setDrumTone(1, 1, 20, 120, 0.95, 720.0,  100 , 0.85, 1.2); // snare
	setDrumTone(2, 1, 17,  3,  0.5, 2400.0, 1   , 0.2, 2.0); // hat
	
	setDrumTone(3, 1, 25,  15,  0.4, 200.0,  50  , 0.5, 2.0); // kick - quiet
	setDrumTone(4, 1, 20, 120, 0.5, 720.0,  100 , 0.45, 1.2); // snare - quiet
	setDrumTone(5, 1, 17,  3,  0.2, 2400.0, 1   , 0.1, 2.0); // hat - quiet
	
	kickFreq = 200.0;
	snareFreq = 720.0;
	hihatFreq = 2400.0;
	
	kickPeakTime = 25; kickDecayTime = 15;
	snarePeakTime = 20; snareDecayTime = 120;
	hihatPeakTime = 17; hihatDecayTime = 3;

	envPos = 0;	
	
	envFinished = false;

	beefUp = false;
	beefUpFactor = 1.0f;
	beefUpFactorNoise = 1.0f;
	compRatio = 4.0f;
	compThreshold = 0.90f;
	
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
	
	// create pTable (pitched element)
	for(int i=0; i<midTablePoint; i++)
		pTable[i] = 0.95f;
	for(int i=midTablePoint; i<NOSC_PTABLE_SIZE; i++)
		pTable[i] = -0.95f;
	
	float b0, b1, b2;
	
	// create pink noise table from white noise
	// using paul Kellet's economy method (http://www.firstpr.com.au/dsp/pink-noise/#uses)
	for(int i=0; i<NOSC_NTABLE_SIZE; i++)
	{
		b0 = 0.99765f * b0 + nTable[i] * 0.0990460f; 
		b1 = 0.96300f * b1 + nTable[i] * 0.2965164f; 
		b2 = 0.57000f * b2 + nTable[i] * 1.0526913f; 
		nPinkTable[i] = (b0 + b1 + b2 + nTable[i] * 0.1848f) * 0.32f;	
	}
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
	
	// DEBUG
	// cout << "attack=" << nAttackFrames[dType] << " peakTime=" << nPeakFrames[dType] << " decayTime" << nDecayFrames[dType] << " envFrames=" << nEnvFrames[dType] << endl;
}

// reset all drum settings to default
void NOSC::resetDrumTones()
{
	squareLevel = 1.0f;
	noiseLevel = 1.0f;
	
	useWhiteNoise();
	
	// set default envelope for each drum
	setDrumTone(0, 1, 25,  15,  0.8, 200.0,  50  , 0.9, 2.0); // kick
	setDrumTone(1, 1, 20, 120, 0.95, 720.0,  100 , 0.85, 1.2); // snare
	setDrumTone(2, 1, 17,  3,  0.5, 2400.0, 1   , 0.2, 2.0); // hat
	
	setDrumTone(3, 1, 25,  15,  0.4, 200.0,  50  , 0.5, 2.0); // kick - quiet
	setDrumTone(4, 1, 20, 120, 0.5, 720.0,  100 , 0.45, 1.2); // snare - quiet
	setDrumTone(5, 1, 17,  3,  0.2, 2400.0, 1   , 0.1, 2.0); // hat - quiet
	
	kickFreq = 200.0;
	snareFreq = 720.0;
	hihatFreq = 2400.0;
	
	kickPeakTime = 25; kickDecayTime = 15;
	snarePeakTime = 20; snareDecayTime = 120;
	hihatPeakTime = 17; hihatDecayTime = 3;
}

// sets kick's length - takes a millisecond value
void NOSC::setKickLength(int lenMilSec)
{
	int peakTime, decayTime;
	if(lenMilSec < 0) lenMilSec = 0;
	else if(lenMilSec > 400) lenMilSec = 400;
	
	if(lenMilSec >= 25)
	{
		peakTime = 25;
		decayTime = lenMilSec - peakTime;
	}
	else // if smaller than 30 milsec, you just get peak stage w/ no decay
	{
		peakTime = lenMilSec;
		decayTime = 0;
	}

	setDrumTone(0, 1, peakTime,  decayTime,  0.8, kickFreq,  50  , 0.9, 2.0); // kick	
	setDrumTone(3, 1, peakTime,  decayTime,  0.4, kickFreq,  50  , 0.5, 2.0); // kick - quiet
	kickPeakTime = peakTime; kickDecayTime = decayTime;
}

// sets snare's length - takes a millisecond value
void NOSC::setSnareLength(int lenMilSec)
{
	int peakTime, decayTime;
	if(lenMilSec < 0) lenMilSec = 0;
	else if(lenMilSec > 1000) lenMilSec = 1000;
	
	if(lenMilSec >= 20)
	{
		peakTime = 20;
		decayTime = lenMilSec - peakTime;
	}
	else // if smaller than 20 milsec, you just get peak stage w/ no decay
	{
		peakTime = lenMilSec;
		decayTime = 0;
	}
	
	setDrumTone(1, 1, peakTime,  decayTime, 0.95, snareFreq,  100 , 0.85, 1.2); // snare
	setDrumTone(4, 1, peakTime,  decayTime, 0.5 , snareFreq,  100 , 0.45, 1.2); // snare - quiet
	snarePeakTime = peakTime; snareDecayTime = decayTime;
}

// sets hihat's length - takes a millisecond value
void NOSC::setHiHatLength(int lenMilSec)
{
	int peakTime, decayTime;
	if(lenMilSec < 0) lenMilSec = 0;
	else if(lenMilSec > 1000) lenMilSec = 1000;

	if(lenMilSec >= 17)
	{
		peakTime = 17;
		decayTime = lenMilSec - peakTime;
	}
	else // if smaller than 17 milsec, you just get peak stage w/ no decay
	{
		peakTime = lenMilSec;
		decayTime = 0;
	}

	setDrumTone(2, 1, peakTime, decayTime,  0.5, hihatFreq, 1   , 0.2, 2.0); // hat
	setDrumTone(5, 1, peakTime, decayTime,  0.2, hihatFreq, 1   , 0.1, 2.0); // hat - quiet
	hihatPeakTime = peakTime; hihatDecayTime = decayTime;
}

// change kick's tuning frequency
// takes a double variable from 50 to 350 (default=200.0, at 50%)
void NOSC::tuneKick(double freq)
{
	if(freq < 50.0) freq = 50.0;
	else if(freq > 350.0) freq = 350.0;
	setDrumTone(0, 1, kickPeakTime, kickDecayTime,  0.8, freq,  50  , 0.9, 2.0); // kick
	setDrumTone(3, 1, kickPeakTime, kickDecayTime,  0.4, freq,  50  , 0.5, 2.0); // kick - quiet
	kickFreq = freq;
}

// change snare's tuning frequency
// takes a double variable from 200 to 1240 (default=720.0 at 50%)
void NOSC::tuneSnare(double freq)
{
	if(freq < 200.0) freq = 200.0;
	else if(freq > 1240.0) freq = 1240.0;
	setDrumTone(1, 1, snarePeakTime, snareDecayTime, 0.95, freq,  100 , 0.85, 1.2); // snare
	setDrumTone(4, 1, snarePeakTime, snareDecayTime, 0.5,  freq,  100 , 0.45, 1.2); // snare - quiet
	snareFreq = freq;	
}

// change snare's tuning frequency
// takes a double variable from 1200 to 3600 (default=2400.0 at 50%)
void NOSC::tuneHiHat(double freq)
{
	if(freq < 1200.0) freq = 1200.0;
	else if(freq > 3600.0) freq = 3600.0;
	setDrumTone(2, 1, hihatPeakTime, hihatDecayTime,  0.5, freq, 1   , 0.2, 2.0); // hat
	setDrumTone(5, 1, hihatPeakTime, hihatDecayTime,  0.2, freq, 1   , 0.1, 2.0); // hat - quiet
	hihatFreq = freq;	
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

	float noiseOut;
	
	if(noiseType[drumType]==1) // if one, use pink noise
		noiseOut = nPinkTable[ph] * noiseLevel;
	else // if zero, use white noise
		noiseOut = nTable[ph] * noiseLevel;	
		
		

	float pitchOut = getPitchOutput() * squareLevel;

	float out;
	
	// output the regular way
	if(!beefUp)
		out = ( noiseOut * gain + pitchOut * gain) * getEnvelopeOutput();
	
	// if beefUp is enabled
	else
		out = limit( 	pitchOut * gain * getEnvelopeOutput() * beefUpFactor +
						noiseOut * gain * getEnvelopeOutput() * beefUpFactorNoise );
	
	historyWriteWait++;
	if(historyWriteWait >= 8)
	{
		pushHistory(out);
		historyWriteWait = 0;
	}	
	
	return out;
	// return ( noiseOut * gain + pitchOut * gain) * getEnvelopeOutput();
}

// same compressor used for regular OSC channel ... might be kind of soft for drums though
float NOSC::compress(float in)
{
	float out = in;
	if(in >= 0.0f && in > compThreshold) // positive value
	{
		float delta = in - compThreshold;
		delta = delta / compRatio;
		out = compThreshold + delta;
		if(out>=0.99f) out = 0.99f;
	}
	else if(in <= 0.0f && in < -compThreshold) // negative value
	{
		float delta = in + compThreshold;
		delta = delta / compRatio;
		out = -compThreshold + delta;
		if(out<=-0.99f) out = -0.99f;
	}
	return out;
}

// alternate BEEFUP processing... brickwall limiter
float NOSC::limit(float in)
{
	float out = in;
	if(out>=0.99f) out = 0.99f;
	else if(out<=-0.99f) out = -0.99f;
	return out;
}

void NOSC::enableBeefUp()
{ beefUp = true; }

void NOSC::disableBeefUp()
{ beefUp = false; }

void NOSC::setBeefUpFactor(float factor)
{
	beefUpFactor = factor; // this works on the pitched element
	beefUpFactorNoise = ((factor - 1.0f) * 0.80f) + 1.0f; // works on the noise element
}

void NOSC::useWhiteNoise()
{ 
	for(int i=0; i<6; i++)
		noiseType[i] = 0;
}

void NOSC::usePinkNoise()
{ 
	for(int i=0; i<6; i++)
		noiseType[i] = 1;
}
	
void NOSC::setKickNoiseType(int type)
	{ noiseType[0] = type; noiseType[3] = type; }
	
void NOSC::setSnareNoiseType(int type)
	{ noiseType[1] = type; noiseType[4] = type; }

void NOSC::setHiHatNoiseType(int type)
	{ noiseType[2] = type; noiseType[5] = type; }
	
void NOSC::setNoiseLevel(float nLevel)
	{ noiseLevel = nLevel; }

void NOSC::setSquareLevel(float sqLevel)
	{ squareLevel = sqLevel; }
	
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
#include <ctime>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
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
	for(int i=0; i<N_EVENT_TAGS; i++)
		eventTag[i] = "";
	eventTag[0]="DEFAULTTONE"; eventTag[1]="LFO=ON"; eventTag[2]="LFO=OFF"; eventTag[3]="PRESET=BEEP";
	eventTag[4]="PRESET=POPPYVIB"; eventTag[5]="PRESET=POPPY"; eventTag[6]="PRESET=BELL";
	eventTag[20]="WAVEFLIP";
	eventTag[100]="WAVEFORM="; eventTag[101]="ATTACKTIME="; eventTag[102]="PEAKTIME="; eventTag[103]="DECAYTIME=";
	eventTag[104]="RELEASETIME="; eventTag[105]="PEAKLEVEL="; eventTag[106]="SUSTAINLEVEL=";
	eventTag[107]="ASTRO=OFF"; eventTag[108]="ASTRO=";
	eventTag[109]="LFORANGE="; eventTag[110]="LFOSPEED="; eventTag[111]="LFOWAIT=";
	eventTag[112]="FALLSPEED="; eventTag[113]="FALLWAIT="; eventTag[114]="RISESPEED="; eventTag[115]="RISERANGE=";
	eventTag[116]="BEEFUP="; eventTag[117]="RINGMOD=OFF"; eventTag[118]="RINGMOD=";
	
	// NOTE: if you want to register a tag that contains another tag name, register the longer tag first!
	// for example, PRESET=POPPYVIB must come earlier than PRESET=POPPY
	
	for(int i=0; i<N_EVENT_TAGS; i++)
	{
		if(eventTag[i].empty())
			eventTagLen[i] = 0;
		else
			eventTagLen[i] = eventTag[i].length();
	}

	for(int i=0; i<N_EVENT_TAGS; i++)
		eventTagDrum[i] = "";
	eventTagDrum[0]="RESETDRUMS"; eventTagDrum[1]="WHITENOISE"; eventTagDrum[2]="PINKNOISE";
	eventTagDrum[3]="KICKNOISE=WHITE"; eventTagDrum[4]="KICKNOISE=PINK";
	eventTagDrum[5]="SNARENOISE=WHITE"; eventTagDrum[6]="SNARENOISE=PINK";
	eventTagDrum[7]="HIHATNOISE=WHITE"; eventTagDrum[8]="HIHATNOISE=PINK";
	eventTagDrum[100]="KICKPITCH="; eventTagDrum[101]="SNAREPITCH="; eventTagDrum[102]="HIHATPITCH=";
	eventTagDrum[103]="BEEFUP="; 
	eventTagDrum[104]="KICKLENGTH="; eventTagDrum[105]="SNARELENGTH="; eventTagDrum[106]="HIHATLENGTH=";
	eventTagDrum[107]="SQUARELEVEL="; eventTagDrum[108]="NOISELEVEL=";
	
	for(int i=0; i<N_EVENT_TAGS; i++)
	{
		if(eventTagDrum[i].empty())
			eventTagLenDrum[i] = 0;
		else
			eventTagLenDrum[i] = eventTagDrum[i].length();
	}

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

	// take out '(', too...
	done = false;
	while(!done)
	{
		found = str.find('(');
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
		output = &(player->data[channel]); // gets pointer to MData object
	else
		return "Error - choose valid channel!";

	// channel source string to work on
	string str = source[channel];

	//
	// first take care of event change commands
	// enclose them with parenthesis for now...
	//
	
	
	bool eventTagsDone = false; // when all config statements are parsed, this gets set to true
	int searchPos = 0;
	int strLen = str.length();
	str = str + "              $$$$$$"; // safeguard, and signal end of string!
	size_t found;

	while(!eventTagsDone)
	{		
		// if an event tag is found, enclose with ()
		for(int i=0; i<N_EVENT_TAGS; i++)
		{
			if(!eventTag[i].empty())
			{
				if( str.substr(searchPos, eventTagLen[i]) == eventTag[i] ) // found!
				{					
					int targetLen = eventTagLen[i];
					int digits = 0;
					
					// number 100 and later - these are tags that take parameters
					// let's place ')' after the parameter digits
					if(i>=100)
					{
						int digitStart = searchPos + targetLen;
						digits = countDigits( str.substr(digitStart, 5) );
					}
					
					str.insert(searchPos + targetLen + digits, ")");
					str.insert(searchPos,"(");
					strLen += 2; // we just increased the string's length by w chars...
					searchPos += targetLen; // advance.. we should skip the newly inserted '('
					i = N_EVENT_TAGS; // force this loop to end
				}
			}
		}
		
		searchPos++;
		if(searchPos>=strLen)
			eventTagsDone = true;

	}

	// cout << "after enclosing all the event commands..." << endl;
	// cout << "channel " << channel << " string is now.." << endl << str << endl;
	// while(!GetAsyncKeyState(VK_SPACE));

	//
	// parse the music part
	//

	// first, parse the repeat signs
	// (any repeated parts will be duplicated)

	str = str + "     $$$$$$"; // to signal end of string
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
			
			if(str.at(i)==',') // fall effect for this note!
			{
				// DEBUG
				cout << "parsing - found a ',' - FALL!\n";
				
				// push this 'fall' event to events vector in MData
				output->eventType.push_back(50); // FALL
				output->eventParam.push_back(0);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;
				i++;
			}
			
			int extraFrames = 0;
			
			if(str.at(i)=='~') // tie to another note unit
			{
				noteLengthToAssign += noteLength;
				extraFrames += noteLength;
				i++;
				
				while(str.at(i)=='~' || str.at(i)==',')
				{
					if(str.at(i)=='~')
					{
						noteLengthToAssign += noteLength;
						extraFrames += noteLength;
						i++;
					}
					else if(str.at(i)==',') // FALL after or among ties!
					{
						// push this 'fall' event to events vector in MData
						output->eventType.push_back(50); // FALL
						output->eventParam.push_back(0);
						output->eventFrame.push_back(framesWritten + extraFrames);
						output->nEvents++;
						i++;
					}
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
		
		else if(ch=='*') // rise!
		{
			// push this 'rise' event to events vector in MData
			output->eventType.push_back(60); // FALL
			output->eventParam.push_back(0);
			output->eventFrame.push_back(framesWritten);
			output->nEvents++;
			
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
			int notes[32] = {0};
			int tie[32] = {0};
			int nNotes = 0;
			int nTied = 0;
			int tupletIndex = 0;
			int wholeLength = noteLength; // default length to set for now
			int risePosition[32] = {0};
			int riseIndex = 0;
			int nRises = 0;
			int fallPosition = 0;
			
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
					
					// process ties here...
					//
					
					// if a tie follows a note name
					if(str.at(i)=='~')
					{
						tie[tupletIndex]++;
						nTied++;
						i++;
						while(str.at(i)=='~') // we might even have more ties!
						{
							tie[tupletIndex]++;
							nTied++;
							i++;
						}
					}
					
					//
					//
					
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
				
				/*

				else if(str.at(i)=='~') // tie last note
				{
					tie[tupletIndex] = 1;
					nTied++;
					i++;
				}
				
				*/

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
				
				else if(str.at(i)=='*') // we have a rise!
				{
					risePosition[riseIndex] = nNotes;
					riseIndex++;
					nRises++;
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
						
						// if there are rises in tuplet, process
						if(nRises > 0)
						{
							for(int i=0;i<nRises;i++)
							{
								int nNotesPassed = risePosition[i];
								int waitUnits = 0;
								for(int j=0; j<nNotesPassed; j++)
								{
									waitUnits++;
									waitUnits += tie[j];
								}
								
								int extraAdd = waitUnits * eachTupletLength;
								// push this 'rise' event to events vector in MData
								output->eventType.push_back(60); // RISE
								output->eventParam.push_back(0);
								output->eventFrame.push_back(framesWritten + extraAdd);
								output->nEvents++;						
							}
						}
	
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
			
			// push this event to events vector in MData
			output->eventType.push_back(0);
			output->eventParam.push_back(value);
			output->eventFrame.push_back(framesWritten);
			output->nEvents++;
			
			i++;
		}

		else if(str.at(i)=='^') // Volume increment request
		{
			
			// push this event to events vector in MData
			output->eventType.push_back(1); // event type 1 is 'increment volume'
			output->eventParam.push_back(0);
			output->eventFrame.push_back(framesWritten);
			output->nEvents++;
			
			i++;
		}

		else if(str.at(i)=='_') // Volume decrement request
		{
			
			// push this event to events vector in MData
			output->eventType.push_back(2); // event type 2 is 'decrement volume'
			output->eventParam.push_back(0);
			output->eventFrame.push_back(framesWritten);
			output->nEvents++;
			
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
		// if we have an event command...
		else if(str.at(i)=='(')
		{
			i++;
			
			if(str.substr(i, 11) == "DEFAULTTONE")
			{
				output->eventType.push_back(1000);
				output->eventParam.push_back(0);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 11) == "PRESET=BEEP")
			{
				output->eventType.push_back(1001);
				output->eventParam.push_back(0);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 15) == "PRESET=POPPYVIB") // longer tag must come first!
			{
				output->eventType.push_back(1003);
				output->eventParam.push_back(0);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 12) == "PRESET=POPPY") // then the shorter one..
			{
				output->eventType.push_back(1002);
				output->eventParam.push_back(0);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 11) == "PRESET=BELL")
			{
				output->eventType.push_back(1004);
				output->eventParam.push_back(0);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 6) == "LFO=ON")
			{
				output->eventType.push_back(30);
				output->eventParam.push_back(1); // 1 for on
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 7) == "LFO=OFF")
			{
				output->eventType.push_back(30);
				output->eventParam.push_back(0); // 0 for off
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 9) == "WAVEFORM=")
			{
				string strValue = str.substr(i+9,2); // get 2 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(99, max(0, value)); // floor + ceil the value				
				
				output->eventType.push_back(10);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;
			}
			else if(str.substr(i, 8) == "WAVEFLIP")
			{
				output->eventType.push_back(11);
				output->eventParam.push_back(0); // 0 - dummy
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 11) == "ATTACKTIME=")
			{
				string strValue = str.substr(i+11,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(9999, max(0, value)); // floor + ceil the value				
				
				output->eventType.push_back(20);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;
			}
			else if(str.substr(i, 9) == "PEAKTIME=")
			{
				string strValue = str.substr(i+9,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(9999, max(0, value)); // floor + ceil the value
				
				output->eventType.push_back(21);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 10) == "DECAYTIME=")
			{
				string strValue = str.substr(i+10,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(9999, max(0, value)); // floor + ceil the value

				output->eventType.push_back(22);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;					
			}
			else if(str.substr(i, 12) == "RELEASETIME=")
			{
				string strValue = str.substr(i+12,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(9999, max(0, value)); // floor + ceil the value

				output->eventType.push_back(23);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;	
			}
			else if(str.substr(i, 10) == "PEAKLEVEL=")
			{
				string strValue = str.substr(i+10,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value
				
				output->eventType.push_back(24);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;					
			}
			else if(str.substr(i, 13) == "SUSTAINLEVEL=")
			{
				string strValue = str.substr(i+13,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value

				output->eventType.push_back(25);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;					
			}
			else if(str.substr(i, 6) == "ASTRO=")
			{
				if(str.substr(i, 9) == "ASTRO=OFF")
				{
					output->eventType.push_back(41);
					output->eventParam.push_back(0);
					output->eventFrame.push_back(framesWritten);
					output->nEvents++;
				}
				else
				{
					string strValue = str.substr(i+6,3); // get 3 digits following '='
					int valueDigits = countDigits(strValue);
					strValue = strValue.substr(0, valueDigits);
					int value = atoi(strValue.c_str());
					value = min(100, max(0, value) ); // floor + ceil the value
	
					output->eventType.push_back(40);
					output->eventParam.push_back(value);
					output->eventFrame.push_back(framesWritten);
					output->nEvents++;
				}
			}
			else if(str.substr(i, 9) == "LFORANGE=")
			{
				string strValue = str.substr(i+9,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(3600, max(1, value)); // floor + ceil the value				
				
				output->eventType.push_back(31);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 9) == "LFOSPEED=")
			{
				string strValue = str.substr(i+9,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atof(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value
				
				output->eventType.push_back(32);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 8) == "LFOWAIT=")
			{
				string strValue = str.substr(i+8,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(3000, max(1, value)); // floor + ceil the value
				
				output->eventType.push_back(33);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 10) == "FALLSPEED=") // 100ths of an octave per second
			{
				string strValue = str.substr(i+10,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(6000, max(1, value)); // floor + ceil the value
				
				output->eventType.push_back(51);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 9) == "FALLWAIT=") // in milliseconds
			{
				string strValue = str.substr(i+9,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(9999, max(1, value)); // floor + ceil the value
				
				output->eventType.push_back(52);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 10) == "RISESPEED=") // 100ths of an octave per second
			{
				string strValue = str.substr(i+10,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(9600, max(1, value)); // floor + ceil the value
				
				output->eventType.push_back(61);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 10) == "RISERANGE=") // in 100ths of octaves, pitch deviation to start from
			{
				string strValue = str.substr(i+10,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(9600, max(1, value)); // floor + ceil the value
				
				output->eventType.push_back(62);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 7) == "BEEFUP=") // in a value 0 to 100 - percentage value
			{
				string strValue = str.substr(i+7,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value
				
				output->eventType.push_back(70);
				output->eventParam.push_back(value);
				output->eventFrame.push_back(framesWritten);
				output->nEvents++;				
			}
			else if(str.substr(i, 8) == "RINGMOD=") // in a value 0 to 9 - channel num, 0 for OFF
			{
				if(str.substr(i, 11) == "RINGMOD=OFF")
				{
					output->eventType.push_back(81);
					output->eventParam.push_back(0);
					output->eventFrame.push_back(framesWritten);
					output->nEvents++;
				}
				else
				{
					string strValue = str.substr(i+8,1); // get 1 digit following '='
					int valueDigits = countDigits(strValue);
					strValue = strValue.substr(0, valueDigits);
					int value = atoi(strValue.c_str());
					value = min(9, max(0, value)); // floor + ceil the value
					
					output->eventType.push_back(80);
					output->eventParam.push_back(value);
					output->eventFrame.push_back(framesWritten);
					output->nEvents++;
				}				
			}
			// cout << "event parsing done! " << str.substr(i,13) << "\n";
			
			// fast forward to where we find the next ')'
			bool fastFwdDone = false;
			while(!fastFwdDone)
			{
				i++;
				if(str.at(i)==')') // stop at where ')' is
					fastFwdDone = true;
				else if(str.at(i)=='$') // we didn't have ')'!!
					fastFwdDone = true;
			}
			
		}
		else if(ch=='$') // end of string
		{
			// insert data to signify end
			output->freqNote.push_back(-1.0);
			output->len.push_back(-1);
			output->param.push_back(0);
			output->eventType.push_back(-1);
			output->eventParam.push_back(0);
			output->eventFrame.push_back(framesWritten);
			output->nEvents++;

			// integrity check

			int r = framesWritten % quarterNoteLength;

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
	

	//
	// first take care of event change commands
	// enclose them with parenthesis for now...
	//
	
	
	bool eventTagsDone = false; // when all config statements are parsed, this gets set to true
	int searchPos = 0;
	int strLen = str.length();
	str = str + "                $$$$$$"; // safeguard, and signal end of string!
	size_t found;

	while(!eventTagsDone)
	{		
		// if an event tag is found, enclose with ()
		for(int i=0; i<N_EVENT_TAGS; i++)
		{
			if(!eventTagDrum[i].empty())
			{
				if( str.substr(searchPos, eventTagLenDrum[i]) == eventTagDrum[i] ) // found!
				{					
					int targetLen = eventTagLenDrum[i];
					int digits = 0;
					
					// number 100 and later - these are tags that take parameters
					// let's place ')' after the parameter digits
					if(i>=100)
					{
						int digitStart = searchPos + targetLen;
						digits = countDigits( str.substr(digitStart, 5) );
					}
					
					str.insert(searchPos + targetLen + digits, ")");
					str.insert(searchPos,"(");
					strLen += 2; // we just increased the string's length by w chars...
					searchPos += targetLen; // advance.. we should skip the newly inserted '('
					i = N_EVENT_TAGS; // force this loop to end
				}
			}
		}
		
		searchPos++;
		if(searchPos>=strLen)
			eventTagsDone = true;

	}
	
	cout << "After parsing drum source...\n" << str << endl;
	
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
			int notes[32] = {0};
			int tie[32] = {0};
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
					
					//
					// process ties here...
					//
					
					// if a tie follows a note name
					if(str.at(i)=='~')
					{
						tie[tupletIndex]++;
						nTied++;
						i++;
						while(str.at(i)=='~') // we might even have more ties!
						{
							tie[tupletIndex]++;
							nTied++;
							i++;
						}
					}

					notes[tupletIndex] = drumNote;

					nNotes++;
					tupletIndex++;
				}
				/*

				else if(str.at(i)=='~') // tie last note
				{
					tie[tupletIndex] = 1;
					nTied++;
					i++;
				}
				*/
				
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
			
			// push this event to events vector in MData
			dOutput->eventType.push_back(0); // type 0 is 'specify volume'
			dOutput->eventParam.push_back(value);
			dOutput->eventFrame.push_back(framesWritten);
			dOutput->nEvents++;
			
			i++;
		}

		else if(str.at(i)=='^') // Volume increment request
		{
			
			// push this event to events vector in MData
			dOutput->eventType.push_back(1); // event type 1 is 'increment volume'
			dOutput->eventParam.push_back(0);
			dOutput->eventFrame.push_back(framesWritten);
			dOutput->nEvents++;			
			
			i++;
		}

		else if(str.at(i)=='_') // Volume decrement request
		{
			
			// push this event to events vector in MData
			dOutput->eventType.push_back(2); // event type 2 is 'decrement volume'
			dOutput->eventParam.push_back(0);
			dOutput->eventFrame.push_back(framesWritten);
			dOutput->nEvents++;			
			
			i++;
		}
		
		else if(str.at(i)=='(')
		{
			i++;
			
			if(str.substr(i, 10) == "RESETDRUMS")
			{
				// push this event to events vector in DData
				dOutput->eventType.push_back(500); // event type 500 is 'reset drum settings'
				dOutput->eventParam.push_back(0);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 10) == "WHITENOISE")
			{
				// push this event to events vector in DData
				dOutput->eventType.push_back(530); // event type 530 is 'use white noise'
				dOutput->eventParam.push_back(0);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 9) == "PINKNOISE")
			{
				// push this event to events vector in DData
				dOutput->eventType.push_back(531); // event type 531 is 'use pink noise'
				dOutput->eventParam.push_back(0);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 15) == "KICKNOISE=WHITE")
			{
				// push this event to events vector in DData
				dOutput->eventType.push_back(532); // event type 532 is 'set kick's noise type'
				dOutput->eventParam.push_back(0); // 0 for white noise
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 14) == "KICKNOISE=PINK")
			{
				// push this event to events vector in DData
				dOutput->eventType.push_back(532); // event type 532 is 'set kick's noise type'
				dOutput->eventParam.push_back(1); // 1 for pink noise
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 16) == "SNARENOISE=WHITE")
			{
				// push this event to events vector in DData
				dOutput->eventType.push_back(533); // event type 533 is 'set snare's noise type'
				dOutput->eventParam.push_back(0); // 0 for white noise
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 15) == "SNARENOISE=PINK")
			{
				// push this event to events vector in DData
				dOutput->eventType.push_back(533); // event type 533 is 'set snare's noise type'
				dOutput->eventParam.push_back(1); // 1 for pink noise
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 16) == "HIHATNOISE=WHITE")
			{
				// push this event to events vector in DData
				dOutput->eventType.push_back(534); // event type 533 is 'set hihat's noise type'
				dOutput->eventParam.push_back(0); // 0 for white noise
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 15) == "HIHATNOISE=PINK")
			{
				// push this event to events vector in DData
				dOutput->eventType.push_back(534); // event type 533 is 'set hihat's noise type'
				dOutput->eventParam.push_back(1); // 1 for pink noise
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 10) == "KICKPITCH=")
			{
				string strValue = str.substr(i+10,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value
	
				dOutput->eventType.push_back(510);
				dOutput->eventParam.push_back(value);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 11) == "SNAREPITCH=")
			{
				string strValue = str.substr(i+11,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value
	
				dOutput->eventType.push_back(511);
				dOutput->eventParam.push_back(value);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 11) == "HIHATPITCH=")
			{
				string strValue = str.substr(i+11,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value
	
				dOutput->eventType.push_back(512);
				dOutput->eventParam.push_back(value);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 7) == "BEEFUP=")
			{
				string strValue = str.substr(i+7,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value
	
				dOutput->eventType.push_back(520);
				dOutput->eventParam.push_back(value);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 11) == "KICKLENGTH=")
			{
				string strValue = str.substr(i+11,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(400, max(0, value)); // floor + ceil the value
	
				dOutput->eventType.push_back(540); // event no. 540 = KICKLENGTH
				dOutput->eventParam.push_back(value);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 12) == "SNARELENGTH=")
			{
				string strValue = str.substr(i+12,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(1000, max(0, value)); // floor + ceil the value
	
				dOutput->eventType.push_back(541); // event no. 541 = SNARELENGTH
				dOutput->eventParam.push_back(value);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 12) == "HIHATLENGTH=")
			{
				string strValue = str.substr(i+12,4); // get 4 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(1000, max(0, value)); // floor + ceil the value
	
				dOutput->eventType.push_back(542); // event no. 542 = HIHATLENGTH
				dOutput->eventParam.push_back(value);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 12) == "SQUARELEVEL=")
			{
				string strValue = str.substr(i+12,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value
	
				dOutput->eventType.push_back(550); // event no. 550 = SQUARELEVEL
				dOutput->eventParam.push_back(value);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			else if(str.substr(i, 11) == "NOISELEVEL=")
			{
				string strValue = str.substr(i+11,3); // get 3 digits following '='
				int valueDigits = countDigits(strValue);
				strValue = strValue.substr(0, valueDigits);
				int value = atoi(strValue.c_str());
				value = min(100, max(0, value)); // floor + ceil the value
	
				dOutput->eventType.push_back(551); // event no. 550 = NOISELEVEL
				dOutput->eventParam.push_back(value);
				dOutput->eventFrame.push_back(framesWritten);
				dOutput->nEvents++;
			}
			
			cout << "event parsing done! " << str.substr(i,13) << "\n";
			
			// fast forward to where we find the next ')'
			bool fastFwdDone = false;
			while(!fastFwdDone)
			{
				i++;
				if(str.at(i)==')') // stop at where ')' is
					fastFwdDone = true;
				else if(str.at(i)=='$') // we didn't have ')'!!
					fastFwdDone = true;
			}
			
			
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
			dOutput->eventType.push_back(-1);
			dOutput->eventParam.push_back(0);
			dOutput->eventFrame.push_back(framesWritten);
			dOutput->nEvents++;

			// integrity check

			// int r = framesWritten % quarterNoteLength;

			// cout << "End of parsing a channel... num of framesWritten=" << framesWritten << endl;
			// cout << "Dividing by quarter note length, remainder=" << r << endl << endl;

			// write the total frame length written
			dOutput->totalFrames = framesWritten;

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
		else if(str.find("REPEAT=") != string::npos)
		{
			fpos = str.find("REPEAT=");
			string strValue = str.substr(fpos+7,1); // get 1 digit following '='
			int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(9, max(1, value)); // floor + ceil the value

			// set repeat count to the value that was read
			player->disableLooping();
			player->setRepeatsRemaining(value);
			str.erase(fpos, 7+valueDigits);
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
		else if(str.find("DELAYTIME=AUTO3") != string::npos)
		{
			fpos = str.find("DELAYTIME=AUTO3");
			int eraseLen = 15;
			double magicNum = 39999.996; // 333.3333 * 120
			if(str.at(fpos+15)=='L') // if 'DELAYTIME=AUTO3L', set to longer 3-based value
			{
				magicNum = 79999.992; // 666.6666 * 120
				eraseLen++;
			}
			
			int value = magicNum / tpo; // calculate tempo-adjusted delay time -> 60000 / tempo
			value = min(999, max(10, value)); // floor + ceil the value

			// set delay parameters - first delay, delay time, gain (negative for no change)
			player->delay[0].setParameters(value, value, -0.1f); // -> LEFT channel = 0
			player->delay[1].setParameters(value*3/2, value, -0.1f); // -> RIGHT channel = 1
			str.erase(fpos, eraseLen);		
		}
		else if(str.find("DELAYTIME=AUTO") != string::npos)
		{
			fpos = str.find("DELAYTIME=AUTO");
			int value = 60000 / tpo; // calculate tempo-adjusted delay time -> 60000 / tempo
			value = min(999, max(10, value)); // floor + ceil the value

			// set delay parameters - first delay, delay time, gain (negative for no change)
			player->delay[0].setParameters(value, value, -0.1f); // -> LEFT channel = 0
			player->delay[1].setParameters(value*3/2, value, -0.1f); // -> RIGHT channel = 1
			str.erase(fpos, 14);		
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
		string strToReturn = "Load error...\xFF";
		setSource(strToReturn);
		inFile.close();
		return strToReturn;
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
	// get the current time from TIMESTAMP
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
	string strTime = "";
	strTime += toString(now->tm_year + 1900) + "-" + toString(now->tm_mon + 1) + "-" + toString(now->tm_mday) + "-"; 
	strTime += toString(now->tm_hour) + ":" + toString(now->tm_min); 
		 
	std::ofstream ofs;
	ofs.open("_errors_mml.txt", std::ofstream::out | std::ofstream::app);
	ofs << errText1 << errText2 << " (" << strTime << ")" << endl;
	ofs.close();
}

std::string MML::toString(int n)
{
	stringstream ss;
	ss << n;
	return ss.str();
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

#include <sndfile.h>
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

	for(unsigned long ii=0; ii<framesPerBuffer; ii++)
	{
		if(!playing) // if player is not playing or finished playing, just pass 0
		{
			soundAmplitudeLeft = 0.0f;
			soundAmplitudeRight = 0.0f;
			
			// place holder for BCPlayer...
			// in BCPlayer, if music is stopped, you get sound effects only :)
			soundAmplitudeLeft = sfx->getOutput(0);
			soundAmplitudeRight = sfx->getOutput(1);
			
			// soundAmplitudeLeft = sfx->getOutput(0);
			// soundAmplitudeRight = sfx->getOutput(1);
			
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
					
					// if there are event requests, digest those first
					bool eventsDone = false;
					
					while(!eventsDone)
					{
						// if next event in vector is set to happen at this frame pos, process
						if( (data[i].eventFrame[eventIndex[i]] <= framePos) && (eventIndex[i] < data[i].nEvents) )
						{
							processEvent(i, data[i].eventType[eventIndex[i]], 
											data[i].eventParam[eventIndex[i]]);
							eventIndex[i]++;
						}
						else
							eventsDone = true;
					}					
					
					remainingFrames[i]--;
					if(remainingFrames[i] <= 0)
					{
						noteIndex[i]++;

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
				// if there are event requests, digest those first
				bool eventsDone = false;

				while(!eventsDone)
				{
					// if next event in vector is set to happen at this frame pos, process
					if( (ddata.eventFrame[dEventIndex] <= framePos) && (dEventIndex < ddata.nEvents) )
					{
						// cout << "event found! for drums" << endl;
						processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
						dEventIndex++;
					}
					else
						eventsDone = true;
				}
				
				dRemainingFrames--;
				if(dRemainingFrames <= 0)
				{
					dNoteIndex++; // move onto the next drum note index

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
				// final point check!
				// ... if there are events to process at this final moment... process them here
				
				for(int i=0;i<9;i++)
				{	
					bool eventsDone = false;
					while(!eventsDone)
					{
						// if next event in vector is set to happen at this frame pos, process
						if( (eventIndex[i] < data[i].nEvents) )
						{
							processEvent(i, data[i].eventType[eventIndex[i]], data[i].eventParam[eventIndex[i]]);
							eventIndex[i]++;
						}
						else
							eventsDone = true;
					}
				}
				
				// process drum events pending at the final point before loop
				bool eventsDone = false;
				while(!eventsDone)
				{
					// if next event in vector is set to happen at this frame pos, process
					if( (dEventIndex < ddata.nEvents) )
					{
						processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
						dEventIndex++;
					}
					else
						eventsDone = true;
				}
				
				if(loopEnabled)
				{
					cout << "Looping back to beginning...\n";
					// enable channels again
					enableChannels(true, true, true, true, true, true, true, true, true, true);

					// enable drum channel
					enableDrumChannel();

					// go back to the beginning
					goToBeginning();
				}
				else if(!loopEnabled && repeatsRemaining > 1) // if repeat times is left.. process
																// when set to 1, it's last time
				{
					repeatsRemaining--;
					cout << "Back to beginning... repeats remaining = " << repeatsRemaining << endl;
					
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

// portaudio stream has been dropped! ... attempt to reopen stream...
void MPlayer::playerStoppedCallback ()
{
	cout << "Stream finished callback called! step 2 :)\n";
	if(!appIsExiting)
		restartStream();
}

////////////////////////////////////////////////////////

MPlayer::MPlayer()
{
	appIsExiting = false; // when this is true, paStreamFinishedCallback will NOT automatically reopen stream
	
	for(int i=0;i<9;i++)
		silenced[i] = false;

	dSilenced = false;
	enableChannels(true, true, true, true, true, true, true, true, true, true);
	enableDrumChannel();

	for(int i=0;i<9;i++)
		osc[0].detune = 0;

	tableType = 1; // OSC's default type - square table
	masterGain = 0.7f;
	masterOutCap = 0.88f;

	compThreshold = 0.5f;
	compRatio = 8;

	framePos = 0;
	songLastFrame = 0;
	songLastFramePure = 0;
	bookmark = 0;
	for(int i=0; i<9; i++)
		eventIndex[i] = 0;
	dEventIndex = 0;

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
	
	// set up callback to be called in case stream gets intrrupted
	err = Pa_SetStreamFinishedCallback( &stream, paStoppedCallback );

	// start port audiostream
    err = Pa_StartStream( stream );
}

// this method should be used if portaudio drops off and stops its stream
void MPlayer::restartStream()
{
	if(appIsExiting)
	{
		cout << "pa stream restart requested, but app is exiting...\n(will not restart audio stream)\n";
		return;
	}
	
	// specifically stop stream first (as user guide recommends it)
	err = Pa_StopStream( stream );
	
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
	
	err = Pa_StartStream( stream );
	if( err != paNoError ) handlePaError( err );
	else
		cout << "portaudio stream restarted - success!\n";
}

// DEBUG
void MPlayer::stopStream()
{
	err = Pa_StopStream( stream );
	cout << "requesting portaudio to stop stream...\n";
}

// 
void MPlayer::declareAppTermination()
	{ appIsExiting = true; }

// utility function - query portaudio stream state
std::string MPlayer::getStreamStateString()
{
	err = Pa_IsStreamStopped( stream );
	std::string strError = Pa_GetErrorText(err);
	return strError;
}

// return portaudio stream state in boolean (success is true)
bool MPlayer::getStreamState()
{
	err = Pa_IsStreamStopped( stream );
	if(err==0)
		return true;
	else
		return false;
}
	
// function to set back to default before loading new song (or 'play' current song again)
// reset all oscillator + delay settings etc. - before parsing source
void MPlayer::resetForNewSong()
{
	// loop is enabled by default
	loopEnabled = true;
	
	// repeat counts - Note: repeating is OFF if loop is enabled
	repeatsRemaining = 1;
	
	// reset frame info for now (in case you're starting a new empty file)
	songLastFrame = 0;
	songLastFramePure = 0;
	songFinished = true;
	
	// initialize oscillators for first note (so that if starting song with rest, OSC is muted)
	//for(int i=0; i<9; i++)
	//	osc[i].initializeForFirstNote();

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
		osc[i].setTable(1); // back to square wave
		osc[i].refreshForSongBeginning();
		osc[i].setEnvelope(22, 18, 250, 40, 0.9f, 0.5f);
		enableChannel(i);
		disableAstro(i);
		osc[i].disableLFO();
		osc[i].initializeLFO(); // resets lfo parameters
		osc[i].detune = 0;
		osc[i].setRiseToDefault();
		osc[i].setFallToDefault();
		osc[i].disableBeefUp();
		osc[i].setBeefUpFactor(1.0f);
		ringModEnabled[i] = false; // start with ring modulation cleared
		ringModFeed[i] = -1; // -1 means no feeder channel
		ringModMute[i] = false;
		osc[i].resetYFlip();
	}
	enableDrumChannel();
	nosc.resetDrumTones();
	nosc.disableBeefUp();
	nosc.setBeefUpFactor(1.0f);
	// nosc.useWhiteNoise();

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
		eventIndex[i] = 0;
	}

	// for drum channel
	dChannelDone = false;
	dRemainingFrames = 0;
	currentDrumNote = 0;
	dNoteIndex = 0;
	dEventIndex = 0;
	
	// set the starting note for each music channel (ch 1 to 9)
	for(int i=0; i<9; i++)
	{
		// if there are event requests (freq >= 70000), digest those first
		bool eventsDone = false;
		
		while(!eventsDone)
		{
			// if next event in vector is set to happen at this frame pos, process
			if( (data[i].eventFrame[eventIndex[i]] == 0) && (eventIndex[i] < data[i].nEvents) )
			{
				processEvent(i, data[i].eventType[eventIndex[i]], 
								data[i].eventParam[eventIndex[i]]);
				eventIndex[i]++;
			}
			else
				eventsDone = true;
		}

		remainingFrames[i] = data[i].len[noteIndex[i]];
		freqNote[i] = data[i].freqNote[noteIndex[i]];

		// if very first note is a rest (freq = 65535), silence channel
		if(freqNote[i]==65535.0)
		{
			setToRest(i); // set this channel to rest
			osc[i].confirmFirstNoteIsRest();
		}
		else if(freqNote[i]<0) // if first note is already end flag (empty MML data)
		{
			channelDone[i] = true;
			disableChannel(i);
		}
		else // otherwise, we have a legit note! go ahead and set to play that note
			setNewNote(i, freqNote[i]);
	}

	// handle drum channel!
	//
	// set starting note for drum channel

	// if there are event requests (freq >= 70000), digest those first
	bool eventsDone = false;
	
	while(!eventsDone)
	{
		// if next event in vector is set to happen at this frame pos, process
		if( (ddata.eventFrame[dEventIndex] == 0) && (dEventIndex < ddata.nEvents) )
		{
			processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
			dEventIndex++;
		}
		else
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

// returns the current framePos (where player's position in music)
long MPlayer::getFramePos()
	{ return framePos; }

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
	
void MPlayer::enableRingMod(int channel, int modulatorChannel)
{
	ringModEnabled[channel] = true; // turn on ring modulation for this channel
	ringModFeed[channel] = modulatorChannel; // assign the modulator/feeder channel
	ringModMute[modulatorChannel] = true; // feeder channel should be muted in main mix
}

void MPlayer::disableRingMod(int channel)
{
	bool found = false;
	int searchChannel = ringModFeed[channel]; // do search for - currently assigned modulator for this channel
	ringModFeed[channel] = -1; // -1 to clear target for this channel to begin...
	int channelToCheck = ringModFeed[channel]; // check for the assigned modulator channel
												// if it's not by any other channel, then we can REVIVE the modulator
	for(int i=0; i<9; i++)
	{
		if( ringModFeed[i] == searchChannel )
			found = true;
	}
	if(!found) // if no other channel has to keep using this modulator that has been used by this channel
		ringModMute[searchChannel] = false; // revive this feeder channel - comes alive in mix again!
	ringModEnabled[channel] = false; // then disable ring modulation for this channel
}

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
	
void MPlayer::setRepeatsRemaining(int value)
	{ repeatsRemaining = value; }
	
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
		if(enabled[i] && silenced[i] == false && !ringModEnabled[i] && !ringModMute[i])
			mix += compress(osc[i].getOutput());
		else if(ringModEnabled[i] && enabled[i] && silenced[i] == false && ringModFeed[i]!=-1)
			mix += compress(osc[i].getOutput() * osc[ringModFeed[i]].getOutput());
	}

	// mix drum channel, too
	if(dEnabled && dSilenced == false)
		mix += compress(nosc.getOutput());

	// update delay - delay output is returned - so add to mix
	if(delayEnabled)
		mix += delay[channel].update(mix);

	// apply master gain and compress
	mix = compress(mix * masterGain);
	
	// place holder for BCPlayer...
	// in BCPlayer, add sound effects on top of music!
	mix += sfx->getOutput(channel);
	
	// mix += sfx->getOutput(channel); // add the entire SFX mix
										// for BCPlayer only!	

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


// process one event at current frame for a specified channel
void MPlayer::processEvent(int channel, int eType, int eParam)
{
	// cout << "Event: ch=" << channel << " type=" << eType << " param=" << eParam << "\t";

	// -1 is end of event series for the channel
	if(eType==-1)
	{
		// cout << "Channel " << channel << ": end of events!\n";
	}
	
	// type 0 - specify the volume
	else if(eType==0)
	{
		// convert the passed value(1-10) to float (0.0 to 0.5f)
		float gainToSet
			= static_cast<float>(eParam) / 20.0f;
		setChannelGain(channel, gainToSet);
		// cout << "VOLUME=" << eParam << endl;
	}
	// type 1 - increment volume
	else if(eType==1)
	{
		float gainToSet = min(0.5f, getChannelGain(channel)+0.05f);
		setChannelGain(channel, gainToSet);		
		cout << "channel " << channel << " VOLUME++ new gain=" << gainToSet << "\n";
	}
	// type 2 - decrement volume
	else if(eType==2)
	{
		float gainToSet = max(0.001f, getChannelGain(channel)-0.05f);
		setChannelGain(channel, gainToSet);
		cout << "channel " << channel << " VOLUME-- new gain=" << gainToSet << "\n";
	}
	// type 10 - set waveform 
	else if(eType==10)
	{
		osc[channel].setTable(eParam); // set wavetable for this value
		// cout << "WAVEFORM=" << eParam << endl;
	}
	// type 11 - flip waveform vertically (helpful for pulse waves etc.)
	else if(eType==11)
	{
		osc[channel].flipYAxis(); // set flipping status to INVERTED
		cout << "channel " << channel << " - WAVEFLIP" << endl;
	}
	// type 1000 - "DEFAULTTONE"
	else if(eType==1000)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 0, 0, 0, 0.65f, 0.65f);
		// cout << "DEFAULTTONE\n";
	}
	// type 1001 - "PRESET=BEEP"
	else if(eType==1001)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 0, 0, 0, 0.65f, 0.65f);
		// cout << "PRESET=BEEP\n";
	}
	// type 1002 - "PRESET=POPPY"
	else if(eType==1002)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 50, 10, 50, 0.90f, 0.40f);
		// cout << "PRESET=POPPY\n";
	}
	// type 1002 - "PRESET=POPPYVIB"
	else if(eType==1003)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 50, 10, 50, 0.90f, 0.40f);
		osc[channel].enableLFO();
		osc[channel].setLFOrange(22);
		osc[channel].setLFOwaitTime(250);
		osc[channel].setLFOspeed(6.0);
		// cout << "PRESET=POPPYVIB\n";
	}
	// type 1003 - "PRESET=BELL"
	else if(eType==1004)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 0, 800, 0, 0.80f, 0.0f);
		// cout << "PRESET=BELL\n";
	}
	// type 20 - "ATTACKTIME="
	else if(eType==20)
	{
		osc[channel].setAttackTime(eParam); // set attack time to this value
		// cout << "ATTACKTIME=" << eParam << endl;
	}
	// type 21 - "PEAKTIME="
	else if(eType==21)
	{
		osc[channel].setPeakTime(eParam); // set peak time to this value
		// cout << "PEAKTIME=" << eParam << endl;
	}
	// type 22 - "DECAYTIME="
	else if(eType==22)
	{
		osc[channel].setDecayTime(eParam); // set decay time to this value
		// cout << "DECAYTIME=" << eParam << endl;
	}
	// type 23 - "RELEASETIME="
	else if(eType==23)
	{
		osc[channel].setReleaseTime(eParam); // set release time to this value
		// cout << "RELEASETIME=" << eParam << endl;
	}
	// type 24 - "PEAKLEVEL="
	else if(eType==24)
	{
		float valuef = static_cast<float>(eParam) / 100.0f;
		osc[channel].setPeakLevel(valuef); // set peak level to this value
		// cout << "PEAKLEVEL=" << eParam << endl; 
	}
	// type 25 - "SUSTAINLEVEL="
	else if(eType==25)
	{
		float valuef = static_cast<float>(eParam) / 100.0f;
		osc[channel].setSustainLevel(valuef); // set sustain level to this value
		// cout << "SUSTAINLEVEL=" << eParam << endl;
	}
	// type 30 - "LFO=ON/OFF"
	else if(eType==30)
	{
		if(eParam==1)
		{
			osc[channel].enableLFO();
			// cout << "LFO=ON\n";
		}
		else if(eParam==0)
		{
			osc[channel].disableLFO();
			// cout << "LFO=ON\n";
		}
	}
	// type 31 - "LFORANGE="
	else if(eType==31)
	{
		osc[channel].setLFOrange(eParam); // set LFO range to this value
		// cout << "LFORANGE=" << eParam << endl;
	}
	// type 32 - "LFOSPEED="
	else if(eType==32)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setLFOspeed(valued); // set LFO speed to this value
		// cout << "LFOSPEED=" << eParam << endl;
	}
	// type 33 - "LFOWAIT="
	else if(eType==33)
	{
		osc[channel].setLFOwaitTime(eParam); // set LFO wait time to this value
		// cout << "LFOWAIT=" << eParam << endl;
	}
	// type 40 - "ASTRO="
	else if(eType==40)
	{
		if(eParam==0)
		{
			disableAstro(channel);
		}
		else
			setAstro(channel, eParam); // set astro to this value
		// cout << "ASTRO=" << eParam << endl;
	}
	// type 41 - "ASTRO=OFF"
	else if(eType==41)
	{
		disableAstro(channel);
		// cout << "ASTRO=OFF" << endl;
	}
	// type 50 - "FALL"
	else if(eType==50)
	{
		osc[channel].startFall();
		// cout << "FALL!" << endl;
	}
	// type 51 - "FALLSPEED="
	else if(eType==51)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setFallSpeed(valued); // set LFO speed to this value
		// cout << "FALLSPEED=" << eParam << endl;
	}
	// type 52 - "FALLWAIT="
	else if(eType==52)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setFallWait(valued); // set LFO speed to this value
		// cout << "FALLWAIT=" << eParam << endl;
	}
	// type 60 - "RISE"
	else if(eType==60)
	{
		osc[channel].startRise();
		// cout << "RISE!" << endl;
	}
	// type 61 - "RISESPEED="
	else if(eType==61)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setRiseSpeed(valued); // set LFO speed to this value
		// cout << "RISESPEED=" << eParam << endl;
	}
	// type 62 - "RISERANGE="
	else if(eType==62)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setRiseRange(valued); // set LFO speed to this value
		// cout << "RISERANGE=" << eParam << endl;
	}
	// type 70 - "BEEFUP="
	else if(eType==70)
	{
		cout << "BEEFUP=" << eParam << endl;
		double valuef = static_cast<float>(eParam);
		if(eParam<0.1f) // if zero, turn off beef-up
		{
			osc[channel].disableBeefUp();
			cout << "BEEFUP disabled!\n";
		}
		else
		{
			osc[channel].enableBeefUp();
			osc[channel].setBeefUpFactor( (valuef*3.0f/100.0f) + 1.0f ); // set BEEFUP to this value
		}
	}
	// type 80 - "RINGMOD="
	else if(eType==80)
	{
		cout << "channel " << channel << " - RINGMOD = " << eParam << endl;
		if(eParam==0) // if 0 is passed, turn OFF
		{
			cout << "RINGMOD=OFF, channel = " << channel << endl;
			disableRingMod(channel); // stop ring modulation for this channel now
		}
		else if(eParam >= 1) // if 1 or bigger is passed, assign ring modulator channel
			enableRingMod(channel, eParam-1); // start ring modulation for this channel now
											// the passed parameter is the modulator/feeder channel number
		
	}
	// type 81 - "RINGMOD=OFF"
	else if(eType==81)
	{
		cout << "RINGMOD=OFF, channel = " << channel << endl;
		disableRingMod(channel); // stop ring modulation for this channel now
	}
}


// process one event at current frame for a specified channel
void MPlayer::processDrumEvent(int eType, int eParam)
{
	// cout << "Event: Drums type=" << eType << "\t";

	// -1 is end of event series for the channel
	if(eType==-1)
	{
		// cout << "Drum channel : end of events!\n";
	}
	
	// type 0 - specify the volume
	else if(eType==0)
	{
		// convert the passed value(1-10) to float (0.0 to 0.5f)
		float gainToSet
			= static_cast<float>(eParam) / 20.0f;
		setDChannelGain(gainToSet);	
		// cout << "VOLUME=" << eParam << endl;
	}
	// type 1 - increment volume
	else if(eType==1)
	{
		float gainToSet = min(0.5f, getDChannelGain()+0.05f);
		setDChannelGain(gainToSet);		
		// cout << "VOLUME++\n";
	}
	// type 2 - decrement volume
	else if(eType==2)
	{
		float gainToSet = max(0.001f, getDChannelGain()-0.05f);
		setDChannelGain(gainToSet);
		// cout << "VOLUME--\n";
	}
	// type 500 - reset all drum settings
	else if(eType==500)
	{
		nosc.resetDrumTones();
		cout << "RESETDRUMS\n";
	}
	// type 510 - tune kick
	// passed value = 0 to 100 (scale), set to freq ranging between 50 and 350hz
	else if(eType==510)
	{
		double tuneFreq = (static_cast<double>(eParam) / 100.0) * 300.0 + 50.0;
		nosc.tuneKick(tuneFreq);
		cout << "KICKPITCH - " << eParam << "%, " << tuneFreq << "hz\n";
	}
	// type 511 - tune snare
	// passed value = 0 to 100 (scale), set to freq ranging between 200 and 1240hz
	else if(eType==511)
	{
		double tuneFreq = (static_cast<double>(eParam) / 100.0) * 1040.0 + 200.0;
		nosc.tuneSnare(tuneFreq);
		cout << "SNAREPITCH - " << eParam << "%, " << tuneFreq << "hz\n";
	}
	// type 512 - tune HiHat
	// passed value = 0 to 100 (scale), set to freq ranging between 1200 and 3600hz
	else if(eType==512)
	{
		double tuneFreq = (static_cast<double>(eParam) / 100.0) * 2400.0 + 1200.0;
		nosc.tuneHiHat(tuneFreq);
		cout << "HIHATPITCH - " << eParam << "%, " << tuneFreq << "hz\n";
	}
	// type 520 - "BEEFUP="
	else if(eType==520)
	{
		cout << "Drum BEEFUP=" << eParam << endl;
		double valuef = static_cast<float>(eParam);
		if(eParam<0.1f) // if zero, turn off beef-up
		{
			nosc.disableBeefUp();
			cout << "Drum - BEEFUP disabled!\n";
		}
		else
		{
			nosc.enableBeefUp();
			nosc.setBeefUpFactor( (valuef*1.6f/100.0f) + 1.0f ); // set BEEFUP to this value
		}
	}
	// type 530 - use white noise
	else if(eType==530)
	{
		nosc.useWhiteNoise();
		cout << "WHITENOISE - All drums\n";
	}
	// type 531 - use pink noise
	else if(eType==531)
	{
		nosc.usePinkNoise();
		cout << "PINKNOISE - All drums\n";
	}
	// type 532 - set kick noise type
	else if(eType==532)
	{
		nosc.setKickNoiseType(eParam);
		cout << "set kick noise type =" << eParam << "\n";
	}
	// type 533 - set snare noise type
	else if(eType==533)
	{
		nosc.setSnareNoiseType(eParam);
		cout << "set snare noise type =" << eParam << "\n";
	}
	// type 534 - set hihat noise type
	else if(eType==534)
	{
		nosc.setHiHatNoiseType(eParam);
		cout << "set hihat noise type =" << eParam << "\n";
	}
	// type 540 - set kick length
	// passed value = 0 to 400 (milliseconds), set to int value ranging between 0 and 400 msec
	else if(eType==540)
	{
		nosc.setKickLength(eParam);
		cout << "KICKLENGTH - " << eParam << "msec\n";
	}
	// type 541 - set snare length
	// passed value = 0 to 1000 (milliseconds), set to int value ranging between 0 and 1000 msec
	else if(eType==541)
	{
		nosc.setSnareLength(eParam);
		cout << "SNARELENGTH - " << eParam << "msec\n";
	}
	// type 542 - set hihat length
	// passed value = 0 to 1000 (milliseconds), set to int value ranging between 0 and 1000 msec
	else if(eType==542)
	{
		nosc.setHiHatLength(eParam);
		cout << "HIHATLENGTH - " << eParam << "msec\n";
	}
	// type 550 - set square wave element mix level (default is 100 = 1.0f)
	// passed value = 0 to 100 (percent), set to float value ranging between 0 and 1.0f
	else if(eType==550)
	{
		float valuef = static_cast<float>(eParam) / 100.0f;
		nosc.setSquareLevel(valuef);
		cout << "SQUARELEVEL - " << eParam << "%, passing value = " << valuef << "\n";
	}
	// type 551 - set noise element mix level (default is 100 = 1.0f)
	// passed value = 0 to 100 (percent), set to float value ranging between 0 and 1.0f
	else if(eType==551)
	{
		float valuef = static_cast<float>(eParam) / 100.0f;
		nosc.setNoiseLevel(valuef);
		cout << "NOISELEVEL - " << eParam << "%, passing value = " << valuef << "\n";
	}
}

	


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
			if(framePos >= songFrameLen)
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
			int framesWrittenFile = sf_writef_float(sndFile, sndBuffer, writeChunkSize);

			// currentFrame += nFramesWritten; // update current position
			if(framePos >= songFrameLen) // reached end...
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
				
				// if there are event requests, digest those first
				bool eventsDone = false;
				
				while(!eventsDone)
				{
					// if next event in vector is set to happen at this frame pos, process
					if( (data[i].eventFrame[eventIndex[i]] <= framePos) && (eventIndex[i] < data[i].nEvents) )
					{
						processEvent(i, data[i].eventType[eventIndex[i]], 
										data[i].eventParam[eventIndex[i]]);
						eventIndex[i]++;
					}
					else
						eventsDone = true;
				}	
				
				remainingFrames[i]--;
				if(remainingFrames[i] <= 0)
				{
					noteIndex[i]++;

					// cout << "Frame " << framePos << " channel " << i << " - note index = " <<  noteIndex[i];

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
			
			// if there are event requests, digest those first
			bool eventsDone = false;
			
			while(!eventsDone)
			{
				// if next event in vector is set to happen at this frame pos, process
				if( (ddata.eventFrame[dEventIndex] <= framePos) && (dEventIndex < ddata.nEvents) )
				{
					processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
					dEventIndex++;
				}
				else
					eventsDone = true;
			}
			
			dRemainingFrames--;
			if(dRemainingFrames <= 0)
			{
				dNoteIndex++; // move onto the next drum note index

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
			
			// final point check!
			// ... if there are events to process at this final moment... process them here
			
			for(int i=0;i<9;i++)
			{
				bool eventsDone = false;
				while(!eventsDone)
				{
					// if next event in vector is set to happen at this frame pos, process
					if( (eventIndex[i] < data[i].nEvents) )
					{
						processEvent(i, data[i].eventType[eventIndex[i]], data[i].eventParam[eventIndex[i]]);
						eventIndex[i]++;
					}
					else
						eventsDone = true;
				}
			}
			
			// process drum events pending at the final point before loop
			bool eventsDone = false;
			while(!eventsDone)
			{
				// if next event in vector is set to happen at this frame pos, process
				if( (dEventIndex < ddata.nEvents) )
				{
					processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
					dEventIndex++;
				}
				else
					eventsDone = true;
			}			
			
			
			if(repeatsRemaining > 1) // if repeat times is left.. process
															// when set to 1, it's last time
			{
				repeatsRemaining--;
				cout << "Back to beginning... repeats remaining = " << repeatsRemaining << endl;
				
				// enable channels again
				enableChannels(true, true, true, true, true, true, true, true, true, true);

				// enable drum channel
				enableDrumChannel();

				// go back to the beginning
				goToBeginning();					
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
			
			// first process all the events up to destination
			bool eventsZappingDone = false;
			while(!eventsZappingDone)
			{
				// if next event in vector is set to happen at this frame pos, process
				if( (data[i].eventFrame[eventIndex[i]] <= destination) && (eventIndex[i] < data[i].nEvents) )
				{
					processEvent(i, data[i].eventType[eventIndex[i]], 
									data[i].eventParam[eventIndex[i]]);
					eventIndex[i]++;
				}
				else
					eventsZappingDone = true;
			}
			
			bool zappingDone = false;
			
			while(!zappingDone)
			{

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
			
			// first process all the events up to destination
			bool eventsZappingDone = false;

			while(!eventsZappingDone)
			{
				// if next event in vector is set to happen at this frame pos, process
				if( (ddata.eventFrame[dEventIndex] <= destination) && (dEventIndex < ddata.nEvents) )
				{
					processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
					dEventIndex++;
				}
				else
					eventsZappingDone = true;
			}
			
			// for each music channel...
			// zap through until very last note before the seekpoint
			// including events
			
			bool zappingDone = false;
			
			while(!zappingDone)
			{

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
					
					long nextDNoteLen = static_cast<long>( ddata.len[dNoteIndex] );
					
					// if adding next note will cause to go past destination, stop here
					if( ( seekPos + nextDNoteLen ) >= destination )
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




