///////////////////////////////////////////////////////
// BCPlayer class /////////////////////////////////////

#ifndef BCPLAYER_H
#define BCPLAYER_H

#include <string>
#include "BC/MPlayer.h"
#include "BC/MML.h"

class MPlayer;
class MML;

class BCPlayer
{
public:

	MPlayer mplayer;
	MML mml;

	BCPlayer();
	~BCPlayer(){}
	
	bool initialize();
	void terminate();
	void resetAudioDevice();
	bool loadFile(std::string &fileName);
	std::string loadFileToString(std::string &filename);
	void loadString(std::string &source);
	void start();
	void stop();
	void pause();
	void restart();
	void enableLooping();
	void disableLooping();
	bool finishedPlaying();
	void setVolumePercent(float percent);
	float getVolumePercent();
	void seek(float percent);
};

#endif