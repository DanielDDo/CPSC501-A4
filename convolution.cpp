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

int main(int argc, char *argv[]) {
  struct WAV wav;
  // open a file
  ifstream file;
  file.open(argv[1], ios::out | ios::binary);
  if (file.is_open()) {
    // Read chunkID - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), wav.chunkID);

    // Read chunkSize - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    wav.chunkSize = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

    // Read format - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), wav.format);

    // Read subChunk1ID - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), wav.subChunk1ID);

    // Read subChunk1Size - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    wav.subChunk1Size = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

    // Read audioFormat - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    wav.audioFormat = (buffer2[0] << 0) | (buffer2[1] << 8);

    // Read numChannels - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    wav.numChannels = (buffer2[0] << 0) | (buffer2[1] << 8);

    // Read sampleRate - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    wav.sampleRate = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

    // Read byteRate - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    wav.byteRate = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

    // Read blockAlign - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    wav.blockAlign = (buffer2[0] << 0) | (buffer2[1] << 8);

    // Read bitsPerSample - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    wav.bitsPerSample = (buffer2[0] << 0) | (buffer2[1] << 8);

    // discard extra bits if they're present
    if (wav.subChunk1Size > 16) {
      file.read((char*) &buffer4[0], wav.subChunk1Size-16);
    }

    // Read subChunk2ID - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), wav.subChunk2ID);

    // Read subChunk2Size - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    wav.subChunk2Size = (buffer4[0] << 0) | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);
  }
  file.close();

  return 0;
}
