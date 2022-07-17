#ifndef AUDIO_STREAM_GRAPH_TRACK_EDITOR_H
#define AUDIO_STREAM_GRAPH_TRACK_EDITOR_H

#include "scene/gui/control.h"

class AudioStreamGraphTrackEditor : public Control {
	GDCLASS(AudioStreamGraphTrackEditor, Control);

private:
	static constexpr uint64_t DEFAULT_SAMPLE_RATE = 41000;

	uint64_t m_offset = 0;
	uint64_t m_scale = DEFAULT_SAMPLE_RATE * 10;
	uint64_t m_step_size = DEFAULT_SAMPLE_RATE;
	uint64_t m_sample_rate = DEFAULT_SAMPLE_RATE;

	float sample_space_to_control_space(uint64_t sample_position) const;
	uint64_t control_space_to_sample_space(float control_position) const;

protected:
	void _notification(int p_what);

public:
	AudioStreamGraphTrackEditor();
};

#endif // AUDIO_STREAM_GRAPH_TRACK_EDITOR_H