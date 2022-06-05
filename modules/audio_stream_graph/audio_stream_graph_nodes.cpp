#include "audio_stream_graph_nodes.h"

// AudioStreamGraphNode

Vector2 AudioStreamGraphNode::get_position() const {
	return m_position;
}

void AudioStreamGraphNode::set_position(Vector2 position) {
	m_position = position;
}

void AudioStreamGraphNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_position", "position"), &AudioStreamGraphNode::set_position);
	ClassDB::bind_method(D_METHOD("get_position"), &AudioStreamGraphNode::get_position);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "position"), "set_position", "get_position");
}

///////////////////////////
// AudioStreamGraphNodeStream

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
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stream", PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"), "set_stream", "get_stream");
}

//////////////////////////
// AudioStreamGraphNodeMix

int AudioStreamGraphNodeMix::get_num_input_ports() const {
	return 4;
}

PackedInt32Array AudioStreamGraphNodeMix::get_optional_input_ports() const {
	PackedInt32Array result;
	result.push_back(0);
	result.push_back(2);
	return result;
}

float AudioStreamGraphNodeMix::get_input_port_default_value(int port) const {
	if (port == 0) {
		return get_mix1();
	} else if (port == 2) {
		return get_mix2();
	} else {
		ERR_FAIL_V_MSG(0, TTR("Port does not have a default value"));
	}
}

void AudioStreamGraphNodeMix::set_mix1(float amount) {
	m_mix1 = amount;
}

float AudioStreamGraphNodeMix::get_mix1() const {
	return m_mix1;
}

void AudioStreamGraphNodeMix::set_mix2(float amount) {
	m_mix2 = amount;
}

float AudioStreamGraphNodeMix::get_mix2() const {
	return m_mix2;
}

void AudioStreamGraphNodeMix::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mix1", "amount"), &AudioStreamGraphNodeMix::set_mix1);
	ClassDB::bind_method(D_METHOD("get_mix1"), &AudioStreamGraphNodeMix::get_mix1);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mix1"), "set_mix1", "get_mix1");
	ClassDB::bind_method(D_METHOD("set_mix2", "amount"), &AudioStreamGraphNodeMix::set_mix2);
	ClassDB::bind_method(D_METHOD("get_mix2"), &AudioStreamGraphNodeMix::get_mix2);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mix2"), "set_mix2", "get_mix2");
}

///////////////////////
// AudioStreamGraphNodeParameter

void AudioStreamGraphNodeParameter::set_parameter_name(StringName name) {
	m_parameter_name = name;
	emit_changed();
}

StringName AudioStreamGraphNodeParameter::get_parameter_name() const {
	return m_parameter_name;
}

void AudioStreamGraphNodeParameter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_parameter_name", "name"), &AudioStreamGraphNodeParameter::set_parameter_name);
	ClassDB::bind_method(D_METHOD("get_parameter_name"), &AudioStreamGraphNodeParameter::get_parameter_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "parameter_name"), "set_parameter_name", "get_parameter_name");
}
