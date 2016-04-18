#include <iostream>
#include <string>
#include <windows.h>
#include "BC/BCPlayer.h"

using namespace std;

int main(){
	
	// always instantiate BCPlayer object first
	BCPlayer bcplayer;
	system("cls");

	cout << "********** BCPlayer SFX - Music Mix test ***********\n\n";
	cout << "BCPlayer can load WAV/OGG files and play sound effects.\n";
	cout << "The SFX engine has 16 different independent slots for sound effects.\n";
	cout << "So different sound effects can overlap in time.\n";
	cout << "And in the background, you can play your BeepComp track at the same time.\n\n";
	cout << "[F1]    ..... Sound effect 1\n";
	cout << "[F2]    ..... Sound effect 2\n";
	cout << "[F3]    ..... Sound effect 3\n";
	cout << "[SPACE] ..... Toggle background music\n";
	cout << "[ESC]   ..... Quit this demo\n";
	
	// load up sounds... and set stereo panning for each
	// note that WAV files need to be 16-bit in bit depth
	// they can be mono or stereo (will be automatically detected)
	bcplayer.loadSFX(1, "audio/sound1.wav");
	bcplayer.setSFXPanning(1, 0);
	bcplayer.loadSFX(2, "audio/sound2.wav");
	bcplayer.setSFXPanning(2, 50);
	bcplayer.loadSFX(3, "audio/sound3.wav");
	bcplayer.setSFXPanning(3, 100);
	
	bool done = false;
	int songIndex = 0;
	
	// look in this music source folder... try replacing these files with your own - have fun! :)
	string songName[] = {"bcsource/song1.txt", "bcsource/song2.txt", "bcsource/song3.txt", "bcsource/song4.txt"};

	// let's begin by playing our first song
	// the music could be maybe a little quieter than usual
	// so that sound effects can cut through the mix
	bcplayer.loadMusic(songName[songIndex]);
	bcplayer.setMusicVolume(70);
	bcplayer.startMusic();
	
	while(!done)
	{
		
		// F1 - F3 keys play the sound effects
		if(GetAsyncKeyState(VK_F1))
		{
			bcplayer.startSFX(1);
			while(GetAsyncKeyState(VK_F1)){}
		}
		if(GetAsyncKeyState(VK_F2))
		{
			bcplayer.startSFX(2);
			while(GetAsyncKeyState(VK_F2)){}
		}
		if(GetAsyncKeyState(VK_F3))
		{
			bcplayer.startSFX(3);
			while(GetAsyncKeyState(VK_F3)){}
		}
		
		// space key to toggle songs to play
		if(GetAsyncKeyState(VK_SPACE))
		{
			songIndex++;
			if(songIndex==4)
				songIndex = 0;
			bcplayer.pauseMusic();
			bcplayer.loadMusic(songName[songIndex]);
			bcplayer.setMusicVolume(70);
			bcplayer.startMusic();
			while(GetAsyncKeyState(VK_SPACE)){}	
		}

		// escape key to exit
		if(GetAsyncKeyState(VK_ESCAPE))
			done = true;
	}
	
	// finally close BCPlayer in a respectful manner
	bcplayer.terminate();
	cout << "Okay, quitting... Bye :)\n";
	return 0;
}