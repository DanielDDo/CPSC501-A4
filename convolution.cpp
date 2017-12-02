#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

struct WAV{
  char chunkID[4];
  unsigned int chunkSize;
  char format[4];

  char subChunk1ID[4];
  unsigned int subChunk1Size;
  unsigned int audioFormat;
  unsigned int numChannels;
  unsigned int sampleRate;
  unsigned int byteRate;
  unsigned int blockAlign;
  unsigned int bitsPerSample;

  char subChunk2ID[4];
  unsigned int subChunk2Size;
};
  vector<unsigned char> buffer4(4);
  vector<unsigned char> buffer2(2);

void parseFile(char *fileName, struct WAV *infoHolder, vector<signed short int> &data, int &numSamples) {
  ifstream file;
  file.open(fileName, ios::out | ios::binary);

  if (file.is_open()) {
    // Read chunkID - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), infoHolder->chunkID);

    // Read chunkSize - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->chunkSize = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

    // Read format - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), infoHolder->format);

    // Read subChunk1ID - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), infoHolder->subChunk1ID);

    // Read subChunk1Size - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->subChunk1Size = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

    // Read audioFormat - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    infoHolder->audioFormat = (buffer2[0] << 0) | (buffer2[1] << 8);

    // Read numChannels - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    infoHolder->numChannels = (buffer2[0] << 0) | (buffer2[1] << 8);

    // Read sampleRate - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->sampleRate = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

    // Read byteRate - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->byteRate = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

    // Read blockAlign - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    infoHolder->blockAlign = (buffer2[0] << 0) | (buffer2[1] << 8);

    // Read bitsPerSample - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    infoHolder->bitsPerSample = (buffer2[0] << 0) | (buffer2[1] << 8);

    // discard extra bits if they're present
    if (infoHolder->subChunk1Size > 16) {
      file.read((char*) &buffer4[0], infoHolder->subChunk1Size-16);
    }

    // Read subChunk2ID - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), infoHolder->subChunk2ID);

    // Read subChunk2Size - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->subChunk2Size = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

    // Calculate the number of samples
    numSamples = (int)(infoHolder->subChunk2Size/(infoHolder->numChannels * (infoHolder->bitsPerSample/8)));
    signed short int a;
    // fill data array with wav file's data
    for (int i = 0; i < numSamples; i++) {
      file.read((char*) &buffer2[0], buffer2.size());
      a = buffer2[0] | buffer2[1] << 8; // data is in little endian
      data.push_back(a);
    }
  }
  file.close();
}

void normalize(vector<signed short int> &data , vector<double> &output, int numSamples) {
  for (int i = 0; i < numSamples; i++) {
    output.push_back((double)data[i] / 32767.0);
  }
}


int main(int argc, char *argv[]) {
  struct WAV wav;
  struct WAV ir;

  int numOutputSamples;
  int numDataSamples;
  int numIRSamples;

  vector<signed short int> wavData;
  vector<signed short int> irData;

  vector<double> x;
  vector<double> h;
  vector<double> y;

  // open a file
  parseFile(argv[1], &wav, wavData, numDataSamples);
  parseFile(argv[2], &ir, irData, numIRSamples);

  normalize(wavData, x, numDataSamples);
  normalize(irData, h, numIRSamples);

  return 0;
}
