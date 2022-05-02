/*************************************************************************/
/* test_audio_stream_sample.h                                            */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef TEST_AUDIO_STREAM_SAMPLE_H
#define TEST_AUDIO_STREAM_SAMPLE_H

#include "core/io/marshalls.h"
#include "core/math/math_defs.h"
#include "core/templates/local_vector.h"
#include "scene/resources/audio_stream_sample.h"
#include "servers/audio/audio_driver_dummy.h"
#include "tests/test_macros.h"

namespace TestAudioStreamSample {

// Default sample rate for test cases.
constexpr float SAMPLE_RATE = 44100;
// Default sample count for test cases, 3 seconds of audio.
constexpr int SAMPLE_COUNT = SAMPLE_RATE * 3;

float gen_sample(float frequency, float sample_rate, int sample_number) {
	// formula for generating a sin wave with given frequency.
	return Math::sin((Math_TAU * frequency / sample_rate) * sample_number);
}

Vector<uint8_t> gen_pcm8_test(float sample_rate, int sample_count, bool stereo) {
	Vector<uint8_t> buffer;
	buffer.resize(stereo ? sample_count * 2 : sample_count);
	uint8_t *write_ptr = buffer.ptrw();
	for (int i = 0; i < buffer.size(); i++) {
		float sample;
		if (stereo) {
			if (i % 2 == 0) {
				sample = gen_sample(440, sample_rate, i / 2);
			} else {
				sample = gen_sample(261.63, sample_rate, i / 2);
			}
		} else {
			sample = gen_sample(440, sample_rate, i);
		}
		// Map sin wave to full range of 8-bit values.
		uint8_t sample_8bit = Math::fast_ftoi(((sample + 1) / 2) * UINT8_MAX);
		// Unlike the .wav format, AudioStreamSample expects signed 8-bit samples.
		uint8_t sample_8bit_signed = sample_8bit - (INT8_MAX + 1);
		write_ptr[i] = sample_8bit_signed;
	}
	return buffer;
}

Vector<uint8_t> gen_pcm16_test(float sample_rate, int sample_count, bool stereo) {
	Vector<uint8_t> buffer;
	buffer.resize(stereo ? sample_count * 4 : sample_count * 2);
	uint8_t *write_ptr = buffer.ptrw();
	for (int i = 0; i < buffer.size() / 2; i++) {
		float sample;
		if (stereo) {
			if (i % 2 == 0) {
				sample = gen_sample(440, sample_rate, i / 2);
			} else {
				sample = gen_sample(261.63, sample_rate, i / 2);
			}
		} else {
			sample = gen_sample(440, sample_rate, i);
		}
		// Map sin wave to full range of 16-bit values.
		uint16_t sample_16bit = Math::fast_ftoi(((sample + 1) / 2) * UINT16_MAX);
		// The .wav format expects samples larger than 8 bits to be signed.
		uint16_t sample_16bit_signed = sample_16bit - (INT16_MAX + 1);
		encode_uint16(sample_16bit_signed, write_ptr + (i * 2));
	}
	return buffer;
}

void run_test(String file_name, AudioStreamSample::Format data_format, bool stereo) {
	String save_path = OS::get_singleton()->get_cache_path().plus_file(file_name);

	Vector<uint8_t> test_data;
	if (data_format == AudioStreamSample::FORMAT_8_BITS) {
		test_data = gen_pcm8_test(SAMPLE_RATE, SAMPLE_COUNT, stereo);
	} else {
		test_data = gen_pcm16_test(SAMPLE_RATE, SAMPLE_COUNT, stereo);
	}

	Ref<AudioStreamSample> stream = memnew(AudioStreamSample);
	stream->set_format(data_format);
	stream->set_stereo(stereo);
	stream->set_data(test_data);

	SUBCASE("Stream length is computed properly") {
		CHECK(Math::is_equal_approx(stream->get_length(), 3));
	}

	SUBCASE("Stream can be saved as .wav") {
		REQUIRE(stream->save_to_wav(save_path) == OK);

		Error error;
		Ref<FileAccess> wav_file = FileAccess::open(save_path, FileAccess::ModeFlags::READ, &error);
		REQUIRE(error == OK);
	}
}

TEST_CASE("[AudioStreamSample] Mono PCM8 format") {
	run_test("test_pcm8_mono.wav", AudioStreamSample::FORMAT_8_BITS, false);
}

TEST_CASE("[AudioStreamSample] Mono PCM16 format") {
	run_test("test_pcm16_mono.wav", AudioStreamSample::FORMAT_16_BITS, false);
}

TEST_CASE("[AudioStreamSample] Stereo PCM8 format") {
	run_test("test_pcm8_stereo.wav", AudioStreamSample::FORMAT_8_BITS, true);
}

TEST_CASE("[AudioStreamSample] Stereo PCM16 format") {
	run_test("test_pcm16_stereo.wav", AudioStreamSample::FORMAT_16_BITS, true);
}

} // namespace TestAudioStreamSample

#endif // TEST_AUDIO_STREAM_SAMPLE_H
