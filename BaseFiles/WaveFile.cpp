#include <assert.h>
#include <fstream>
#include <cstring>
#include "WaveFile.h"

WaveFile::WaveFile(unsigned __int16 format, unsigned __int16 channels,
	unsigned __int32 sampleRate, unsigned __int16 bitsPerSample,
	unsigned __int32 numberOfSamples) {

	audioFormat_ = format;
	// this class does not know how to handle non-PCM compression
	assert(audioFormat_ == 1);
	
	numChannels_ = channels;
	sampleRate_ = sampleRate;

	bitsPerSample_ = bitsPerSample;
	bytesPerSample_ = bitsPerSample_ / 8;
	// we do this because I don't know the behavior for
	// values of bps that aren't this.
	assert(bitsPerSample_ == 8 || bitsPerSample_ == 16);
	
	numSamples_ = numberOfSamples;
	unsigned __int32 dataSize = numSamples_ * numChannels_ * bytesPerSample_;
	data_ = new char[dataSize];
	for (unsigned int i = 0; i < dataSize; i++) {
		data_[i] = 0;
	}
}

WaveFile::WaveFile(std::ifstream* fStream) {
	// all of this is based off of the page here:
	//https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
	assert(fStream != NULL);
	assert(fStream->good());

	char* headerbuffer = new char[4];
	fStream->read(headerbuffer, 4);
	assert(strncmp(headerbuffer, "RIFF", 4) == 0);

	unsigned long chunk0Size = 0;
	fStream->read((char *)&chunk0Size, 4);
	assert(chunk0Size >= 44);

	fStream->read(headerbuffer, 4);
	assert(strncmp(headerbuffer, "WAVE", 4) == 0);

	fStream->read(headerbuffer, 4);
	assert(strncmp(headerbuffer, "fmt ", 4) == 0);

	unsigned long chunk1Size = 0;
	fStream->read((char*)&chunk1Size, 4);
	assert(chunk1Size == 16);

	fStream->read((char*)&audioFormat_, 2);
	assert(audioFormat_ == 1);

	fStream->read((char*)&numChannels_, 2);

	fStream->read((char*)&sampleRate_, 4);

	// we will skip over byte rate, as that information
	// can be inferred from the other information
	fStream->read(headerbuffer, 4);

	// discard block align, as this also is information
	// that can be inferred
	fStream->read(headerbuffer, 2);
	
	fStream->read((char*)&bitsPerSample_, 2);
	// we do this because I don't know the behavior for
	// values of bps that aren't this.
	assert(bitsPerSample_ == 8 || bitsPerSample_ == 16);
	bytesPerSample_ = bitsPerSample_ / 8;

	fStream->read(headerbuffer, 4);
	assert(strncmp(headerbuffer, "data", 4) == 0);

	unsigned long chunk2Size = 0;
	fStream->read((char*)&chunk2Size, 4);
	// since its a 32 bit application we check the bounds
	assert(chunk2Size <= 4294967296);
	
	numSamples_ = chunk2Size / bytesPerSample_;

	data_ = new char[(unsigned int)chunk2Size];
	fStream->read(data_, chunk2Size);
}


float WaveFile::getSampleAsFloat(unsigned __int32 sampleNumber, unsigned __int16 channel) {
	// ensure that the sample and channel exists
	assert(sampleNumber < numSamples_);
	assert(channel < numChannels_);
	unsigned __int32 baseIndex = ((sampleNumber * numChannels_ * bytesPerSample_) + channel);

	//8bit wave files are constructed in a weird way
	if (bytesPerSample_ == 1)
		return (float)(data_[baseIndex] - 128) / 128.0f;

	//however, 16bit wave files are constructed in a matter that makes sense
	__int16 totalValue = ((__int16)data_[baseIndex] & 0xFF) | ((__int16)data_[baseIndex + 1] << 8);
	return (float)totalValue / 32768.0f;
}

void WaveFile::setSample(unsigned __int32 sampleNumber, unsigned __int16 channel, float value) {
	assert(sampleNumber <= numSamples_);
	assert(channel < numChannels_);
	assert(value <= 1.0f && value >= -1.0f);
	unsigned int baseIndex = (unsigned int)
		((sampleNumber * numChannels_ * bytesPerSample_) + channel);
	
	if (bytesPerSample_ == 1) {
		data_[baseIndex]= (char)((value * 128) + 127);
	}

	if (bytesPerSample_ == 2) {
		__int16 sample = (__int16)(value * 32768);
		// remember, the samples are held little endian
		data_[baseIndex] = (char)sample;
		data_[baseIndex + 1] = (char)(sample >> 8);
	}
}

void WaveFile::writeToStream(std::ofstream* fileStream) {
	// in order as specified here
	//https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
	fileStream->write("RIFF", 4);

	// this value is 4 + 8 + chunk1size + 8 + chunk2size
	// and chunk1size should be 16 for pcm, which we made an assertion about
	unsigned __int32 chunk0size = 40 + numSamples_ * bytesPerSample_;
	fileStream->write((char*)&chunk0size, 4);

	fileStream->write("WAVEfmt ", 8);

	unsigned __int32 chunk1size = 16;
	fileStream->write((char*)&chunk1size, 4);

	fileStream->write((char*)&audioFormat_, 2);
	
	fileStream->write((char*)&numChannels_, 2);

	fileStream->write((char*)&sampleRate_, 4);

	unsigned long byteRate = sampleRate_ * numChannels_ * bytesPerSample_;
	fileStream->write((char*)&byteRate, 4);

	unsigned short blockAlign = numChannels_ * bytesPerSample_;
	fileStream->write((char*)&blockAlign, 2);

	fileStream->write((char*)&bitsPerSample_, 2);

	fileStream->write("data", 4);

	unsigned long chunk2size = numSamples_ * numChannels_ * bytesPerSample_;
	fileStream->write((char*)&chunk2size, 4);

	fileStream->write(data_, chunk2size);
}

WaveFile::~WaveFile() {
	delete[] data_;
}

unsigned __int32 WaveFile::getNumberOfSamples() {
	return numSamples_;
}

unsigned __int16 WaveFile::getNumberOfChannels() {
	return numChannels_;
}