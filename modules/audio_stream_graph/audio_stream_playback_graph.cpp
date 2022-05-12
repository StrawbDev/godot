#include "audio_stream_playback_graph.h"

void AudioStreamPlaybackGraph::set_resource(Ref<AudioStreamGraph> resource) {
	m_resource = resource;
	ERR_FAIL_COND(m_resource.is_null());
	if (m_resource.is_valid()) {
		if (m_resource->get_test_stream().is_valid()) {
			m_test_playback = m_resource->get_test_stream()->instance_playback();
		}
	}
}

void AudioStreamPlaybackGraph::start(float p_from_pos) {
	if (m_test_playback.is_valid()) {
		m_test_playback->start(p_from_pos);
	}
}

void AudioStreamPlaybackGraph::stop() {
	if (m_test_playback.is_valid()) {
		m_test_playback->stop();
	}
}

bool AudioStreamPlaybackGraph::is_playing() const {
	if (m_test_playback.is_valid()) {
		return m_test_playback->is_playing();
	} else {
		return false;
	}
}

int AudioStreamPlaybackGraph::get_loop_count() const {
	if (m_test_playback.is_valid()) {
		return m_test_playback->get_loop_count();
	} else {
		return 0;
	}
}

float AudioStreamPlaybackGraph::get_playback_position() const {
	if (m_test_playback.is_valid()) {
		return m_test_playback->get_playback_position();
	} else {
		return 0;
	}
}

void AudioStreamPlaybackGraph::seek(float p_time) {
	if (m_test_playback.is_valid()) {
		return m_test_playback->seek(p_time);
	}
}

int AudioStreamPlaybackGraph::mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) {
	if (m_test_playback.is_valid()) {
		return m_test_playback->mix(p_buffer, p_rate_scale, p_frames);
	} else {
		return 0;
	}
}