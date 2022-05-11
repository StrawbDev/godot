#include "modules/audio_stream_generative/audio_stream_playback_generative.h"

void AudioStreamPlaybackGenerative::set_resource(Ref<AudioStreamGenerative> resource) {
	m_resource = resource;
	ERR_FAIL_COND(m_resource.is_null());
	if (m_resource.is_valid()) {
		if (m_resource->get_test_stream().is_valid()) {
			m_test_playback = m_resource->get_test_stream()->instance_playback();
		}
	}
}

void AudioStreamPlaybackGenerative::start(float p_from_pos) {
	if (m_test_playback.is_valid()) {
		m_test_playback->start(p_from_pos);
	}
}

void AudioStreamPlaybackGenerative::stop() {
	if (m_test_playback.is_valid()) {
		m_test_playback->stop();
	}
}

bool AudioStreamPlaybackGenerative::is_playing() const {
	if (m_test_playback.is_valid()) {
		return m_test_playback->is_playing();
	} else {
		return false;
	}
}

int AudioStreamPlaybackGenerative::get_loop_count() const {
	if (m_test_playback.is_valid()) {
		return m_test_playback->get_loop_count();
	} else {
		return 0;
	}
}

float AudioStreamPlaybackGenerative::get_playback_position() const {
	if (m_test_playback.is_valid()) {
		return m_test_playback->get_playback_position();
	} else {
		return 0;
	}
}

void AudioStreamPlaybackGenerative::seek(float p_time) {
	if (m_test_playback.is_valid()) {
		return m_test_playback->seek(p_time);
	}
}

int AudioStreamPlaybackGenerative::mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) {
	if (m_test_playback.is_valid()) {
		return m_test_playback->mix(p_buffer, p_rate_scale, p_frames);
	} else {
		return 0;
	}
}