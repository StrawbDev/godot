#include "audio_stream_graph.h"
#include "audio_stream_playback_graph.h"

Ref<AudioStreamPlayback> AudioStreamGraph::instance_playback() {
	Ref<AudioStreamPlaybackGraph> playback;
	playback.instantiate();
	playback->set_resource(this);
	return playback;
}

String AudioStreamGraph::get_stream_name() const {
	return "AudioStreamGraph";
}

float AudioStreamGraph::get_length() const {
	return 0;
}

bool AudioStreamGraph::is_monophonic() const {
	return false;
}

void AudioStreamGraph::set_test_stream(Ref<AudioStream> test_stream) {
	m_test_stream = test_stream;
}

Ref<AudioStream> AudioStreamGraph::get_test_stream() {
	return m_test_stream;
}

void AudioStreamGraph::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_test_stream", "test_stream"), &AudioStreamGraph::set_test_stream);
	ClassDB::bind_method(D_METHOD("get_test_stream"), &AudioStreamGraph::get_test_stream);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "test_stream", PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"), "set_test_stream", "get_test_stream");
}
