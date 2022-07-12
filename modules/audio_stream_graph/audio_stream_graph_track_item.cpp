#include "audio_stream_graph_track_item.h"

void AudioStreamGraphTrackItem::set_stream(Ref<AudioStream> stream) {
	m_stream = stream;
	emit_changed();
}

Ref<AudioStream> AudioStreamGraphTrackItem::get_stream() const {
	return m_stream;
}

void AudioStreamGraphTrackItem::set_start_position(uint64_t position) {
	m_start_position = position;
	emit_changed();
}

uint64_t AudioStreamGraphTrackItem::get_start_position() const {
	return m_start_position;
}

void AudioStreamGraphTrackItem::set_end_position(uint64_t position) {
	m_end_position = position;
	emit_changed();
}

uint64_t AudioStreamGraphTrackItem::get_end_position() const {
	return m_end_position;
}

void AudioStreamGraphTrackItem::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_stream", "stream"), &AudioStreamGraphTrackItem::set_stream);
	ClassDB::bind_method(D_METHOD("get_stream"), &AudioStreamGraphTrackItem::get_stream);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stream", PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"), "set_stream", "get_stream");

	ClassDB::bind_method(D_METHOD("set_start_position", "position"), &AudioStreamGraphTrackItem::set_start_position);
	ClassDB::bind_method(D_METHOD("get_start_position"), &AudioStreamGraphTrackItem::get_start_position);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "start_position"), "set_start_position", "get_start_position");

	ClassDB::bind_method(D_METHOD("set_end_position", "position"), &AudioStreamGraphTrackItem::set_end_position);
	ClassDB::bind_method(D_METHOD("get_end_position", "position"), &AudioStreamGraphTrackItem::get_end_position);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "end_position"), "set_end_position", "get_end_position");
}
