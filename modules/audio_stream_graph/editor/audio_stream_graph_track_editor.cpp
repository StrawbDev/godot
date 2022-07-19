#include "audio_stream_graph_track_editor.h"

///////////////////////////////////
// AudioStreamGraphTrackEditor

AudioStreamGraphTrackEditor::AudioStreamGraphTrackEditor() {
	AudioStreamGraphTimelineEditor *temp = memnew(AudioStreamGraphTimelineEditor);
	temp->set_anchors_preset(LayoutPreset::PRESET_WIDE);
	add_child(temp);
}

///////////////////////////////////
// AudioStreamGraphTimelineEditor

float AudioStreamGraphTimelineEditor::sample_space_to_control_space(uint64_t sample_position) const {
	ERR_FAIL_COND_V(m_offset > sample_position, 0);
	float units_per_sample = get_size().x / m_scale;
	return (sample_position - m_offset) * units_per_sample;
}

uint64_t AudioStreamGraphTimelineEditor::control_space_to_sample_space(float control_position) const {
	float units_per_sample = get_size().x / m_scale;
	return (control_position / units_per_sample) + m_offset;
}

void AudioStreamGraphTimelineEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW:
			Ref<Font> default_font = get_theme_default_font();
			float baseline = default_font->get_ascent(get_theme_default_font_size());

			uint64_t zero_align = m_offset % m_step_size == 0 ? 0 : 1;
			uint64_t value = m_step_size * ((m_offset / m_step_size) + zero_align);
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

void AudioStreamGraphTimelineEditor::gui_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventMouseButton> button_event = p_event;
	if (button_event.is_valid()) {
		float scroll_delta = button_event->get_factor();
		if (scroll_delta == 0.0f) {
			scroll_delta = 1.0f;
		}

		if (button_event->get_button_index() == MouseButton::MIDDLE) {
			m_dragging = button_event->is_pressed();
		} else if (button_event->get_button_index() == MouseButton::WHEEL_UP) {
			m_scale -= scroll_delta * ZOOM_SPEED * m_scale;
			accept_event();
		} else if (button_event->get_button_index() == MouseButton::WHEEL_DOWN) {
			m_scale += scroll_delta * ZOOM_SPEED * m_scale;
			accept_event();
		} else if (button_event->get_button_index() == MouseButton::RIGHT) {
			// TODO
		}

		if (m_scale > m_step_size * 20) {
			m_step_size *= 2;
		} else if (m_step_size > m_scale / 5) {
			m_step_size /= 2;
		}

		update();
	}

	Ref<InputEventMouseMotion> motion_event = p_event;
	if (motion_event.is_valid()) {
		if (m_dragging) {
			int dir = SIGN(-motion_event->get_relative().x);
			uint64_t delta = control_space_to_sample_space(Math::abs(motion_event->get_relative().x));
			delta -= MIN(delta, m_offset);
			if (dir == -1) {
				delta = MIN(delta, m_offset);
				m_offset -= delta;
			} else {
				m_offset += delta;
			}
			accept_event();
			update();
		}
	}
}

AudioStreamGraphTimelineEditor::AudioStreamGraphTimelineEditor() {
	set_clip_contents(true);
}

/////////////////////////////////////
// AudioStreamGraphTrackItemEditor
