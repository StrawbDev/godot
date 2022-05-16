#include "audio_stream_graph_editor.h"
#include "scene/gui/label.h"

#include "audio_stream_graph_editor_nodes.h"

const Color AudioStreamGraphEditor::SLOT_COLOR_AUDIO = Color(1, 0, 0);

void AudioStreamGraphEditor::_on_connection_request(StringName from, int from_slot, StringName to, int to_slot) {
	m_graph->connect_node(from, from_slot, to, to_slot);
}

void AudioStreamGraphEditor::_on_disconnection_request(StringName from, int from_slot, StringName to, int to_slot) {
	m_graph->disconnect_node(from, from_slot, to, to_slot);
}

void AudioStreamGraphEditor::edit(AudioStreamGraph *resource) {
	m_current_resource = resource;
	ERR_FAIL_COND(m_current_resource == nullptr);
}

void AudioStreamGraphEditor::add_editor_node(AudioStreamGraphEditorNode *editor_node) {
	ERR_FAIL_COND(m_current_resource == nullptr);
	m_graph->add_child(editor_node);
	m_current_resource->add_node(editor_node->get_node_resource());
}

AudioStreamGraphEditor::AudioStreamGraphEditor() {
	m_graph = memnew(GraphEdit);
	m_graph->set_v_size_flags(SIZE_EXPAND_FILL);
	m_graph->add_valid_right_disconnect_type(SLOT_TYPE_AUDIO);
	m_graph->connect("connection_request", callable_mp(this, &AudioStreamGraphEditor::_on_connection_request));
	m_graph->connect("disconnection_request", callable_mp(this, &AudioStreamGraphEditor::_on_disconnection_request));
	add_child(m_graph);
}
