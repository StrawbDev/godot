#include "audio_stream_graph_track_item.h"

void AudioStreamGraphTrackItem::set_stream(Ref<AudioStream> stream) {
	m_stream = stream;
	emit_changed();
}

Ref<AudioStream> AudioStreamGraphTrackItem::get_stream() const {
	return m_stream;
}

void AudioStreamGraphTrackItem::set_position(int64_t position) {
	m_position = position;
}

int64_t AudioStreamGraphTrackItem::get_position() const {
	return m_position;
}

void AudioStreamGraphTrackItem::set_length(int64_t length) {
	m_length = length;
}

int64_t AudioStreamGraphTrackItem::get_length() const {
	return m_length;
}

void AudioStreamGraphTrackItem::set_start_trim(int64_t start_trim) {
	m_start_trim = start_trim;
}

int64_t AudioStreamGraphTrackItem::get_start_trim() const {
	return m_start_trim;
}

void AudioStreamGraphTrackItem::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_stream", "stream"), &AudioStreamGraphTrackItem::set_stream);
	ClassDB::bind_method(D_METHOD("get_stream"), &AudioStreamGraphTrackItem::get_stream);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stream", PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"), "set_stream", "get_stream");

	ClassDB::bind_method(D_METHOD("set_position", "position"), &AudioStreamGraphTrackItem::set_position);
	ClassDB::bind_method(D_METHOD("get_position"), &AudioStreamGraphTrackItem::get_position);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "position"), "set_position", "get_position");

	ClassDB::bind_method(D_METHOD("set_length", "length"), &AudioStreamGraphTrackItem::set_length);
	ClassDB::bind_method(D_METHOD("get_length"), &AudioStreamGraphTrackItem::get_length);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "length"), "set_length", "get_length");

	ClassDB::bind_method(D_METHOD("set_start_trim", "start_trim"), &AudioStreamGraphTrackItem::set_start_trim);
	ClassDB::bind_method(D_METHOD("get_start_trim"), &AudioStreamGraphTrackItem::get_start_trim);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "start_trim"), "set_start_trim", "get_start_trim");
}
