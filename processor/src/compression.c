#include "compression.h"
#include "audio.h"
#include "ipc.h"
#include <math.h>

bool attack_stage = false;
bool main_stage = false;
bool compress_stage = false;
bool release_stage = false;

unsigned int attack_counter;
unsigned int attack_counter_aim;
unsigned int release_counter;
unsigned int release_counter_aim;

void compress_routine(double *data, double divider_start, double divider_end)
{
	unsigned int i;
	double step = (divider_end - divider_start) / (double)audio_period_size_frames;

	for (i = 0; i < audio_period_size_frames; i++)
		data[i] /= (divider_start + (step * i));
}

void compression(double *data)
{
	unsigned int i;
	double peek = 0.0;
	bool threshold_reached = false;
	double compression_divider;
	double compression_divider_start;
	double compression_divider_end;
	bool apply_compression = false;

	if (!ipc_comp->enable)
		return;

	/* determine peek level */
	double abs_value;
	for (i = 0; i < audio_period_size_frames; i++) {
		abs_value = fabs(data[i]);
		if (abs_value > peek)
			peek = abs_value;
	}

	/* determine if threshold value has been reached */
	if (peek > ipc_comp->threshold)
		threshold_reached = true;

	/* calculate compression divider using peek level, threshold level and compression ratio */
	compression_divider = peek / (ipc_comp->threshold + ((peek - ipc_comp->threshold) / ipc_comp->ratio));

	/* attack_stage */
	if (attack_stage) {
		attack_counter++;

		if (threshold_reached) {
			apply_compression = true;
			compression_divider_start = 1.0 + ((compression_divider - 1.0) * ((double)attack_counter / (double)attack_counter_aim));
			compression_divider_end = 1.0 + ((compression_divider - 1.0) * ((double)(attack_counter + 1) / (double)attack_counter_aim));
		}

		if (attack_counter == (attack_counter_aim - 1)) {
			printf("main\n");
			attack_stage = false;
			main_stage = true;
		}
	/* main stage */
	} else if (main_stage) {
		if (threshold_reached) {
			apply_compression = true;
			compression_divider_start = compression_divider;
			compression_divider_end = compression_divider;
		} else {
			printf("release\n");
			main_stage = false;
			release_stage = true;
			release_counter = 1;
			release_counter_aim = ipc_comp->release_ms / (AUDIO_PERIOD_SEC * 1000.0);
		}
	/* release stage */
	} else if (release_stage) {
		release_counter++;

		if (threshold_reached) {
			apply_compression = true;
			compression_divider_start = 1.0 + ((compression_divider - 1.0) * ((double)(release_counter_aim - release_counter) / (double)release_counter_aim));
			compression_divider_end = 1.0 + ((compression_divider - 1.0) * ((double)(release_counter_aim - (release_counter + 1)) / (double)release_counter_aim));
		}

		if (release_counter == (release_counter_aim - 1)) {
			printf("monitoring\n");
			release_stage = false;
		}
	/* monitoring stage */
	} else {
		if (threshold_reached) {
			printf("attack\n");
			attack_stage = true;
			attack_counter = 0;
			attack_counter_aim = ipc_comp->attack_ms / (AUDIO_PERIOD_SEC * 1000.0);
		}
	}

	if (apply_compression) {
		compress_routine(data, compression_divider_start, compression_divider_end);
		printf("%f %f %f\n", compression_divider, compression_divider_start, compression_divider_end);
	}
}
