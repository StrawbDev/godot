#include "audio_stream_playback_graph.h"

int AudioStreamPlaybackGraph::get_sample_rate() const {
#warning "TODO: proper sample rate support. Currently everything must be 44100Hz to work"
	return 44100;
}

void AudioStreamPlaybackGraph::set_resource(Ref<AudioStreamGraph> resource) {
	m_resource = resource;
}

void AudioStreamPlaybackGraph::start(float p_from_pos) {
	seek(p_from_pos);
	m_playing = true;
}

void AudioStreamPlaybackGraph::stop() {
	m_playing = false;
}

bool AudioStreamPlaybackGraph::is_playing() const {
	return m_playing;
}

int AudioStreamPlaybackGraph::get_loop_count() const {
	return 0;
}

float AudioStreamPlaybackGraph::get_playback_position() const {
	return m_position / float(get_sample_rate());
}

void AudioStreamPlaybackGraph::seek(float p_time) {
	if (p_time < 0) {
		m_position = 0;
	} else {
		m_position = p_time * get_sample_rate();
	}
}

int AudioStreamPlaybackGraph::mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) {
#warning "TODO"
	return -1;
}