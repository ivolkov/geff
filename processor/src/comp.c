#include "comp.h"
#include "audio.h"
#include <math.h>

#define ATTACK_LIMIT 5
#define RELEASE_LIMIT 50

const unsigned int threshold = 5000.0;
const double ratio = 4.0;
const double attack_ms = 20.0;
const double release_ms = 100.0;

bool attack_stage = false;
bool compress_stage = false;
bool release_stage = false;

unsigned int attack_counter = 0;
unsigned int release_counter = 0;

void compress_routine(double *data)
{
	unsigned int i;

	for (i = 0; i < audio_period_size_frames; i++)
		data[i] /= ratio;
}

void compress(double *data)
{
	unsigned int i;
	bool threshold_reach = false;

	/* determine if threshold level has been reached */
	for (i = 0; i < audio_period_size_frames; i++)
		if (fabs(data[i]) > threshold) {
			threshold_reach = true;
			break;
		}

	/* attack stage */
	if (attack_stage) {
		if (threshold_reach) {
			attack_counter++;
			if (attack_counter > ATTACK_LIMIT) {
				attack_stage = false;
				compress_stage = true;
			}
		} else {
			attack_stage = false;
		}
	/* compression stage */
	} else if (compress_stage) {
		compress_routine(data);

		if (!threshold_reach) {
			compress_stage = false;
			release_stage = true;
			release_counter = 0;
		}
	/* release stage */
	} else if (release_stage) {
		compress_routine(data);
		if (threshold_reach) {
			release_stage = false;
			compress_stage = true;
		} else {
			release_counter++;
			if (release_counter > RELEASE_LIMIT) {
				release_stage = false;
			}
		}
	/* monitoring stage */
	} else {
		if (threshold_reach) {
			attack_stage = true;
			attack_counter = 0;
		}
	}
}
