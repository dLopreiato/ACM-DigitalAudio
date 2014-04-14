#include <fstream>
/*This class assumes that you are on a big endian machine,
and uses assert statments instead of error handling as this
is meant to be modified. One thing to note about this
class is that it reads entire wave files into memory. This
is an awful idea as wave files are generally large, but as
this is just a demo to digital audio, we'll do it.*/
#ifndef PSU_ACM2014_WAVEFILE
#define PSU_ACM2014_WAVEFILE

class WaveFile {
public:
	// Constructs a silent wave file with given specifications
	WaveFile(unsigned __int16 format, unsigned __int16 channels,
		unsigned __int32 sampleRate, unsigned __int16 bitsPerSample,
		unsigned __int32 numberOfSamples);
	// Constructs a WaveFile from a file in the wave format
	WaveFile(std::ifstream* fileStream);
	~WaveFile();
	// Gets a sample from the data as a float. Sample number and channel are indexed.
	float getSampleAsFloat(unsigned __int32 sampleNumber, unsigned __int16 channel);
	// Sets a sample from a given float between 1 and -1. Sample number and channel are indexed.
	void setSample(unsigned __int32 sampleNumber, unsigned __int16 channel, float value);
	// Writes the microsoft specified wave file format of this wave file to a stream
	void writeToStream(std::ofstream* fileStream);
	// Gets the number of samples in one channel in this WaveFile
	unsigned __int32 getNumberOfSamples();
	// Gets the number of channels in this WaveFile
	unsigned __int16 getNumberOfChannels();
private:
	unsigned __int16 audioFormat_;
	unsigned __int16 numChannels_;
	unsigned __int32 sampleRate_;
	unsigned __int16 bitsPerSample_;
	unsigned __int16 bytesPerSample_;
	unsigned __int32 numSamples_;
	char* data_;
};

#endif
