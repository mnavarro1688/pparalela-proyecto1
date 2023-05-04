programaP: cut_video_paralelo.c
	mpicxx $(CXXFLAGS) -o programaP cut_video_paralelo.c $(LIBS)

LIBS=-lmpi -fopenmp `pkg-config --cflags --libs opencv4` -lboost_filesystem
CXXFLAGS=-std=c++17 -Wall -O3 -fopenmp
