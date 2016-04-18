bcplayerApp:
	g++ BCPlayer.cpp BCPlayerApp.cpp -I./include lib/portaudio_x86.lib -o bcplayerApp

cleanbcPlayerApp:
	rm ./bcplayerApp.exe
	
stringPlayer:
	g++ BCPlayer.cpp stringPlayer.cpp -I./include lib/portaudio_x86.lib -o stringPlayer

cleanStringPlayer:
	rm ./stringPlayer.exe

SFXTest:
	g++ Sound.cpp SFX.cpp BCPlayer.cpp -I./include lib/libsndfile-1.lib lib/portaudio_x86.lib -o SFXTest

cleanSFXTest:
	rm ./SFXTest.exe

cleanAll:
	rm ./*.exe