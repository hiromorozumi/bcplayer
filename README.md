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
