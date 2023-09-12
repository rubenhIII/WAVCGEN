#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <pulse/simple.h>

#define FREQ 440
#define RATE 44100

double PI = 3.14159265358979323846;

float sampling_rate = 44200;
float freq = 2550.0;
int wave_size;
int *wave;
float wave_sample;

int main(){

    wave_size = round(1.0/freq*sampling_rate);
    wave = (int *) malloc (wave_size * sizeof(int));

    printf("c(");
    for (int i=0 ; i<wave_size; i++){
        wave[i] = (int) round((sin(2*PI*freq/sampling_rate*i) + 1)/2*255);
        printf("%d,",wave[i]);
    }
    printf(")\n");


    return 0;
}

/* gcc -W -Wall -O2 $(shell pkg-config --cflags libpulse-simple) -o sine sine.c -lm $(shell pkg-config --libs libpulse-simple) */

#include <stdlib.h>
#include <stdint.h>
#include <math.h>



static int16_t samples[RATE / FREQ];

static const pa_sample_spec spec = {
	.format		= PA_SAMPLE_S16NE,
	.rate		= RATE,
	.channels	= 1,
};

int main(void)
{
	double x = 0;

	for (int i = 0; i < RATE / FREQ; i++, x += (2 * M_PI * FREQ) / RATE)
		samples[i] = INT16_MAX * sin(x);

	pa_simple *s = pa_simple_new(NULL, "sine", PA_STREAM_PLAYBACK, NULL, "sine", &spec, NULL, NULL, NULL);
	if (!s) 
		return EXIT_FAILURE;

	for (int i = 0; i < FREQ; i++)	
		pa_simple_write(s, samples, sizeof(samples), NULL);

	pa_simple_drain(s, NULL);
	pa_simple_free(s);

	return EXIT_SUCCESS;
}
