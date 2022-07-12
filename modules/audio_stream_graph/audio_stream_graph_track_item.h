#ifndef AUDIO_STREAM_GRAPH_TRACK_ITEM_H
#define AUDIO_STREAM_GRAPH_TRACK_ITEM_H

#include "servers/audio/audio_stream.h"

class AudioStreamGraphTrackItem : public Resource {
	GDCLASS(AudioStreamGraphTrackItem, Resource);

private:
	Ref<AudioStream> m_stream;
	uint64_t m_start_position;
	uint64_t m_end_position;

protected:
	static void _bind_methods();

public:
	void set_stream(Ref<AudioStream> stream);
	Ref<AudioStream> get_stream() const;

	void set_start_position(uint64_t position);
	uint64_t get_start_position() const;

	void set_end_position(uint64_t position);
	uint64_t get_end_position() const;
};

#endif // AUDIO_STREAM_GRAPH_TRACK_ITEM_H
