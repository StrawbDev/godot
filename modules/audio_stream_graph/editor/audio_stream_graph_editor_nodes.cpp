#include "audio_stream_graph_editor_nodes.h"

void AudioStreamGraphEditorNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_node_resource"), &AudioStreamGraphEditorNode::get_node_resource);
}

////////////////////////////

void AudioStreamGraphEditorNodeStream::_on_picker_resource_changed(Ref<Resource> resource) {
	print_line(vformat("_on_picker_resource_changed(%s)", resource));
	if (m_node_resource.is_valid()) {
		if (resource != m_node_resource->get_stream()) {
			m_node_resource->set_stream(resource);
		}
	}
}

void AudioStreamGraphEditorNodeStream::_on_node_resource_changed() {
	print_line("_on_node_resource_changed()");
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
}
