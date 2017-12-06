#include <iostream>
#include <vector>
#include <fstream>
#include <
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

// convert big-endian to little-endian and vice versa.
unsigned int swapEndian(vector<unsigned char> &buffer) {
    unsigned int a = 0;
    int max = buffer.size();
    for (int i = 0; i < max; i++) {
      a |= buffer[i] << (i*8);
    }
    return a;
}

// Convert integers to a char array. swap endianness as well
void itoa(unsigned int i, unsigned char* a) {
  if (sizeof(a)/2 == 4) {
    a[0] = i & 0xff;
    a[1] = (i >> 8) & 0xff;
    a[2] = (i >> 16) & 0xff;
    a[3] = (i >> 24) & 0xff;
  } else {
    a[0] = i & 0xff;
    a[1] = (i >> 8) & 0xff;
  }
}

// Parses Input files
void parseFile(char *fileName, struct WAV *infoHolder, vector<signed short int> &data, int &numSamples) {
  ifstream file;
  file.open(fileName, ios::out | ios::binary);

  if (file.is_open()) {
    // Read chunkID - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), infoHolder->chunkID);

    // Read chunkSize - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->chunkSize = swapEndian(buffer4);

    // Read format - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), infoHolder->format);

    // Read subChunk1ID - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), infoHolder->subChunk1ID);

    // Read subChunk1Size - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->subChunk1Size = swapEndian(buffer4);

    // Read audioFormat - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    infoHolder->audioFormat = swapEndian(buffer2);

    // Read numChannels - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    infoHolder->numChannels = swapEndian(buffer2);

    // Read sampleRate - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->sampleRate = swapEndian(buffer4);

    // Read byteRate - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->byteRate = swapEndian(buffer4);

    // Read blockAlign - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    infoHolder->blockAlign = swapEndian(buffer2);

    // Read bitsPerSample - little endian
    file.read((char*) &buffer2[0], buffer2.size());
    infoHolder->bitsPerSample = swapEndian(buffer2);

    // discard extra bits if they're present
    if (infoHolder->subChunk1Size > 16) {
      file.read((char*) &buffer2[0], infoHolder->subChunk1Size-16);
    }

    // Read subChunk2ID - big endian
    file.read((char*) &buffer4[0], buffer4.size());
    copy(buffer4.begin(), buffer4.end(), infoHolder->subChunk2ID);

    // Read subChunk2Size - little endian
    file.read((char*) &buffer4[0], buffer4.size());
    infoHolder->subChunk2Size = swapEndian(buffer4);

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

// Create a new WAV file.
void writeWAV(char *outputFile, const vector<signed short int> &data, struct WAV &infoHolder) {
  ofstream file(outputFile, ios::out | ios::binary);
  unsigned char a4[4];
  unsigned char a2[2];

  if (file.is_open()) {
    // Write chunkID - big-endian
    file.write(infoHolder.chunkID, sizeof(infoHolder.chunkID));

    // Write chunkSize - little-endian
    itoa(infoHolder.chunkSize, a4);
    file.write((char*) &a4, sizeof(a4));

    // Write format - big-endian
    file.write(infoHolder.format, sizeof(infoHolder.format));

    // Write subChunk1ID - big-endian
    file.write(infoHolder.subChunk1ID, sizeof(infoHolder.subChunk1ID));

    // Write subChunk1Size - little-endian
    itoa(infoHolder.subChunk1Size, a4);
    file.write((char*) &a4, sizeof(a4));

    // Write audioFormat - litten-endian
    itoa(infoHolder.audioFormat, a2);
    file.write((char*) &a2, sizeof(a2));

    // Write numChannels - little-endian
    itoa(infoHolder.numChannels, a2);
    file.write((char*) &a2, sizeof(a2));

    // Write sampleRate - little-endian
    itoa(infoHolder.sampleRate, a4);
    file.write((char*) &a4, sizeof(a4));

    // Write byteRate - little-endian
    itoa(infoHolder.byteRate, a4);
    file.write((char*) &a4, sizeof(a4));

    // Write blockAlign - little-endian
    itoa(infoHolder.blockAlign, a2);
    file.write((char*) &a2, sizeof(a2));

    // Write bitsPerSample - little-endian
    itoa(infoHolder.bitsPerSample, a2);
    file.write((char*) &a2, sizeof(a2));

    // Write extra bits to account for file size
    if (infoHolder.subChunk1Size > 16) {
      a2[0] = 0;
      a2[1] = 0;
      file.write((char*) &a2, sizeof(a2));
    }

    // Write subChunk2ID - big endian
    file.write(infoHolder.subChunk2ID, sizeof(infoHolder.subChunk2ID));

    // Write subChunk2Size - little-endian
    itoa(infoHolder.subChunk2Size, a4);
    file.write((char*) &a4, sizeof(a4));

    int i, max;
    max = data.size();
    for (i = 0; i < max; i++) {
      itoa(data[i], a2);
      file.write((char*) &a2, sizeof(a2));
    }
  }
  file.close();
}

void normalize(vector<signed short int> &data, vector<double> &output, int numSamples) {
  for (int i = 0; i < numSamples; i++) {
    output.push_back((double)data[i] / 32767.0);
  }
}

void denormalize(vector<double> &input, vector<signed short int> &output, int length) {
  for (int i = 0; i < length; i++) {
    output[i] = input[i] * 32767;
  }
}

void scaleDown(vector<double> &data) {
  double max, min, scale;
  max = -2.01;
  min = 2.01;
  int i, num;
  num = data.size();
  for (i = 0; i < num; i++) {
    if (data[i] > max) {
      max = data[i];
    }
    if (data[i] < min) {
      min = data[i];
    }
  }
  if (max > fabs(min)) {
    scale = 1.0/max;
  } else {
    scale = 1.0/fabs(min);
  }

  for (i = 0; i < num; i++) {
    data[i] *= scale;
  }
}

void convolve(vector<double> &x, int N, vector<double> &h, int M, vector<double> &y, int P) {
  int n, m;

  for (n = 0; n < P; n++) {
    y[n] = 0.0;
  }
// n*M + m
  for (n = 0; n < N; n++) {
    for (m = 0; m < M; m++) {
      y[n+m] += x[n] * h[m];
    }
  }
  cout << endl;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    cout << "ERROR: Missing argument" << endl;
    cout << "input: Sound file, Impulse Response file, Output file name";
    exit(1);
  }

  struct WAV wav;
  struct WAV ir;

  //int numOutputSamples;
  int numDataSamples;
  int numIRSamples;

  vector<signed short int> wavData;
  vector<signed short int> irData;

  vector<double> x;
  vector<double> h;

  // open a file
  parseFile(argv[1], &wav, wavData, numDataSamples);
  parseFile(argv[2], &ir, irData, numIRSamples);

  normalize(wavData, x, numDataSamples);
  normalize(irData, h, numIRSamples);

  int numOutputSamples = numDataSamples + numIRSamples - 1;
  vector<double> y(numOutputSamples);
  vector<signed short int> convolvedData(numOutputSamples);

  cout << numDataSamples << endl;
  cout << numIRSamples << endl;
  cout << numOutputSamples << endl;

  convolve(x, numDataSamples, h, numIRSamples, y, numOutputSamples);

  scaleDown(y);

  denormalize(y, convolvedData, numOutputSamples);

  writeWAV(argv[3], convolvedData, wav);

  return 0;
}
