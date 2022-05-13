#include "servers/audio/audio_stream.h"

#ifndef AUDIO_STREAM_GRAPH_H
#define AUDIO_STREAM_GRAPH_H

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

class AudioStreamGraph : public AudioStream {
	GDCLASS(AudioStreamGraph, AudioStream);

private:
	Vector<Ref<AudioStreamGraphNode>> m_nodes;
	PackedInt32Array m_connections;

	int _find_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx);

protected:
	static void _bind_methods();

public:
	virtual Ref<AudioStreamPlayback> instance_playback() override;
	virtual String get_stream_name() const override;
	virtual float get_length() const override;
	virtual bool is_monophonic() const override;

	PackedInt32Array get_connections();
	void set_connections(PackedInt32Array connections);
	void add_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx);
	void remove_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx);

	int num_nodes();
	int add_node(Ref<AudioStreamGraphNode> node);
	void remove_node(int node_idx);
	Ref<AudioStreamGraphNode> get_node(int node_idx);
	void set_node(int node_idx, Ref<AudioStreamGraphNode> node);
};

#endif // AUDIO_STREAM_GRAPH_H
