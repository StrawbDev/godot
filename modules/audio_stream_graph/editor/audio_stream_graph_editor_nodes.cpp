#include "audio_stream_graph_editor_nodes.h"

void AudioStreamGraphEditorNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_node_resource"), &AudioStreamGraphEditorNode::get_node_resource);
}

////////////////////////////

void AudioStreamGraphEditorNodeStream::_on_picker_resource_changed(Ref<Resource> resource) {
	Ref<AudioStream> stream = resource;
	m_node_resource->set_stream(stream);
}

void AudioStreamGraphEditorNodeStream::_on_node_resource_changed() {
	m_picker->set_edited_resource(m_node_resource->get_stream());
}

Ref<AudioStreamGraphNode> AudioStreamGraphEditorNodeStream::get_node_resource() const {
	return m_node_resource;
}

void AudioStreamGraphEditorNodeStream::_bind_methods() {
}

AudioStreamGraphEditorNodeStream::AudioStreamGraphEditorNodeStream() :
		m_node_resource(memnew(AudioStreamGraphNodeStream)) {
	m_node_resource->connect("changed", callable_mp(this, &AudioStreamGraphEditorNodeStream::_on_node_resource_changed));

	set_title(TTR("Stream"));
	m_picker = memnew(EditorResourcePicker);
	m_picker->set_base_type("AudioStream");
	m_picker->connect("resource_changed", callable_mp(this, &AudioStreamGraphEditorNodeStream::_on_picker_resource_changed));
	add_child(m_picker);
}
