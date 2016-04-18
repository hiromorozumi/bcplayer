//////////////////////////////////////////////////////
// SFX class /////////////////////////////////////////

#ifndef SFX_H
#define SFX_H

#include <string>
#include "Sound.h"

class Sound;

class SFX
{
public:

	static const int N_SLOTS = 16;
	Sound sound[N_SLOTS];
	float compThreshold;
	float compRatio;
	
	SFX();
	~SFX(){}
	
	std::string getErrorText(int slot);
	bool loadSound(int slot, const std::string &filename);
	float getGain(int slot);
	void setGain(int slot, float g);
	float getPanning(int slot);
	void setPanning(int slot, float p);
	void start(int slot);
	void stop(int slot);
	void pause(int slot);
	void resume(int slot);
	float getOutput(int channel);
	float compress(float input);

};

#endif
