#ifndef AUDIO_STREAM_GRAPH_EDITOR_H
#define AUDIO_STREAM_GRAPH_EDITOR_H

#include "../audio_stream_graph.h"
#include "audio_stream_graph_editor_nodes.h"
#include "scene/gui/box_container.h"
#include "scene/gui/graph_edit.h"

class AudioStreamGraphEditor : public VBoxContainer {
	GDCLASS(AudioStreamGraphEditor, VBoxContainer);

private:
	GraphEdit *m_graph;
	AudioStreamGraph *m_current_resource = nullptr;

	void _on_connection_request(StringName from, int from_slot, StringName to, int to_slot);
	void _on_disconnection_request(StringName from, int from_slot, StringName to, int to_slot);

public:
	enum SlotTypes {
		SLOT_TYPE_AUDIO = 0,
	};

	static const Color SLOT_COLOR_AUDIO;

	void edit(AudioStreamGraph *resource);

	void add_editor_node(AudioStreamGraphEditorNode *editor_node);

	AudioStreamGraphEditor();
};

#endif // AUDIO_STREAM_GRAPH_EDITOR_H
