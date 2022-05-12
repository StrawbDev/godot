#ifndef AUDIO_STREAM_PLAYBACK_GRAPH_H
#define AUDIO_STREAM_PLAYBACK_GRAPH_H

#include "audio_stream_graph.h"
#include "servers/audio/audio_stream.h"

class AudioStreamPlaybackGraph : public AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackGraph, AudioStreamPlayback);

private:
	Ref<AudioStreamGraph> m_resource;
	Ref<AudioStreamPlayback> m_test_playback;

public:
	virtual void start(float p_from_pos = 0.0) override;
	virtual void stop() override;
	virtual bool is_playing() const override;

	virtual int get_loop_count() const override;

	virtual float get_playback_position() const override;
	virtual void seek(float p_time) override;

	virtual int mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) override;

	void set_resource(Ref<AudioStreamGraph> resource);
};

#endif // AUDIO_STREAM_PLAYBACK_GRAPH_H