#ifndef AUDIO_STREAM_GRAPH_EDITOR_H
#define AUDIO_STREAM_GRAPH_EDITOR_H

#include "../audio_stream_graph.h"
#include "audio_stream_graph_editor_nodes.h"
#include "core/object/undo_redo.h"
#include "scene/gui/box_container.h"
#include "scene/gui/graph_edit.h"

class AudioStreamGraphEditor : public VBoxContainer {
	GDCLASS(AudioStreamGraphEditor, VBoxContainer);

private:
	UndoRedo *m_undo_redo = nullptr;
	GraphEdit *m_graph = nullptr;
	AudioStreamGraph *m_current_resource = nullptr;

	void _on_connection_request(StringName from, int from_slot, StringName to, int to_slot);
	void _on_disconnection_request(StringName from, int from_slot, StringName to, int to_slot);

protected:
	static void _bind_methods();

public:
	virtual void gui_input(const Ref<InputEvent> &p_event) override;

	enum SlotTypes {
		SLOT_TYPE_AUDIO = 0,
		SLOT_TYPE_CONTROL = 1,
	};

	static const Color SLOT_COLOR_AUDIO;
	static const Color SLOT_COLOR_CONTROL;

	void edit(AudioStreamGraph *resource);

	void remove_editor_node(AudioStreamGraphEditorNode *editor_node);
	void add_editor_node(AudioStreamGraphEditorNode *editor_node);
	void clear_editor();

	AudioStreamGraphEditor();
	void set_undo_redo(UndoRedo *undo_redo);
};

#endif // AUDIO_STREAM_GRAPH_EDITOR_H
