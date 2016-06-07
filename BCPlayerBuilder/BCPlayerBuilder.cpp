// BCPlayer builder //////////////////////////////////////////////
// use this program to make necessary changes to BeepComp ////////
// source files to build a monolithic BCPlayer.cpp ///////////////

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <windows.h>

using namespace std;

std::string str;
vector<string> sourceCPP;

int main()
{
	system("cls");
	
	str = "";

	// first
	// add all the cpp files to be included
	// in the right order... (MPlayer will be added last)
	sourceCPP.push_back("source_cpp/BCPlayerBase.cpp");
	sourceCPP.push_back("source_cpp/Sound.cpp");
	sourceCPP.push_back("source_cpp/SFX.cpp");
	sourceCPP.push_back("source_cpp/MData.cpp");
	sourceCPP.push_back("source_cpp/DData.cpp");
	sourceCPP.push_back("source_cpp/Astro.cpp");
	sourceCPP.push_back("source_cpp/Fall.cpp");
	sourceCPP.push_back("source_cpp/LFO.cpp");
	sourceCPP.push_back("source_cpp/OSC.cpp");
	sourceCPP.push_back("source_cpp/NOSC.cpp");
	sourceCPP.push_back("source_cpp/DelayLine.cpp");
	sourceCPP.push_back("source_cpp/MML.cpp");
	sourceCPP.push_back("source_cpp/MPlayer.cpp");
	
	// number of cpp files we have
	int nCPPFiles = sourceCPP.size();

	string line;
	ifstream inFile;
	
	for(int i=0; i<nCPPFiles; i++)
	{
		inFile.open(sourceCPP[i].c_str());
		if(!inFile)
			cout << "Error opening: " << sourceCPP[i] << endl;
		
		while( getline(inFile, line) )
		{
			str += line + "\n";
		}
		inFile.close();
		
		str += "\n\n\n\n";
	}
	
	// now rewrite the include directives... for example
	// #include "MML.h" will become #include "BC/MML.h"

	cout << "rewriting the header filenames to BC/---.h ...\n";	
	string header[11] = {	"\"MData.h", "\"DData.h", "\"Astro.h", "\"LFO.h", "\"OSC.h", "\"OSC.h", "\"NOSC.h",
						 "\"DelayLine.h", "\"MML.h", "\"MPlayer.h", "\"Fall.h" };
	
	// there are 10 target filenames to rewrite
	for(int i=0; i<11; i++)
	{
		bool done = false;
		size_t found;
		while(!done)
		{
			found = str.find(header[i]);
			if(found!=string::npos) // target found!
			{
				str.erase(found, 1); // first erase the starting double quote
				str.insert(found, "\"BC/");
			}
			else
				done = true;
		}
	}
	cout << "...finished!\n";
	
	// comment out windows.h references...
	
	cout << "commenting out windows.h references...\n";
	
	bool done = false;
	size_t found;
	while(!done)
	{
		found = str.find("#include <windows.h>");
		if(found!=string::npos) // found!
		{
			str.insert(found+1, "?");
			str.insert(found, "//");
		}
		else
			done = true;
	}
	while(!done)
	{
		found = str.find("#include <Windows.h>");
		if(found!=string::npos) // found!
		{
			str.insert(found+1, "?");
			str.insert(found, "//");
		}
		else
			done = true;
	}
	
	cout << "..finished!\n";
	
	// comment out certain sections ... export functions etc.
	
	cout << "taking care of parts that need to be commented out...\n";
	
	done = false;
	string target = "/*-----[BCPLAYER][STARTCOMMENTOUT]-----*/";
	int nCharsToErase = strlen(target.c_str());	
	while(!done)
	{
		found = str.find(target);
		if(found!=string::npos) // found!
		{
			str.erase(found, nCharsToErase);
			str.insert(found, "/*----------");
		}
		else
			done = true;
	}

	done = false;
	target = "/*-----[BCPLAYER][ENDCOMMENTOUT]-----*/";
	nCharsToErase = strlen(target.c_str());	
	while(!done)
	{
		found = str.find(target);
		if(found!=string::npos) // found!
		{
			str.erase(found, nCharsToErase);
			str.insert(found, "----------*/");
		}
		else
			done = true;
	}
	
	cout << "..finished!\n";
	
	// adjust MPlayer.cpp file
	
	cout << "fixing up MPlayer.cpp now...\n";

	done = false;
	target = "/*-----[BCPLAYER][CALLSFX]-----*/";
	nCharsToErase = strlen(target.c_str());	
	while(!done)
	{
		found = str.find(target);
		if(found!=string::npos) // found!
		{
			str.erase(found, nCharsToErase);
			str.insert(found, "mix += sfx->getOutput(channel);");
		}
		else
			done = true;
	}

	done = false;
	target = "/*-----[BCPLAYER][CALLSFXINCALLBACK]-----*/";
	nCharsToErase = strlen(target.c_str());
	while(!done)
	{
		found = str.find(target);
		if(found!=string::npos) // found!
		{
			str.erase(found, nCharsToErase);
			str.insert(found, "soundAmplitudeLeft = sfx->getOutput(0);\n\t\t\tsoundAmplitudeRight = sfx->getOutput(1);");
		}
		else
			done = true;
	}

	done = false;
	target = "/*-----[BCPLAYER][ADDSFXBINDER]-----*/";
	nCharsToErase = strlen(target.c_str());	
	while(!done)
	{
		found = str.find(target);
		if(found!=string::npos) // found!
		{
			str.erase(found, nCharsToErase);
			str.insert(found, "void MPlayer::bindSFX(SFX *sfxObj)\n\t{ sfx = sfxObj; }");
		}
		else
			done = true;
	}
	
	cout << "...finished!\n";
	
	// finally write the contents of our string to BCPlayer.cpp
	ofstream outFile("target_cpp/BCPlayer.cpp", ios::trunc);
	if(!outFile)
		cout << "Cannot access or create the target file target_cpp/BCPlayer.cpp!\n";
	else
		cout << "Writing BCPlayer.cpp file now...\n";
	outFile << str;
	outFile.close();
	cout << "Finished! Look in target_cpp folder :)\n";

	// now we'll adjust MPlayer.h file, also!
	
	cout << "fixing up MPlayer.h now...\n";
	
	ifstream inFile2("source_h/MPlayer.h");
	if(!inFile2)
		cout << "Error opening: " << "source_h/MPlayer.h" << endl;
	
	str = "";
	while( getline(inFile2, line) )
	{
		str += line + "\n";
	}
	inFile2.close();
	
	// search and replace!
	
	done = false;
	target = "/*-----[BCPLAYER][INCLUDESFXHEADER]-----*/";
	nCharsToErase = strlen(target.c_str());	
	while(!done)
	{
		found = str.find(target);
		if(found!=string::npos) // found!
		{
			str.erase(found, nCharsToErase);
			str.insert(found, "#include \"SFX.h\"");
		}
		else
			done = true;
	}

	done = false;
	target = "/*-----[BCPLAYER][ADDSFXPROPERTIES]-----*/";
	nCharsToErase = strlen(target.c_str());	
	while(!done)
	{
		found = str.find(target);
		if(found!=string::npos) // found!
		{
			str.erase(found, nCharsToErase);
			str.insert(found, "void bindSFX(SFX* sfxObj);\n\tSFX* sfx;");
		}
		else
			done = true;
	}
	
	// change portaudio.h target place to BC/ folder
	
	done = false;
	target = "#include <portaudio.h>";
	nCharsToErase = strlen(target.c_str());	
	while(!done)
	{
		found = str.find(target);
		if(found!=string::npos) // found!
		{
			str.erase(found, nCharsToErase);
			str.insert(found, "#include \"BC/portaudio.h\"");
		}
		else
			done = true;
	}

	done = false;
	target = "#include \"portaudio.h\"";
	nCharsToErase = strlen(target.c_str());	
	while(!done)
	{
		found = str.find(target);
		if(found!=string::npos) // found!
		{
			str.erase(found, nCharsToErase);
			str.insert(found, "#include \"BC/portaudio.h\"");
		}
		else
			done = true;
	}
	
	cout << "...finished!\n";
	
	// finally write the contents of our string to MPlayer.h
	ofstream outFile2("target_h/MPlayer.h", ios::trunc);
	if(!outFile2)
		cout << "Cannot access or create the target file target_h/MPlayer.h!\n";
	else
		cout << "Writing MPlayer.h file now...\n";
	outFile2 << str;
	outFile2.close();
	cout << "Finished! Look in target_h folder :)\n";
	
	ShellExecute(NULL, "open", "target_cpp", NULL, NULL, SW_SHOWNORMAL);
	ShellExecute(NULL, "open", "target_h", NULL, NULL, SW_SHOWNORMAL);
	
	return 0;
}