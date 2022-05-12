#ifndef AUDIO_STREAM_GENERATIVE_EDITOR_H
#define AUDIO_STREAM_GENERATIVE_EDITOR_H

#include "modules/audio_stream_generative/audio_stream_generative.h"
#include "scene/gui/box_container.h"
#include "scene/gui/graph_edit.h"

class AudioStreamGenerativeEditor : public VBoxContainer {
	GDCLASS(AudioStreamGenerativeEditor, VBoxContainer);

private:
	GraphEdit *m_graph;
	AudioStreamGenerative *m_current_resource = nullptr;

	void _on_connection_request(StringName from, int from_slot, StringName to, int to_slot);
	void _on_disconnection_request(StringName from, int from_slot, StringName to, int to_slot);

public:
	enum SlotTypes {
		SLOT_TYPE_AUDIO = 0,
	};

	static const Color SLOT_COLOR_AUDIO;

	void edit(AudioStreamGenerative *resource);

	AudioStreamGenerativeEditor();
};

#endif
