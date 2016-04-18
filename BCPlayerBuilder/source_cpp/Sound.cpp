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