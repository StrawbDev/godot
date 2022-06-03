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

	virtual int get_num_input_ports() const { return 0; }
	virtual PackedInt32Array get_optional_input_ports() const { return PackedInt32Array(); }
	virtual float get_input_port_default_value(int port) const { return 0; }
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

public:
	virtual int get_num_input_ports() const override { return 1; }
};

class AudioStreamGraphNodeMix : public AudioStreamGraphNode {
	GDCLASS(AudioStreamGraphNodeMix, AudioStreamGraphNode);

private:
	float m_mix1 = 1;
	float m_mix2 = 1;

protected:
	static void _bind_methods();

public:
	virtual int get_num_input_ports() const override;
	virtual PackedInt32Array get_optional_input_ports() const override;
	virtual float get_input_port_default_value(int port) const override;

	void set_mix1(float amount);
	float get_mix1() const;

	void set_mix2(float amount);
	float get_mix2() const;
};

#endif
