#ifndef AUDIO_STREAM_GRAPH_EDITOR_NODES_H
#define AUDIO_STREAM_GRAPH_EDITOR_NODES_H

#include "../audio_stream_graph_nodes.h"
#include "core/object/undo_redo.h"
#include "editor/editor_resource_picker.h"
#include "scene/gui/graph_node.h"

class AudioStreamGraphEditorNode : public GraphNode {
	GDCLASS(AudioStreamGraphEditorNode, GraphNode);

protected:
	UndoRedo *m_undo_redo = nullptr;

	static void _bind_methods();
	void _on_dragged(Vector2 from, Vector2 to);
	void _notification(int p_notification);

public:
	virtual String get_node_resource_type() = 0;
	virtual void set_node_resource(Ref<AudioStreamGraphNode> node_resource) = 0;
	virtual Ref<AudioStreamGraphNode> get_node_resource() const = 0;

	void set_undo_redo(UndoRedo *undo_redo);
	AudioStreamGraphEditorNode();
};

class AudioStreamGraphEditorNodeStream : public AudioStreamGraphEditorNode {
	GDCLASS(AudioStreamGraphEditorNodeStream, AudioStreamGraphEditorNode);

private:
	Ref<AudioStreamGraphNodeStream> m_node_resource;
	EditorResourcePicker *m_picker = nullptr;

	void _on_picker_resource_changed(Ref<Resource> resource);
	void _on_node_resource_changed();

protected:
	static void _bind_methods();

public:
	virtual String get_node_resource_type() override;
	virtual void set_node_resource(Ref<AudioStreamGraphNode> node_resource) override;
	virtual Ref<AudioStreamGraphNode> get_node_resource() const override;

	AudioStreamGraphEditorNodeStream();
};

#endif // AUDIO_STREAM_GRAPH_EDITOR_NODES_H