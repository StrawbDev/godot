#include "modules/audio_stream_generative/audio_stream_generative.h"
#include "modules/audio_stream_generative/audio_stream_playback_generative.h"

Ref<AudioStreamPlayback> AudioStreamGenerative::instance_playback() {
	Ref<AudioStreamPlaybackGenerative> playback;
	playback.instantiate();
	playback->set_resource(this);
	return playback;
}

String AudioStreamGenerative::get_stream_name() const {
	return "AudioStreamGenerative";
}

float AudioStreamGenerative::get_length() const {
	return 0;
}

bool AudioStreamGenerative::is_monophonic() const {
	return false;
}

void AudioStreamGenerative::set_test_stream(Ref<AudioStream> test_stream) {
	m_test_stream = test_stream;
}

Ref<AudioStream> AudioStreamGenerative::get_test_stream() {
	return m_test_stream;
}

void AudioStreamGenerative::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_test_stream", "test_stream"), &AudioStreamGenerative::set_test_stream);
	ClassDB::bind_method(D_METHOD("get_test_stream"), &AudioStreamGenerative::get_test_stream);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "test_stream", PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"), "set_test_stream", "get_test_stream");
}
