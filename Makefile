normal:
	g++ BCPlayer.cpp main.cpp -I./include lib/portaudio_x86.lib -o bcplayerApp
	
bcplayerApp:
	g++ BCPlayer.cpp main.cpp -I./include lib/portaudio_x86.lib -o bcplayerApp

cleanbcPlayerApp:
	rm ./bcplayerApp.exe
	
stringPlayer:
	g++ BCPlayer.cpp stringPlayer.cpp -I./include lib/portaudio_x86.lib -o stringPlayer

cleanStringPlayer:
	rm ./stringPlayer.exe