#include "servers/audio/audio_stream.h"

#ifndef AUDIO_STREAM_GRAPH_H
#define AUDIO_STREAM_GRAPH_H

class AudioStreamGraph : public AudioStream {
	GDCLASS(AudioStreamGraph, AudioStream);
	RES_BASE_EXTENSION("genaudio")

private:
	Ref<AudioStream> m_test_stream;

protected:
	static void _bind_methods();

public:
	virtual Ref<AudioStreamPlayback> instance_playback() override;
	virtual String get_stream_name() const override;

	virtual float get_length() const override;
	virtual bool is_monophonic() const override;

	void set_test_stream(Ref<AudioStream> test_stream);
	Ref<AudioStream> get_test_stream();
};

#endif // AUDIO_STREAM_GRAPH_H
