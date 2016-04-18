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