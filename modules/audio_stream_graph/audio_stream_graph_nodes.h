#ifndef AUDIO_STREAM_GRAPH_NODES_H
#define AUDIO_STREAM_GRAPH_NODES_H

#include "servers/audio/audio_stream.h"

class AudioStreamGraphNode : public Resource {
	GDCLASS(AudioStreamGraphNode, Resource);

private:
	Vector2 m_position;

protected:
	static void _bind_methods();

public:
	Vector2 get_position() const;
	void set_position(Vector2 position);
};

class AudioStreamGraphNodeStream : public AudioStreamGraphNode {
	GDCLASS(AudioStreamGraphNodeStream, AudioStreamGraphNode);

private:
	Ref<AudioStream> m_stream;

protected:
	static void _bind_methods();

public:
	Ref<AudioStream> get_stream() const;
	void set_stream(Ref<AudioStream> stream);
};

class AudioStreamGraphNodeOutput : public AudioStreamGraphNode {
	GDCLASS(AudioStreamGraphNodeOutput, AudioStreamGraphNode);
};

class AudioStreamGraphNodeMix : public AudioStreamGraphNode {
	GDCLASS(AudioStreamGraphNodeMix, AudioStreamGraphNode);

private:
	float m_mix1 = 1;
	float m_mix2 = 1;

protected:
	static void _bind_methods();

public:
	void set_mix1(float amount);
	float get_mix1() const;

	void set_mix2(float amount);
	float get_mix2() const;
};

#endif
