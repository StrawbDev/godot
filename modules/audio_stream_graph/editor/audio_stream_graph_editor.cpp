#include "audio_stream_graph_editor.h"
#include "audio_stream_graph_editor_nodes.h"
#include "scene/gui/label.h"

const Color AudioStreamGraphEditor::SLOT_COLOR_AUDIO = Color(1, 0, 0);
const Color AudioStreamGraphEditor::SLOT_COLOR_CONTROL = Color(1, 1, 1);

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

void AudioStreamGraphEditor::_on_add_parameter_pressed() {
	static int test = 0;
	ERR_FAIL_COND(m_current_resource == nullptr);
	StringName name = vformat("test%d", test);
	m_undo_redo->create_action(TTR("Add Parameter"));
	m_undo_redo->add_do_method(m_current_resource, "add_parameter", name, 0.0);
	m_undo_redo->add_undo_method(m_current_resource, "remove_parameter", name);
	m_undo_redo->commit_action();
	test++;
}

void AudioStreamGraphEditor::gui_input(const Ref<InputEvent> &p_event) {
	VSplitContainer::gui_input(p_event);

	ERR_FAIL_COND(p_event.is_null());
	Ref<InputEventMouseButton> mouse_event = p_event;
	if (mouse_event.is_valid() && mouse_event->get_button_index() == MouseButton::RIGHT && !mouse_event->is_pressed()) {
		AudioStreamGraphEditorNodeParameter *editor_node = memnew(AudioStreamGraphEditorNodeParameter);
		String resource_class = editor_node->get_node_resource_type();
		AudioStreamGraphNode *node_resource = Object::cast_to<AudioStreamGraphNode>(ClassDB::instantiate(resource_class));
		ERR_FAIL_COND(node_resource == nullptr);

		int idx = m_current_resource->next_node_id();
		editor_node->set_name(String::num_int64(idx));
		editor_node->set_node_resource(node_resource);

		m_undo_redo->create_action(TTR("Add Node"));
		m_undo_redo->add_do_reference(editor_node);
		m_undo_redo->add_do_method(this, "add_editor_node", editor_node);
		m_undo_redo->add_undo_method(this, "remove_editor_node", editor_node);
		m_undo_redo->add_do_method(m_current_resource, "add_node", node_resource);
		m_undo_redo->add_undo_method(m_current_resource, "remove_node", idx);
		m_undo_redo->commit_action();
	}
}

void AudioStreamGraphEditor::edit(AudioStreamGraph *resource) {
	m_current_resource = resource;
	ERR_FAIL_COND(m_current_resource == nullptr);

	clear_editor();

	Dictionary nodes = m_current_resource->get_nodes();
	for (int i = 0; i < nodes.size(); i++) {
		int id = nodes.get_key_at_index(i);
		const Ref<AudioStreamGraphNode> &node_resource = nodes.get_value_at_index(i);
		ERR_FAIL_COND(node_resource.is_null());

		StringName node_class = node_resource->get_class_name();
		AudioStreamGraphEditorNode *editor_node;
		if (node_class == "AudioStreamGraphNodeStream") {
			editor_node = memnew(AudioStreamGraphEditorNodeStream);
		} else if (node_class == "AudioStreamGraphNodeOutput") {
			editor_node = memnew(AudioStreamGraphEditorNodeOutput);
		} else if (node_class == "AudioStreamGraphNodeMix") {
			editor_node = memnew(AudioStreamGraphEditorNodeMix);
		} else if (node_class == "AudioStreamGraphNodeParameter") {
			AudioStreamGraphEditorNodeParameter *param_node = memnew(AudioStreamGraphEditorNodeParameter);
			param_node->set_valid_parameter_names(m_current_resource->get_parameter_names());
			editor_node = param_node;
		} else {
			ERR_FAIL_MSG(vformat("Can't edit unknown AudioStreamGraphNode type %s", node_class));
		}

		editor_node->set_name(String::num_int64(id));
		editor_node->set_node_resource(node_resource);
		editor_node->set_position_offset(node_resource->get_position());
		add_editor_node(editor_node);
	}

	Dictionary connections = m_current_resource->get_connections();
	for (int i = 0; i < connections.size(); i++) {
		String from_node = String::num_int64(connections.get_key_at_index(i));
		Array connection_list = connections.get_value_at_index(i);
		for (int j = 0; j < connection_list.size(); j++) {
			Array tuple = connection_list[j];
			ERR_FAIL_COND(tuple.size() != 3);
			String to_node = String::num_int64(tuple[1]);
			m_graph->connect_node(from_node, tuple[0], to_node, tuple[2]);
		}
	}

	m_track_editor->edit(resource);
}

void AudioStreamGraphEditor::add_editor_node(AudioStreamGraphEditorNode *editor_node) {
	editor_node->set_undo_redo(m_undo_redo);
	m_graph->add_child(editor_node);
}

void AudioStreamGraphEditor::remove_editor_node(AudioStreamGraphEditorNode *editor_node) {
	m_graph->remove_child(editor_node);
}

void AudioStreamGraphEditor::clear_editor() {
	m_graph->clear_connections();
	for (int i = m_graph->get_child_count(false) - 1; i >= 0; i--) {
		Node *node = m_graph->get_child(i, false);
		m_graph->remove_child(node);
		memfree(node);
	}
}

void AudioStreamGraphEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_editor_node", "editor_node"), &AudioStreamGraphEditor::add_editor_node);
	ClassDB::bind_method(D_METHOD("remove_editor_node", "editor_node"), &AudioStreamGraphEditor::remove_editor_node);
}

AudioStreamGraphEditor::AudioStreamGraphEditor() {
	m_graph = memnew(GraphEdit);
	m_graph->set_v_size_flags(SIZE_EXPAND_FILL);
	m_graph->add_valid_right_disconnect_type(SLOT_TYPE_AUDIO);
	m_graph->connect("connection_request", callable_mp(this, &AudioStreamGraphEditor::_on_connection_request));
	m_graph->connect("disconnection_request", callable_mp(this, &AudioStreamGraphEditor::_on_disconnection_request));
	add_child(m_graph);

	m_add_param_button = memnew(Button);
	m_add_param_button->set_text(TTR("Add Parameter"));
	m_add_param_button->connect("pressed", callable_mp(this, &AudioStreamGraphEditor::_on_add_parameter_pressed));
	m_graph->get_zoom_hbox()->add_child(m_add_param_button);

	m_track_editor = memnew(AudioStreamGraphTrackEditor);
	m_track_editor->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(m_track_editor);
}

void AudioStreamGraphEditor::set_undo_redo(UndoRedo *undo_redo) {
	m_undo_redo = undo_redo;
	m_track_editor->set_undo_redo(undo_redo);
}
