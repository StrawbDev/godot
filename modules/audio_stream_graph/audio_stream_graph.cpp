#include "audio_stream_graph.h"
#include "audio_stream_playback_graph.h"

void AudioStreamGraph::_on_sub_resource_changed() {
	emit_changed();
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
	Callable subresource_change_cb = callable_mp(this, &AudioStreamGraph::_on_sub_resource_changed);
	if (!node->is_connected("changed", subresource_change_cb)) {
		node->connect("changed", subresource_change_cb);
	}

	m_max_id++;
	m_nodes[m_max_id] = node;
	emit_changed();
	return m_max_id;
}

void AudioStreamGraph::add_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx) {
	if (!m_connections[from_node_idx].is_array()) {
		m_connections[from_node_idx] = Array();
	}

	Array node_connections = m_connections[from_node_idx];
	Array connection_tuple;
	connection_tuple.push_back(from_port_idx);
	connection_tuple.push_back(to_node_idx);
	connection_tuple.push_back(to_port_idx);
	node_connections.append(connection_tuple);
	emit_changed();
}

void AudioStreamGraph::remove_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx) {
	if (!m_connections[from_node_idx].is_array()) {
		return;
	}

	Array node_connections = m_connections[from_node_idx];
	Array connection_tuple;
	connection_tuple.push_back(from_port_idx);
	connection_tuple.push_back(to_node_idx);
	connection_tuple.push_back(to_port_idx);
	int connection_idx = node_connections.find(connection_tuple);
	if (connection_idx != -1) {
		node_connections.remove_at(connection_idx);
		emit_changed();
	}
}

bool AudioStreamGraph::is_connection(int from_node_idx, int from_port_idx, int to_node_idx, int to_port_idx) const {
	if (!m_connections[from_node_idx].is_array()) {
		return false;
	}

	Array node_connections = m_connections[from_node_idx];
	Array connection_tuple;
	connection_tuple.push_back(from_port_idx);
	connection_tuple.push_back(to_node_idx);
	connection_tuple.push_back(to_port_idx);
	return node_connections.find(connection_tuple) != -1;
}

Dictionary AudioStreamGraph::get_connections() const {
	return m_connections;
}

void AudioStreamGraph::set_connections(Dictionary connections) {
	m_connections = connections;
	emit_changed();
}

Ref<AudioStreamGraphNode> AudioStreamGraph::get_node(int node_id) const {
	return m_nodes[node_id];
}

void AudioStreamGraph::set_node(int node_id, Ref<AudioStreamGraphNode> node) {
	m_nodes[node_id] = node;
	emit_changed();
}

int AudioStreamGraph::next_node_id() const {
	return m_max_id + 1;
}

int AudioStreamGraph::num_nodes() const {
	return m_nodes.size();
}

void AudioStreamGraph::remove_node(int node_id) {
	m_nodes.erase(node_id);
	if (node_id == m_max_id) {
		m_max_id--;
	}
	emit_changed();
}

void AudioStreamGraph::set_nodes(Dictionary nodes) {
	m_nodes = nodes;
	emit_changed();
}

Dictionary AudioStreamGraph::get_nodes() const {
	return m_nodes;
}

PackedInt32Array AudioStreamGraph::get_connections_for_node(int node_id) const {
	PackedInt32Array result;
	for (int i = 0; i < m_connections.size(); i++) {
		int from_node = m_connections.get_key_at_index(i);

		Array connection_tuple = m_connections.get_value_at_index(i);
		if (connection_tuple.size() != 3) {
			continue;
		}

		int from_port = connection_tuple[0];
		int to_node = connection_tuple[1];
		int to_port = connection_tuple[2];

		result.push_back(from_node);
		result.push_back(from_port);
		result.push_back(to_node);
		result.push_back(to_port);
	}

	return result;
}

void AudioStreamGraph::remove_connections_for_node(int node_id) {
	PackedInt32Array to_remove = get_connections_for_node(node_id);
	for (int i = 0; i < to_remove.size(); i += 4) {
		remove_connection(to_remove[i], to_remove[i + 1], to_remove[i + 2], to_remove[i + 3]);
	}
}

void AudioStreamGraph::_get_property_list(List<PropertyInfo> *r_props) const {
	for (int i = 0; i < m_nodes.size(); i++) {
		int node_id = m_nodes.get_key_at_index(i);
		StringName name = vformat("nodes/%d", node_id);
		r_props->push_back(PropertyInfo(Variant::OBJECT, name, PROPERTY_HINT_RESOURCE_TYPE, "AudioStreamGraphNode", PROPERTY_USAGE_NO_EDITOR));
	}
}

bool AudioStreamGraph::_get(const StringName &p_property, Variant &r_value) const {
	String name = p_property;
	if (name.begins_with("nodes/")) {
		int id = name.get_slicec('/', 1).to_int();
		r_value = get_node(id);
		return true;
	}

	return false;
}

bool AudioStreamGraph::_set(const StringName &p_property, const Variant &p_value) {
	String name = p_property;
	if (name.begins_with("nodes/")) {
		int id = name.get_slicec('/', 1).to_int();
		set_node(id, p_value);
		return true;
	}

	return false;
}

void AudioStreamGraph::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_connections"), &AudioStreamGraph::set_connections);
	ClassDB::bind_method(D_METHOD("get_connections"), &AudioStreamGraph::get_connections);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "connections", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_connections", "get_connections");
	ClassDB::bind_method(D_METHOD("add_connection", "from_node_idx", "from_port_idx", "to_node_idx", "to_port_idx"), &AudioStreamGraph::add_connection);
	ClassDB::bind_method(D_METHOD("remove_connection", "from_node_idx", "from_port_idx", "to_node_idx", "to_port_idx"), &AudioStreamGraph::remove_connection);
	ClassDB::bind_method(D_METHOD("is_connection", "from_node_idx", "from_port_idx", "to_node_idx", "to_port_idx"), &AudioStreamGraph::is_connection);

	ClassDB::bind_method(D_METHOD("num_nodes"), &AudioStreamGraph::num_nodes);
	ClassDB::bind_method(D_METHOD("add_node", "node"), &AudioStreamGraph::add_node);
	ClassDB::bind_method(D_METHOD("remove_node", "node_idx"), &AudioStreamGraph::remove_node);
	ClassDB::bind_method(D_METHOD("set_node", "node_idx", "node"), &AudioStreamGraph::set_node);
	ClassDB::bind_method(D_METHOD("get_node", "node_idx"), &AudioStreamGraph::get_node);
	ClassDB::bind_method(D_METHOD("set_nodes", "nodes"), &AudioStreamGraph::set_nodes);
	ClassDB::bind_method(D_METHOD("get_nodes"), &AudioStreamGraph::get_nodes);
}
