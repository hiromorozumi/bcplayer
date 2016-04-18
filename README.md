***** BCPlayer - BeepComp Audio Engine API *****
================================================

I am developing this library so that game/app developers can easily use
music made with BeepComp in their projects.
The point of making this library is to come up with an audio engine
that can read and play directly from text source files, eliminating 
the need for the user to convert their music work to audio files.
This way, your project file will stay lightweight, too! :)


The currently available features are:

- Load BeepComp source text files to play background music
- Can load a std::string source to play music instead of loading a file 
- SFX engine with 16 independent slots for sound effects
- Load sound with 16bit WAV or OGG audio files, mono or stereo
- Stereo panning for each sound effect


Dependencies
------------

This library depends on the following libraries:

	- portaudio
	- libsndfile

So if you want to use this library for your project, you will need to 
link against these libraries. The lib folder has the two necessary .lib files
--- portaudio.lib and libsndfile, these should be linked to your program
when you compile.


Folder Structure
----------------

I would recommend setting up your project this way:


    MyGameProject -----+----- include/ --------- BC/ ---------------- (many .h files)
                       |         
                       +----- lib/ ---------+--- portaudio_x86.lib       
                       |                    |
                       |                    +--- libsndfile-1.lib           				 
                       |
                       +----- BCPlayer.cpp
                       |
                       +----- main.cpp


The BC folder inside the include folder has all the header files that BCPlayer needs.
All you have to do to "include" to BCPlayer library is just add the following directive
at the top your main function.

    #include "BC/BCPlayer.h"

The single BCPlayer.cpp file is the library. Let it sit next to your main function and
just compile along with all your source code.					   
And when you comile, remember to link against the two .lib files in the lib folder.


How to Use the Library
----------------------

Using BCPlayer is pretty easy.
Begin by including the header in your source:

    #include "BC/BCPlayer.h"
	
In your program where you want to use BCPlayer, instantiate a BCPlayer object.

    BCPlayer bcplayer;
	
Then, to load music from a BeepComp source text file and play it: 

    bcplayer.loadMusic("mySong.txt");
	bcplayer.startMusic();

And... you can do other things like changing volume or setting up your music to loop etc.:

    bcplayer.setMusicVolume(60); // scale to 100
	bcplayer.enableLooping()

You can play sound effects on top your music.
There are 16 possible slots (0-15). This example load a sound to slot #2 and play it: 

    bcplayer.loadSFX(2, "bang.wav"); // needs to be 16-bit WAV, can be mono or stereo
    bcplayer.startSFX(2);

These example programs will show you more....:

- [Simple Background Music Demo](https://github.com/hiromorozumi/bcplayer/blob/master/BCPlayerApp.cpp)
- [Play a String Source](https://github.com/hiromorozumi/bcplayer/blob/master/stringPlayer.cpp)
- [SFX Demo](https://github.com/hiromorozumi/bcplayer/blob/master/SFXTest.cpp)


Building Your Project with BCPlayer
-----------------------------------

Bulletpoints of what should happen in your compilation:

    - The **include** folder should have the **BC** folder inside it
	- Tell the compiler to include your **include** folder
    - Compile your source along with BCPlayer.cpp
	- Link against **libsndfile** and **portaudio** libraries, which BCPlayer depends on

So, if you're using MinGW, you can build your project like this:

    g++ BCPlayer.cpp myApp.cpp -I/.include -L/.lib lib/libsndfile-1.lib lib/portaudio_x86.lib -o myApp
