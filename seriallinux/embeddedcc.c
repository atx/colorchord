//
// This is the teststrap for the Embedded ColorChord System.
// It is intended as a minimal scaffolding for testing Embedded ColorChord.
//
//
// Copyright 2015 <>< Charles Lohr, See LICENSE file for license info.


#include "embeddednf.h"
#include <assert.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>   // Added by [olel] for atoi
#include <stdbool.h>
#include "embeddedout.h"

struct sockaddr_in servaddr;
int sock;

#define expected_lights NUM_LIN_LEDS

static FILE *fout;

void NewFrame()
{
	// A single bit is ~450ns
	uint8_t buffer[2048];
	memset(buffer, 0, sizeof(buffer));
	int i = 0;
	int bit_i = 0;

	void push_bit(bool bit) {
		if (bit_i == 8) {
			bit_i = 0;
			i++;
			assert(i < sizeof(buffer) - 1);
		}
		if (bit) {
			buffer[i] |= 1 << bit_i;
		}
		bit_i++;
	}

	void push_ws_bit(bool bit) {
		if (bit) {
			push_bit(1);
			push_bit(1);
			push_bit(0);
		} else {
			push_bit(1);
			push_bit(0);
			push_bit(0);
		}
	}

	void push_ws_byte(uint8_t byte) {
		for (int i = 7; i >= 0; i--) {
			push_ws_bit(!!(byte & (1 << i)));
		}
	}

	// Formatted WS2812B bytes
	for (int i = 0; i < expected_lights; i++) {
		push_ws_byte(ledOut[i*3+1]);
		push_ws_byte(ledOut[i*3+0]);
		push_ws_byte(ledOut[i*3+2]);
	}

	// Write the buffer
	fwrite(buffer, 1, i, fout);
	// Control character
	fputc(0xff, fout);
	fflush(fout);

	HandleFrameInfo();
	UpdateLinearLEDs();
}


int main( int argc, char ** argv )
{
	int ci;

	if (argc < 2) {
		fprintf(stderr, "Error: usage: <outfile>\n");
		return -1;
	}

	fout = fopen(argv[1], "w");
	if (fout == NULL) {
		fprintf(stderr, "Failed to open file '%s'\n", argv[1]);
	}

	InitColorChord();  // Changed by [olel] cause this was changed in embeddednf

	int wf = 0;
	while (true) {
		uint8_t data[2];
		int ret = fread(data, 1, sizeof(data), stdin);
		if (ret != 2) {
			fprintf(stderr, "Input read failed\n");
			return EXIT_FAILURE;
		}
		int16_t sample = data[0] | (data[1] << 8);
		PushSample32(sample / 2);
		if (wf++ == 128) {
			NewFrame();
			wf = 0;
		}
	}

	return 0;
}

