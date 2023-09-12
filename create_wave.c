#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define FREQ 440
#define RATE 44100

const unsigned char *riff = "RIFF";
const unsigned char *wave = "WAVE";
const unsigned char *fmt  = "fmt ";
const unsigned char *data = "data";
const char *file = "wave_test.wav";
double PI = 3.14159265358979323846;

typedef struct {
   unsigned int   file_size;
   unsigned int   len_fmt;
   unsigned short type_fmt;
   unsigned short channels;
   unsigned int   samp_rate;
   unsigned int   samp_calc;
   unsigned short bits_calc;
   unsigned short bits_samp;
   unsigned int   data_size;
} WaveStruct;

typedef struct {
   signed short *wave;
   int wave_size;
} waveGenerated;

unsigned char *littleEndian(unsigned int num){
   unsigned char *bytes = malloc(4 * sizeof(unsigned char));
   bytes[3] = (num >> 24) & 0xFF;
   bytes[2] = (num >> 16) & 0xFF;
   bytes[1] = (num >> 8)  & 0xFF;
   bytes[0] = num & 0xFF;
   return bytes;
}

unsigned char *littleEndian_16(unsigned short num){
   unsigned char *bytes = malloc(2 * sizeof(unsigned short));
   bytes[1] = (num >> 8) & 0xFF;
   bytes[0] = num & 0xFF;
   return bytes;
}

signed char *littleEndian_s16(signed short num){
   signed char *bytes = malloc(2 * sizeof(signed short));
   bytes[1] = (num >> 8) & 0xFF;
   bytes[0] = num & 0xFF;
   return bytes;
}

int saveWav(WaveStruct header, waveGenerated waveData){
   int fd = open(file, O_CREAT | O_TRUNC | O_RDWR , S_IRUSR | S_IWUSR);
   if(fd == -1)
      fprintf(stderr, "Open File Error! %d\n", errno);
   //snprintf(header.file_size, 9, "%08x", littleEndian(num));

   write(fd, riff, 4);
   printf("Riff header: %s \n", riff);
   write(fd, littleEndian(header.file_size), 4);
   printf("File size: %d \n", header.file_size);
   write(fd, wave, 4);
   printf("Wave header: %s\n", wave);
   write(fd, fmt, 4);
   printf("Format header: %s\n", fmt);
   write(fd, littleEndian(header.len_fmt), 4);
   printf("Format length: %d\n", header.len_fmt);
   write(fd, littleEndian_16(header.type_fmt), 2);
   printf("Format type: %d\n", header.type_fmt);
   write(fd, littleEndian_16(header.channels), 2);
   printf("Channels: %d\n", header.channels);
   write(fd, littleEndian(header.samp_rate), 4);
   printf("Sampling rate: %d\n", header.samp_rate);
   write(fd, littleEndian(header.samp_calc), 4);
   printf("Byte rate: %d\n", header.samp_calc);
   write(fd, littleEndian_16(header.bits_calc), 2);
   printf("Block align: %d\n", header.bits_calc);
   write(fd, littleEndian_16(header.bits_samp), 2);
   printf("Bits per sample: %d\n", header.bits_samp);
   write(fd, data, 4);
   printf("Data header: %s\n", data);
   write(fd, littleEndian(header.data_size),4);
   printf("Bytes in data: %d\n", header.data_size);

   for(int i=0 ; i<waveData.wave_size; i++)
      write(fd, littleEndian_s16(waveData.wave[i]), 2);

   return close(fd);
}

waveGenerated genWave(float freq, float sampling_rate){
   waveGenerated waveSine;
   int len_aux = 0;
   int wave_len =  round(1.0 / freq * sampling_rate);
   waveSine.wave_size = 880 * wave_len;
   waveSine.wave = (signed short *) malloc(waveSine.wave_size * sizeof(signed short));

   //printf("c(");
   for(int j=0; j<880; j++){
      for(int i=0; i<wave_len; i++){
         waveSine.wave[i+len_aux] = (signed short) round(sin(2*PI*freq/sampling_rate*i)*32767);
         //printf("%d, ", waveSine.wave[i]);
      }
      len_aux = len_aux +  wave_len;
   }
   //printf(")\n");

   return waveSine;
}

extern int errno;
int fd, res;
unsigned int num = 44100;
unsigned int aux = 0;

int main (){
   waveGenerated waveSine = genWave(440.0, 44100.0);
   WaveStruct wavFile = {
      36 + waveSine.wave_size * sizeof(signed short),
      16, 1, 1, 44100,
      (44100*16*1)/8, 2, 16,
      waveSine.wave_size * sizeof(signed short)};
   int r = saveWav(wavFile, waveSine);
   if(r == -1)
      printf("ERROR");

   return 0;
}
