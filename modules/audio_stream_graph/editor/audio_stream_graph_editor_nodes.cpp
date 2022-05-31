#include "audio_stream_graph_editor_nodes.h"
#include "audio_stream_graph_editor.h"
#include "scene/gui/label.h"

// AudioStreamGraphEditorNode

void AudioStreamGraphEditorNode::_notification(int p_notification) {
	switch (p_notification) {
		case NOTIFICATION_READY:
			connect("dragged", callable_mp(this, &AudioStreamGraphEditorNode::_on_dragged));
			break;
	}
}

void AudioStreamGraphEditorNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_node_resource"), &AudioStreamGraphEditorNode::get_node_resource);
}

void AudioStreamGraphEditorNode::set_undo_redo(UndoRedo *undo_redo) {
	m_undo_redo = undo_redo;
}

void AudioStreamGraphEditorNode::_on_dragged(Vector2 from, Vector2 to) {
	Ref<AudioStreamGraphNode> node_resource = get_node_resource();
	m_undo_redo->create_action(TTR("Change Node Position"));
	m_undo_redo->add_do_property(node_resource.ptr(), "position", to);
	m_undo_redo->add_undo_property(node_resource.ptr(), "position", from);
	m_undo_redo->add_do_property(this, "position_offset", to);
	m_undo_redo->add_undo_property(this, "position_offset", from);
	m_undo_redo->commit_action();
}

AudioStreamGraphEditorNode::AudioStreamGraphEditorNode() {
}

////////////////////////////
// AudioStreamGraphEditorNodeStream

void AudioStreamGraphEditorNodeStream::_on_picker_resource_changed(Ref<Resource> resource) {
	if (m_node_resource.is_valid()) {
		Ref<AudioStream> old_stream = m_node_resource->get_stream();
		if (resource != old_stream) {
			m_undo_redo->create_action(TTR("Change Stream"));
			m_undo_redo->add_do_property(m_node_resource.ptr(), "stream", resource);
			m_undo_redo->add_undo_property(m_node_resource.ptr(), "stream", old_stream);
			m_undo_redo->commit_action();
		}
	}
}

void AudioStreamGraphEditorNodeStream::_on_node_resource_changed() {
	if (m_node_resource.is_valid()) {
		if (m_node_resource->get_stream() != m_picker->get_edited_resource()) {
			m_picker->set_edited_resource(m_node_resource->get_stream());
		}
	}
}

String AudioStreamGraphEditorNodeStream::get_node_resource_type() {
	return AudioStreamGraphNodeStream::get_class_static();
}

void AudioStreamGraphEditorNodeStream::set_node_resource(Ref<AudioStreamGraphNode> node_resource) {
	m_node_resource = node_resource;
	if (m_node_resource.is_valid()) {
		m_picker->set_edited_resource(m_node_resource->get_stream());
		m_node_resource->connect("changed", callable_mp(this, &AudioStreamGraphEditorNodeStream::_on_node_resource_changed));
	}
}

Ref<AudioStreamGraphNode> AudioStreamGraphEditorNodeStream::get_node_resource() const {
	return m_node_resource;
}

void AudioStreamGraphEditorNodeStream::_bind_methods() {
}

AudioStreamGraphEditorNodeStream::AudioStreamGraphEditorNodeStream() {
	set_title(TTR("Stream"));
	m_picker = memnew(EditorResourcePicker);
	m_picker->set_base_type("AudioStream");
	m_picker->connect("resource_changed", callable_mp(this, &AudioStreamGraphEditorNodeStream::_on_picker_resource_changed));
	add_child(m_picker);
	set_slot_enabled_right(0, true);
	set_slot_type_right(0, AudioStreamGraphEditor::SLOT_TYPE_AUDIO);
	set_slot_color_right(0, AudioStreamGraphEditor::SLOT_COLOR_AUDIO);
}

////////////////////////////////////////////
// AudioStreamGraphEditorNodeOutput

String AudioStreamGraphEditorNodeOutput::get_node_resource_type() {
	return "AudioStreamGraphNodeOutput";
}

void AudioStreamGraphEditorNodeOutput::set_node_resource(Ref<AudioStreamGraphNode> node_resource) {
	m_node_resource = node_resource;
}

Ref<AudioStreamGraphNode> AudioStreamGraphEditorNodeOutput::get_node_resource() const {
	return m_node_resource;
}

AudioStreamGraphEditorNodeOutput::AudioStreamGraphEditorNodeOutput() {
	set_title(TTR("Audio Output"));
	Label *label = memnew(Label);
	label->set_text(TTR("Output"));
	add_child(label);
	set_slot_enabled_left(0, true);
	set_slot_type_left(0, AudioStreamGraphEditor::SLOT_TYPE_AUDIO);
	set_slot_color_left(0, AudioStreamGraphEditor::SLOT_COLOR_AUDIO);
}

////////////////////////////////////
// AudioStreamGraphEditorNodeMix

String AudioStreamGraphEditorNodeMix::get_node_resource_type() {
	return "AudioStreamGraphNodeMix";
}

void AudioStreamGraphEditorNodeMix::set_node_resource(Ref<AudioStreamGraphNode> node_resource) {
	m_node_resource = node_resource;
}

Ref<AudioStreamGraphNode> AudioStreamGraphEditorNodeMix::get_node_resource() const {
	return m_node_resource;
}

AudioStreamGraphEditorNodeMix::AudioStreamGraphEditorNodeMix() {
	set_title(TTR("Mix"));
	int i = 0;

	Label *mix1_label = memnew(Label);
	mix1_label->set_text("1.0");
	add_child(mix1_label);
	set_slot_enabled_left(i, true);
	set_slot_type_left(i, AudioStreamGraphEditor::SLOT_TYPE_CONTROL);
	set_slot_color_left(i, AudioStreamGraphEditor::SLOT_COLOR_CONTROL);
	i++;

	Label *audio1_label = memnew(Label);
	audio1_label->set_text(TTR("Input 1"));
	add_child(audio1_label);
	set_slot_enabled_left(i, true);
	set_slot_type_left(i, AudioStreamGraphEditor::SLOT_TYPE_AUDIO);
	set_slot_color_left(i, AudioStreamGraphEditor::SLOT_COLOR_AUDIO);
	i++;

	Label *mix2_label = memnew(Label);
	mix2_label->set_text("1.0");
	add_child(mix2_label);
	set_slot_enabled_left(i, true);
	set_slot_type_left(i, AudioStreamGraphEditor::SLOT_TYPE_CONTROL);
	set_slot_color_left(i, AudioStreamGraphEditor::SLOT_COLOR_CONTROL);
	i++;

	Label *audio2_label = memnew(Label);
	audio2_label->set_text(TTR("Input 2"));
	add_child(audio2_label);
	set_slot_enabled_left(i, true);
	set_slot_type_left(i, AudioStreamGraphEditor::SLOT_TYPE_AUDIO);
	set_slot_color_left(i, AudioStreamGraphEditor::SLOT_COLOR_AUDIO);
	i++;

	set_slot_enabled_right(0, true);
	set_slot_type_right(0, AudioStreamGraphEditor::SLOT_TYPE_AUDIO);
	set_slot_color_right(0, AudioStreamGraphEditor::SLOT_COLOR_AUDIO);
}
