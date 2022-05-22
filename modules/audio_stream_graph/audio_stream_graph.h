#ifndef AUDIO_STREAM_GRAPH_H
#define AUDIO_STREAM_GRAPH_H

#include "audio_stream_graph_nodes.h"
#include "servers/audio/audio_stream.h"

class AudioStreamGraph : public AudioStream {
	GDCLASS(AudioStreamGraph, AudioStream);

private:
	Dictionary m_nodes;
	Dictionary m_connections;
	int m_max_id = -1;

	void _on_sub_resource_changed();

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

	void set_connections(Dictionary connections);
	Dictionary get_connections() const;
	void add_connection(int from_node_id, int from_port_id, int to_node_id, int to_port_id);
	void remove_connection(int from_node_id, int from_port_id, int to_node_id, int to_port_id);
	bool is_connection(int from_node_id, int from_port_id, int to_node_id, int to_port_id) const;

	int next_node_id() const;
	int num_nodes() const;
	int add_node(Ref<AudioStreamGraphNode> node);
	void remove_node(int node_id);
	void set_node(int node_id, Ref<AudioStreamGraphNode> node);
	Ref<AudioStreamGraphNode> get_node(int node_id) const;
	void set_nodes(Dictionary nodes);
	Dictionary get_nodes() const;

	PackedInt32Array get_connections_for_node(int node_id) const;
	void remove_connections_for_node(int node_id);
};

#endif // AUDIO_STREAM_GRAPH_H
