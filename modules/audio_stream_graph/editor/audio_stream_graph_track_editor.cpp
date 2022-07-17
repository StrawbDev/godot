#include "audio_stream_graph_track_editor.h"

float AudioStreamGraphTrackEditor::sample_space_to_control_space(uint64_t sample_position) const {
	float units_per_sample = get_size().x / m_scale;
	return (sample_position - m_offset) * units_per_sample;
}

uint64_t AudioStreamGraphTrackEditor::control_space_to_sample_space(float control_position) const {
	float units_per_sample = get_size().x / m_scale;
	return (control_position / units_per_sample) + m_offset;
}

void AudioStreamGraphTrackEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW:
			Ref<Font> default_font = get_theme_default_font();
			float baseline = default_font->get_ascent(get_theme_default_font_size());

			uint64_t zero_align = m_offset % m_step_size == 0 ? 0 : 1;
			uint64_t value = (m_offset / m_step_size) + zero_align;
			while (true) {
				float draw_pos = sample_space_to_control_space(value);
				String string_value = vformat("%0.1f", float(value) / m_sample_rate);
				draw_string(default_font, Point2(draw_pos, baseline), string_value);
				draw_line(Point2(draw_pos, baseline + 10), Point2(draw_pos, get_size().y), Color("white"));
				if (draw_pos > get_size().x) {
					break;
				}
				value += m_step_size;
			}
			break;
	}
}

AudioStreamGraphTrackEditor::AudioStreamGraphTrackEditor() {
	set_clip_contents(true);
}
