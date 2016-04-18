g++ BCPlayer.cpp BCPlayerApp.cpp -I./include lib/libsndfile-1.lib lib/portaudio_x86.lib -o BCPlayerApp

g++ BCPlayer.cpp stringPlayer.cpp -I./include lib/libsndfile-1.lib lib/portaudio_x86.lib -o stringPlayer

g++ BCPlayer.cpp SFXTest.cpp -I./include lib/libsndfile-1.lib lib/portaudio_x86.lib -o SFXTest