#ifndef AUDIO_STREAM_GRAPH_H
#define AUDIO_STREAM_GRAPH_H

#include "audio_stream_graph_nodes.h"
#include "servers/audio/audio_stream.h"

class AudioStreamGraph : public AudioStream {
	GDCLASS(AudioStreamGraph, AudioStream);

private:
	Vector<Ref<AudioStreamGraphNode>> m_nodes;
	PackedInt32Array m_connections;

	int _find_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx) const;

protected:
	static void _bind_methods();
	void _get_property_list(List<PropertyInfo> *r_props) const;
	bool _get(const StringName &p_property, Variant &r_value) const;
	bool _set(const StringName &p_property, const Variant &p_value);

public:
	virtual Ref<AudioStreamPlayback> instance_playback() override;
	virtual String get_stream_name() const override;
	virtual float get_length() const override;
	virtual bool is_monophonic() const override;

	void set_connections(PackedInt32Array connections);
	PackedInt32Array get_connections() const;
	void add_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx);
	void remove_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx);
	bool is_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx) const;

	int num_nodes() const;
	int add_node(Ref<AudioStreamGraphNode> node);
	void remove_node(int node_idx);
	void set_node(int node_idx, Ref<AudioStreamGraphNode> node);
	Ref<AudioStreamGraphNode> get_node(int node_idx) const;
};

#endif // AUDIO_STREAM_GRAPH_H
