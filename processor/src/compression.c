#include "compression.h"
#include "audio.h"
#include "ipc.h"
#include <math.h>

bool attack_stage = false;
bool compress_stage = false;
bool release_stage = false;

unsigned int attack_counter = 0;
unsigned int release_counter = 0;

void compress_routine(double *data, double divider)
{
	unsigned int i;

	for (i = 0; i < audio_period_size_frames; i++)
		data[i] /= divider;
}

void compression(double *data)
{
	unsigned int i;
	double peek = 0.0;
	bool threshold_reach = false;
	double divider = 1.0;

	/* determine peek level */
	double abs_value;
	for (i = 0; i < audio_period_size_frames; i++) {
		abs_value = fabs(data[i]);
		if (abs_value > peek)
			peek = abs_value;
	}

	/* determine if threshold value has been reached */
	if (peek > ipc_comp->threshold) {
		threshold_reach = true;
		divider = peek / (ipc_comp->threshold + ((peek - ipc_comp->threshold) / ipc_comp->ratio));
	}

	/* attack stage */
	if (attack_stage) {
		if (threshold_reach) {
			attack_counter++;
			if ((attack_counter * AUDIO_PERIOD_SEC) > (ipc_comp->attack_ms / 1000.0)) {
				attack_stage = false;
				compress_stage = true;
			}
		} else {
			attack_stage = false;
		}
	/* compression stage */
	} else if (compress_stage) {
		compress_routine(data, divider);

		if (!threshold_reach) {
			compress_stage = false;
			release_stage = true;
			release_counter = 0;
		}
	/* release stage */
	} else if (release_stage) {
		compress_routine(data, divider);
		if (threshold_reach) {
			release_stage = false;
			compress_stage = true;
		} else {
			release_counter++;
			if ((release_counter * AUDIO_PERIOD_SEC) > (ipc_comp->release_ms / 1000.0)) {
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
