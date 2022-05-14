#include "audio_stream_graph.h"
#include "audio_stream_playback_graph.h"

int AudioStreamGraph::_find_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx) const {
	ERR_FAIL_COND_V(m_connections.size() % 4 != 0, -1);
	for (int i = 0; i < m_connections.size(); i += 4) {
		if (m_connections[i] == from_node_idx && m_connections[i + 1] == from_port_idx && m_connections[i + 2] == to_node_idx && m_connections[i + 3] == to_port_idx) {
			return i;
		}
	}
	return -1;
}

Ref<AudioStreamPlayback> AudioStreamGraph::instance_playback() {
	Ref<AudioStreamPlaybackGraph> playback;
	playback.instantiate();
	playback->set_resource(this);
	return playback;
}

String AudioStreamGraph::get_stream_name() const {
	return "AudioStreamGraph";
}

float AudioStreamGraph::get_length() const {
	return 0;
}

bool AudioStreamGraph::is_monophonic() const {
	return false;
}

int AudioStreamGraph::add_node(Ref<AudioStreamGraphNode> node) {
	m_nodes.push_back(node);
	return m_nodes.size();
}

void AudioStreamGraph::add_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx) {
	ERR_FAIL_INDEX(from_node_idx, m_nodes.size());
	ERR_FAIL_INDEX(to_node_idx, m_nodes.size());
	ERR_FAIL_COND(_find_connection(from_node_idx, from_port_idx, to_node_idx, to_port_idx) != -1);
	m_connections.push_back(from_node_idx);
	m_connections.push_back(from_port_idx);
	m_connections.push_back(to_node_idx);
	m_connections.push_back(to_port_idx);
}

void AudioStreamGraph::remove_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx) {
	int i = _find_connection(from_node_idx, from_port_idx, to_node_idx, to_port_idx);
	ERR_FAIL_COND(i == -1);
	m_connections.remove_at(i + 3);
	m_connections.remove_at(i + 2);
	m_connections.remove_at(i + 1);
	m_connections.remove_at(i);
}

PackedInt32Array AudioStreamGraph::get_connections() const {
	return m_connections;
}

void AudioStreamGraph::set_connections(PackedInt32Array connections) {
	ERR_FAIL_COND(connections.size() % 4 != 0);
	m_connections = connections;
}

Ref<AudioStreamGraphNode> AudioStreamGraph::get_node(int node_idx) const {
	ERR_FAIL_INDEX_V(node_idx, m_nodes.size(), nullptr);
	return m_nodes[node_idx];
}

void AudioStreamGraph::set_node(int node_idx, Ref<AudioStreamGraphNode> node) {
	ERR_FAIL_INDEX(node_idx, m_nodes.size());
	m_nodes.write[node_idx] = node;
}

int AudioStreamGraph::num_nodes() const {
	return m_nodes.size();
}

void AudioStreamGraph::remove_node(int node_idx) {
	ERR_FAIL_INDEX(node_idx, m_nodes.size());
	ERR_FAIL_COND(m_connections.size() % 4 != 0);

	for (int i = m_connections.size() - 1; i >= 0; i -= 4) {
		if (i - 1 == node_idx || i - 3 == node_idx) {
			m_connections.remove_at(i);
			m_connections.remove_at(i - 1);
			m_connections.remove_at(i - 2);
			m_connections.remove_at(i - 3);
		}
	}

	m_nodes.remove_at(node_idx);
}

void AudioStreamGraph::_bind_methods() {
}
