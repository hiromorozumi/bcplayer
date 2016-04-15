rm *.exe
g++ BCPlayer.cpp main.cpp -I./include lib/portaudio_x86.lib -o bcplayerApp
g++ BCPlayer.cpp stringPlayer.cpp -I./include lib/portaudio_x86.lib -o stringPlayer
