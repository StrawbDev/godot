#include "audio_stream_graph_nodes.h"

Vector2 AudioStreamGraphNode::get_position() const {
	return m_position;
}

void AudioStreamGraphNode::set_position(Vector2 position) {
	m_position = position;
	emit_changed();
}

void AudioStreamGraphNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_position", "position"), &AudioStreamGraphNode::set_position);
	ClassDB::bind_method(D_METHOD("get_position"), &AudioStreamGraphNode::get_position);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "position"), "set_position", "get_position");
}

///////////////////////////

Ref<AudioStream> AudioStreamGraphNodeStream::get_stream() const {
	return m_stream;
}

void AudioStreamGraphNodeStream::set_stream(Ref<AudioStream> stream) {
	m_stream = stream;
	emit_changed();
}

void AudioStreamGraphNodeStream::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_stream", "stream"), &AudioStreamGraphNodeStream::set_stream);
	ClassDB::bind_method(D_METHOD("get_stream"), &AudioStreamGraphNodeStream::get_stream);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stream", PropertyHint::PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"), "set_stream", "get_stream");
}
