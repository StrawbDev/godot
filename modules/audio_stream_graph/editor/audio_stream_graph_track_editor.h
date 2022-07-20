#ifndef AUDIO_STREAM_GRAPH_TRACK_EDITOR_H
#define AUDIO_STREAM_GRAPH_TRACK_EDITOR_H

#include "scene/gui/control.h"
#include "scene/gui/panel_container.h"

/////////////////////////////////
// AudioStreamGraphTrackEditor
class AudioStreamGraphTrackEditor : public Control {
	GDCLASS(AudioStreamGraphTrackEditor, Control);

public:
	AudioStreamGraphTrackEditor();
};

/////////////////////////////////
// AudioStreamGraphTimelineEditor

class AudioStreamGraphTimelineEditor : public Control {
	GDCLASS(AudioStreamGraphTimelineEditor, Control);

private:
	static constexpr uint64_t DEFAULT_SAMPLE_RATE = 41000;
	static constexpr float ZOOM_SPEED = 0.1;
	static constexpr float PAN_SPEED = 0.001;

	uint64_t m_offset = 0;
	uint64_t m_scale = DEFAULT_SAMPLE_RATE * 10;
	uint64_t m_step_size = DEFAULT_SAMPLE_RATE;
	uint64_t m_sample_rate = DEFAULT_SAMPLE_RATE;

	bool m_dragging = false;

	float sample_space_to_control_space(uint64_t sample_position) const;
	uint64_t control_space_to_sample_space(float control_position) const;

protected:
	void _notification(int p_what);

public:
	virtual void gui_input(const Ref<InputEvent> &p_event) override;
	AudioStreamGraphTimelineEditor();
};

///////////////////////////////////////
// AudioStreamGraphTrackItemEditor

class AudioStreamGraphTrackItemEditor : public PanelContainer {
	GDCLASS(AudioStreamGraphTrackItemEditor, PanelContainer);

private:
	constexpr static int RESIZE_DEADZONE = 20;
	enum class ResizeSide {
		None,
		Left,
		Right
	};

	ResizeSide m_resize_side;
	Point2 m_drag_point = Vector2(-1, -1);

	ResizeSide is_inside_resize_deadzone(Point2 point) const;

public:
	virtual void gui_input(const Ref<InputEvent> &p_event) override;
	virtual CursorShape get_cursor_shape(const Point2 &p_pos = Point2i()) const override;
	AudioStreamGraphTrackItemEditor();
};

#endif // AUDIO_STREAM_GRAPH_TRACK_EDITOR_H