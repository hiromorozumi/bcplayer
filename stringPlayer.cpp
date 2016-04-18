#include <iostream>
#include <string>

#include "BC/BCPlayer.h"

using namespace std;

int main(){
	
	BCPlayer bcplayer;
	system("cls");
	string command = "";
	
	while(command!="QUIT")
	{
		cout << "\nType a string for BCPlayer to play (QUIT to quit):\n";
		getline(cin, command);
		if(command!="QUIT")
		{
			bcplayer.loadString(command);
			bcplayer.startMusic();
		}
	}
	
	bcplayer.terminate();
	cout << "Okay, quitting... Bye :)\n";
	return 0;
}