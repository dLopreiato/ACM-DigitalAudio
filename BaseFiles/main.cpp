#include <iostream>
#include <fstream>
#include <sstream>
#include "WaveFile.h"

using namespace std;

int main() {

	ifstream kickFile;
	kickFile.open("MonoKick.wav", ifstream::in | ifstream::binary);
	WaveFile kick = WaveFile(&kickFile);
	kickFile.close();
	
	ofstream newFile;
	newFile.open("NewFile.wav", ifstream::out | ifstream::binary);
	kick.writeToStream(&newFile);
	newFile.close();

	return 0;
}