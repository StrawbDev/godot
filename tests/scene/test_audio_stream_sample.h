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

Vector<uint8_t> gen_mono_pcm8_test(float sample_rate, int sample_count) {
	Vector<uint8_t> buffer;
	buffer.resize(sample_count);
	uint8_t *write_ptr = buffer.ptrw();
	for (int i = 0; i < buffer.size(); i++) {
		// Formula for a 440Hz sin wave.
		float sample = Math::sin((Math_TAU * 440 / sample_rate) * i);
		// Map sin wave to full range of 8-bit values.
		uint8_t sample_8bit = Math::fast_ftoi(((sample + 1) / 2) * UINT8_MAX);
		// AudioStreamSample expects signed PCM8 data.
		uint8_t sample_8bit_signed = sample_8bit - 128;
		write_ptr[i] = sample_8bit_signed;
	}
	return buffer;
}

Vector<uint8_t> gen_mono_pcm16_test(float sample_rate, int sample_count) {
	Vector<uint8_t> buffer;
	buffer.resize(sample_count * 2);
	uint8_t *write_ptr = buffer.ptrw();
	for (int i = 0; i < buffer.size() / 2; i++) {
		// Formula for a 440Hz sin wave.
		float sample = Math::sin((Math_TAU * 440 / sample_rate) * i);
		// Map sin wave to full range of 16-bit values.
		uint16_t sample_16bit = Math::fast_ftoi(((sample + 1) / 2) * UINT16_MAX);
		encode_uint16(sample_16bit, write_ptr + (i * 2));
	}
	return buffer;
}

TEST_CASE("[AudioStreamSample] Mono PCM8 format") {
	String save_path = OS::get_singleton()->get_cache_path().plus_file("test_pcm8.wav");

	Vector<uint8_t> test_data = gen_mono_pcm8_test(SAMPLE_RATE, SAMPLE_COUNT);

	Ref<AudioStreamSample> stream = memnew(AudioStreamSample);
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

TEST_CASE("[AudioStreamSample] Mono PCM16 format") {
	String save_path = OS::get_singleton()->get_cache_path().plus_file("test_pcm16.wav");

	Vector<uint8_t> test_data = gen_mono_pcm16_test(SAMPLE_RATE, SAMPLE_COUNT);

	Ref<AudioStreamSample> stream = memnew(AudioStreamSample);
	stream->set_format(AudioStreamSample::Format::FORMAT_16_BITS);
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

} // namespace TestAudioStreamSample

#endif // TEST_AUDIO_STREAM_SAMPLE_H
