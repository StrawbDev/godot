#include "audio_stream_graph_editor.h"
#include "scene/gui/label.h"

#include "audio_stream_graph_editor_nodes.h"

const Color AudioStreamGraphEditor::SLOT_COLOR_AUDIO = Color(1, 0, 0);

void AudioStreamGraphEditor::_on_connection_request(StringName from, int from_slot, StringName to, int to_slot) {
	ERR_FAIL_COND(m_current_resource == nullptr);
	ERR_FAIL_COND(m_undo_redo == nullptr);

	int from_num = String(from).to_int();
	int to_num = String(to).to_int();
	m_undo_redo->create_action(TTR("Connect Node"));
	m_undo_redo->add_do_method(m_graph, "connect_node", from, from_slot, to, to_slot);
	m_undo_redo->add_undo_method(m_graph, "disconnect_node", from, from_slot, to, to_slot);
	m_undo_redo->add_do_method(m_current_resource, "add_connection", from_num, from_slot, to_num, to_slot);
	m_undo_redo->add_undo_method(m_current_resource, "remove_connection", from_num, from_slot, to_num, to_slot);
	m_undo_redo->commit_action();
}

void AudioStreamGraphEditor::_on_disconnection_request(StringName from, int from_slot, StringName to, int to_slot) {
	ERR_FAIL_COND(m_current_resource == nullptr);
	ERR_FAIL_COND(m_undo_redo == nullptr);

	int from_num = String(from).to_int();
	int to_num = String(to).to_int();
	m_undo_redo->create_action(TTR("Disconnect Node"));
	m_undo_redo->add_do_method(m_graph, "disconnect_node", from, from_slot, to, to_slot);
	m_undo_redo->add_undo_method(m_graph, "connect_node", from, from_slot, to, to_slot);
	m_undo_redo->add_do_method(m_current_resource, "remove_connection", from_num, from_slot, to_num, to_slot);
	m_undo_redo->add_undo_method(m_current_resource, "add_connection", from_num, from_slot, to_num, to_slot);
	m_undo_redo->commit_action();
}

void AudioStreamGraphEditor::edit(AudioStreamGraph *resource) {
	m_current_resource = resource;
	ERR_FAIL_COND(m_current_resource == nullptr);

	clear_editor();

	for (int i = 0; i < m_current_resource->num_nodes(); i++) {
		const Ref<AudioStreamGraphNode> &node_resource = m_current_resource->get_node(i);
		StringName node_class = node_resource->get_class_name();
		AudioStreamGraphEditorNode *editor_node;
		if (node_class == "AudioStreamGraphNodeStream") {
			editor_node = memnew(AudioStreamGraphEditorNodeStream);
		} else if (node_class == "AudioStreamGraphNodeOutput") {
			editor_node = memnew(AudioStreamGraphEditorNodeOutput);
		} else {
			ERR_FAIL_MSG(vformat("Can't edit unknown AudioStreamGraphNode type %s", node_class));
		}

		editor_node->set_name(String::num_int64(i));
		editor_node->set_position_offset(node_resource->get_position());
		editor_node->set_node_resource(node_resource);
		add_editor_node(editor_node);
	}

	PackedInt32Array connections = m_current_resource->get_connections();
	ERR_FAIL_COND(connections.size() % 4 != 0);
	for (int i = 0; i < connections.size() / 4; i += 4) {
		StringName from = String::num_int64(connections[i]);
		int from_port = connections[i + 1];
		StringName to = String::num_int64(connections[i + 2]);
		int to_port = connections[i + 3];
		m_graph->connect_node(from, from_port, to, to_port);
	}
}

void AudioStreamGraphEditor::add_editor_node(AudioStreamGraphEditorNode *editor_node) {
	editor_node->set_undo_redo(m_undo_redo);
	m_graph->add_child(editor_node);
}

void AudioStreamGraphEditor::clear_editor() {
	m_graph->clear_connections();
	for (int i = m_graph->get_child_count(false) - 1; i >= 0; i--) {
		Node *node = m_graph->get_child(i, false);
		m_graph->remove_child(node);
		memfree(node);
	}
}

AudioStreamGraphEditor::AudioStreamGraphEditor() {
	m_graph = memnew(GraphEdit);
	m_graph->set_v_size_flags(SIZE_EXPAND_FILL);
	m_graph->add_valid_right_disconnect_type(SLOT_TYPE_AUDIO);
	m_graph->connect("connection_request", callable_mp(this, &AudioStreamGraphEditor::_on_connection_request));
	m_graph->connect("disconnection_request", callable_mp(this, &AudioStreamGraphEditor::_on_disconnection_request));
	add_child(m_graph);
}

void AudioStreamGraphEditor::set_undo_redo(UndoRedo *undo_redo) {
	m_undo_redo = undo_redo;
}
