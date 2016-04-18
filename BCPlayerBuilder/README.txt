BCPlayerBuilder
===============

This is a little program to assist myself in adopting source files from BeepComp to build a monolithic BCPlayer.cpp file.
By using this program, following files are produced:

	- BCPlayer.cpp
	- MPlayer.h
	
BCPlayer.cpp is the monolithic (big!) cpp file to implement the BCPlayer library.
It includes all the classes required to run BCPlayer, specifically the following classes:

    Sound, SFX, MData, DData, Astro, LFO, OSC, NOSC, DelayLine, MML, MPlayer

Steps to use BCPlayerBuilder to produce BCPlayer-ready "BCPlayer.cpp" and "MPlayer.h":

1. Copy all the .cpp files from the current BeepComp source version to source_cpp folder inside this directory.
2. Place the current BCPlayerBase.cpp file into the source_cpp folder.
3. Copy all the .h files from the current BeepComp source version to source_h folder inside this directory.
4. Run BCPlayerBuilder.exe.
5. Copy the new BCPlayer.cpp in the target_h folder into BCPlayer folder (overwrite).
6. Copy the new MPlayer.h in the target_h folder into BCPlayer/include/BC foler (overwrite).
