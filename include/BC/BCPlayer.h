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
	SFX sfx;

	BCPlayer();
	~BCPlayer(){}
	
	bool initialize();
	void terminate();
	void resetAudioDevice();
	bool loadMusic(const std::string &fileName);
	std::string loadFileToString(const std::string &filename);
	void loadString(const std::string &source);
	void startMusic();
	void stopMusic();
	void pauseMusic();
	void restartMusic();
	void enableLooping();
	void disableLooping();
	bool musicFinished();
	void setMusicVolume(float percent);
	float getMusicVolume();
	void seek(float percent);
	
	std::string loadSFX(int slot, std::string filename);
	void setSFXVolume(int slot, int volumePercent);
	int getSFXVolume(int slot);
	void setSFXPanning(int slot, int panningPercent);
	int getSFXPanning(int slot);
	void startSFX(int slot);
	void stopSFX(int slot);
	void pauseSFX(int slot);
	void resumeSFX(int slot);
	
};

#endif