#ifndef AUDIO_STREAM_GRAPH_TRACK_ITEM_H
#define AUDIO_STREAM_GRAPH_TRACK_ITEM_H

#include "servers/audio/audio_stream.h"

class AudioStreamGraphTrackItem : public Resource {
	GDCLASS(AudioStreamGraphTrackItem, Resource);

private:
	Ref<AudioStream> m_stream = nullptr;
	int64_t m_position = 0;
	int64_t m_length = 0;
	int64_t m_start_trim = 0;

protected:
	static void _bind_methods();

public:
	void set_stream(Ref<AudioStream> stream);
	Ref<AudioStream> get_stream() const;

	void set_position(int64_t position);
	int64_t get_position() const;

	void set_length(int64_t length);
	int64_t get_length() const;

	void set_start_trim(int64_t start_trim);
	int64_t get_start_trim() const;
};

#endif // AUDIO_STREAM_GRAPH_TRACK_ITEM_H
