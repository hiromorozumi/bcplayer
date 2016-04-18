//
//	BCPlayer v0.1.5 tiny demo
//
//  BCPlayer is BeepComp module you can use for your game development
//  You can use BCPlayer to play your BeepComp source text files
//	(without the need to export to audio files)
//
//	This program demonstrates how you can incorporate BCPlayer
//	to your game project...
//

#include <ctime>
#include <cstdlib>
#include <iostream>

// be sure to include "BC/BCPlayer.h"
// (and compile with BCPlayer.cpp as well)

#include "BC/BCPlayer.h"

using namespace std;

int main()
{	
	// first create a BCPlayer object
	BCPlayer bcplayer;
	
	system("cls");
		
	bool result = true;
	
	// load the song 1... use std::string for the file name
	// the result is a boolean (success/fail)
	string filePath = "bcsource/main_song.txt";
	result = bcplayer.loadMusic(filePath);
	
	if(result==false)
	{
		// loading caused an error!
		cout << "Error loading the main song...\n";
		return -1;
	}
	
	// start playing song 1
	bcplayer.startMusic();
	
	bool gameOn = true;
	int seed = time(NULL);
	srand(seed);
	int number = rand() % 99;
	int guess;
	int count = 0;
	
	// silly guess-a-number game loop...
	while(gameOn)
	{
		cout << "Guess a number (0 to 99): ";
		cin >> guess;		
		count++;
		
		if(number==guess)
			gameOn = false;
		else if(number > guess)
			cout << "Too small!\n";
		else if(number < guess)
			cout << "Too big!\n";
	}
	
	// you guessed the number right..
	cout << "You guessed it right! :)\n";
	cout << "...after trying " << count << " times.\n\n";
	
	// you should pause before loading a new song
	bcplayer.pauseMusic();
	
	// load song 2...
	filePath = "bcsource/game_over.txt";
	result = bcplayer.loadMusic(filePath);
	
	if(!result)
	{
		cout << "Error loading the game over music...\n";
		return -1;
	}
	
	// disable looping , and start song 1
	bcplayer.disableLooping();
	bcplayer.startMusic();
	
	// wait until the song finishes playing
	while(!bcplayer.musicFinished())
	{}

	cout << "Music finished! ..bye :)\n";
	
	// close BCPlayer the right way
	bcplayer.terminate();
	
	return 0;
}