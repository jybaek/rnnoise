/* Copyright (c) 2017 Mozilla */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include "rnnoise.h"

#define FRAME_SIZE 480
#define ADD_WAVE_HEADER 0

int main(int argc, char **argv) {
  int i;
  int first = 1;
  float x[FRAME_SIZE];
  FILE *f1, *fout;
  DenoiseState *st;
  st = rnnoise_create();
  if (argc!=3) {
    fprintf(stderr, "usage: %s <noisy speech> <output denoised>\n", argv[0]);
    return 1;
  }
  f1 = fopen(argv[1], "r");
  fout = fopen(argv[2], "w");

#if ADD_WAVE_HEADER
  int channel = 2;
  int bitspersample = 16;
  int samplerate = 48000;
  long byteRate = channel * samplerate * bitspersample / 8;

  fseek(f1, 0, SEEK_END);
  long fileLength = ftell(f1);
  fseek(f1, 0, SEEK_SET);
#endif

  while (1) {
    short tmp[FRAME_SIZE];
    fread(tmp, sizeof(short), FRAME_SIZE, f1);
    if (feof(f1)) break;
    for (i=0;i<FRAME_SIZE;i++) x[i] = tmp[i];
    rnnoise_process_frame(st, x, x);
    for (i=0;i<FRAME_SIZE;i++) tmp[i] = x[i];
    if (!first) fwrite(tmp, sizeof(short), FRAME_SIZE, fout);
#if ADD_WAVE_HEADER
    else {
      short header[FRAME_SIZE];
      sprintf(header, "%c%c%c%c%c%c%c%c" 
          "%c%c%c%c%c%c%c%c"
          "%c%c%c%c%c%c"
          "%c%c" // channel, 0
          "%c%c%c%c" // samplerate
          "%c%c%c%c" // byteRate
          "%c%c" // channel * bitspersample / 8, 0
          "%c%c" // bitspersample, 0
          "%c%c%c%c" // data
          "%c%c%c%c", // pcmsize
          'R', 'I', 'F', 'F', (char)(fileLength&0xff), (char)((fileLength>>8)&0xff), (char)((fileLength>>16)&0xff), (char)((fileLength>>24)&0xff),
          'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
          16, 0, 0, 0, 1, 0,
          (char)channel, 0, // channel
          (char)(samplerate&0xff), (char)((samplerate>>8)&0xff), (char)((samplerate>>16)&0xff), (char)((samplerate>>24)&0xff), //samplerate
          (char)(byteRate&0xff), (char)((byteRate>>8)&0xff), (char)((byteRate>>16)&0xff), (char)((byteRate>>24)&0xff), //byteRate
          (char)(channel * bitspersample / 8), 0,
          bitspersample, 0,
          'd', 'a', 't', 'a',
          (char)(fileLength&0xff), (char)((fileLength>>8)&0xff), (char)((fileLength>>16)&0xff), (char)((fileLength>>24)&0xff)
          );
      fwrite(header, sizeof(char), FRAME_SIZE, fout);
    }
#endif
    first = 0;
  }
  rnnoise_destroy(st);
  fclose(f1);
  fclose(fout);
  return 0;
}
