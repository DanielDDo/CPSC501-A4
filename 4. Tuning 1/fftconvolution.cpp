#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
using namespace std;

#define PI        3.141592653589793
#define TWO_PI    (2.0 * PI)
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

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
  void writeWAV(char *outputFile, const vector<signed short int> &data, struct WAV &infoHolder, int subChunk2Size) {
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
      itoa(subChunk2Size, a4);
      file.write((char*) &a4, sizeof(a4));

      int i, size;
      size = data.size();
      for (i = 0; i < size; i++) {
        itoa(data[i], a2);
        file.write((char*) &a2, sizeof(a2));
      }
    }
    file.close();
  }

  void normalize(vector<signed short int> &data, vector<double> &output, int numSamples) {
    int i;
    for (i = 0; i < numSamples; i++) {
      output.push_back((double)data[i] / 32767.0);
    }
  }

  void fft(vector<double> &data, int nn, int isign) {
    unsigned long n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta, tempr, tempi;

    n = nn << 1;
    j = 1;

    for (i = 1; i < n; i += 2) {
      if (j > i) {
        SWAP(data[j], data[i]);
        SWAP(data[j+1], data[i+1]);
      }
      m = nn;
      while (m >= 2 && j > m) {
        j -= m;
        m >>= 1;
      }
      j += m;
    }

    mmax = 2;
    while (n > mmax) {
      istep = mmax << 1;
      theta = isign * (TWO_PI / mmax);
      wtemp = sin(0.5 * theta);
      wpr = -2.0 * wtemp * wtemp;
      wpi = sin(theta);
      wr = 1.0;
      wi = 0.0;
      for (m = 1; m < mmax; m += 2) {
        for (i = m; i <= n; i += istep) {
          j = i + mmax;
          tempr = wr * data[j] - wi * data[j+1];
          tempi = wr * data[j+1] + wi * data[j];
          data[j] = data[i] - tempr;
          data[j+1] = data[i+1] - tempi;
          data[i] += tempr;
          data[i+1] += tempi;
        }
        wr = (wtemp = wr) * wpr - wi * wpi + wr;
        wi = wi * wpr + wtemp * wpi + wi;
      }
      mmax = istep;
    }
  }

  void complexMultiplication(vector<double> &X, vector<double> &H, vector<double> &Y, int size) {
    Y.push_back(0.0);
    for (int k = 1; k < size; k += 2) {
      Y.push_back(X[k] * H[k] - (X[k+1] * H[k+1]));
      Y.push_back(X[k+1] * H[k] + X[k] * H[k+1]);
    }
  }

  void combineRealAndImaginary(vector<double> &x, vector<double> &x2, vector<double> &h, vector<double> &h2) {
    // Find the nearest 2 power.
    int temp = (int) ceil(log2(x.size()));
    int newSize = 1 << temp;

    while((int)x.size() < newSize) {
      x.push_back(0.0);
    }
    while((int)h.size() < newSize) {
      h.push_back(0.0);
    }
    x2.push_back(0.0);
    h2.push_back(0.0);
    for(int i = 0; i < newSize; i++) {
      x2.push_back(x[i]);
      x2.push_back(0.0);
      h2.push_back(h[i]);
      h2.push_back(0.0);
    }
  }

  void splitRealAndImaginary(vector<double> &Y, vector<double> &y, int outputSize) {
    int k;
    for (k = 1; k < outputSize; k ++) {
      y.push_back(Y[2*k-1]);
    }
  }

  void afterFFTScale(vector<double> &Y, int N) {
    int i;
    for (i = 0; i < N; i += 2) {
      Y[i] /= (double) (N/2);
      Y[i+1] /= (double) (N/2);
    }
  }

  void scaleAndDenormalize(vector<double> &y, vector<signed short int> &outputData) {
    double max, min, scale;
    int i, num;
    max = -2.01;
    min = 2.01;
    num = y.size();
    // find the max/min data points
    for (i = 0; i < num; i++) {
      if (y[i] > max) {
        max = y[i];
      }
      if (y[i] < min) {
        min = y[i];
      }
    }
    if (max > fabs(min)) {
      scale = 1.0/max;
    } else {
      scale = 1.0/fabs(min);
    }
    for (i = 0; i < num; i++) {
      outputData.push_back(y[i] * scale * 32767);
    }
  }
int main(int argc, char* argv[]) {
  if (argc != 4) {
    cout << "ERROR: Missing argument" << endl;
    cout << "input: Sound file, Impulse Response file, Output file name" << endl;;
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
  vector<double> y;

  vector<double> X;
  vector<double> H;
  vector<double> Y;

  vector<signed short int> outputData;
  // open a file
  parseFile(argv[1], &wav, wavData, numDataSamples);
  parseFile(argv[2], &ir, irData, numIRSamples);

  normalize(wavData, x, numDataSamples);
  normalize(irData, h, numIRSamples);

  int outputSize = x.size() + h.size() - 1;

  combineRealAndImaginary(x, X, h, H);

  fft(X, (X.size()-1)/2, 1);
  fft(H, (H.size()-1)/2, 1);

  complexMultiplication(X, H, Y, X.size());

  fft(Y, (Y.size()-1)/2, -1);
  afterFFTScale(Y, Y.size());

  splitRealAndImaginary(Y, y, outputSize);

  scaleAndDenormalize(y, outputData);

  int newSubChunk2Size = (outputSize * wav.numChannels * (wav.bitsPerSample/8));

  writeWAV(argv[3], outputData, wav, newSubChunk2Size);
  return 0;
}
