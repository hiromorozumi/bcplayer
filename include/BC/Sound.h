////////////////////////////////////////////////////////
// Sound class /////////////////////////////////////////

#ifndef SOUND_H
#define SOUND_H

#include <string>
#include <vector>

class Sound
{
	
public:	
	
	static const int MAX_SECONDS = 4;
	static const int BLOCK_SIZE = 4096;
	
	// holds the actual sound data
	std::vector<float> leftData;
	std::vector<float> rightData;
	int dataSize;
	
	// use to read from file
	float readBuffer[8300]; // if stereo, 4092 frames each
	
	std::string error;
	bool stereo;
	int pos;
	bool playing;
	float gain;
	float panning;
	float rightGain;
	float leftGain;
	
	Sound();
	~Sound(){}

	bool isStereo();
	std::string getErrorText();
	bool loadFile(const std::string &filename);
	void setGain(float g);
	float getGain();
	void setPanning(float p);
	float getPanning();
	void refresh();
	void start();
	void stop();
	void pause();
	void resume();
	float update(int channel);
};

#endif