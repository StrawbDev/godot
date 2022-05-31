#include "audio_stream_graph.h"
#include "audio_stream_playback_graph.h"

void AudioStreamGraph::_on_sub_resource_changed() {
	emit_changed();
}

int AudioStreamGraph::_find_output_node() const {
	for (const KeyValue<int, Ref<AudioStreamGraphNode>> &entry : m_nodes) {
		if (entry.value.is_valid() && entry.value->is_class("AudioStreamGraphNodeOutput")) {
			return entry.key;
		}
	}
	return -1;
}

HashMap<int, Vector<AudioStreamGraph::ConnectionTuple>> AudioStreamGraph::_get_inverted_connections_sorted() const {
	HashMap<int, Vector<ConnectionTuple>> result;
	for (const KeyValue<int, Vector<ConnectionTuple>> &entry : m_connections) {
		for (const ConnectionTuple &tuple : entry.value) {
			ConnectionTuple connection{ tuple.to_port, entry.key, tuple.from_port };
			result[tuple.to_node].push_back(connection);
		}
	}

	for (KeyValue<int, Vector<ConnectionTuple>> &entry : result) {
		Vector<ConnectionTuple> &connection_list = entry.value;
		struct Sorter {
			bool operator()(const ConnectionTuple &a, const ConnectionTuple &b) const {
				return a >= b;
			}
		};
		connection_list.sort_custom<Sorter>();
	}

	return result;
}

void AudioStreamGraph::_do_compile_traversal() const {
	int start = _find_output_node();
	HashMap<int, Vector<ConnectionTuple>> compile_graph = _get_inverted_connections_sorted();
	Set<int> visited;
	Vector<int> to_visit;
	to_visit.push_back(start);

	while (!to_visit.is_empty()) {
		int current = to_visit.get(to_visit.size() - 1);
		to_visit.remove_at(to_visit.size() - 1);

		// emit_bytecode(current);
		visited.insert(current);
		print_line(vformat("AudioStreamGraph visited %d", current));

		Vector<ConnectionTuple> connections = compile_graph[current];
		for (const ConnectionTuple &connection : connections) {
			int next = connection.to_node;
			if (!visited.has(next)) {
				to_visit.push_back(next);
			}
		}
	}
}

Ref<AudioStreamPlayback> AudioStreamGraph::instance_playback() {
	compile();
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

void AudioStreamGraph::add_connection(int from_node_id, int from_port_idx, int to_node_id, int to_port_idx) {
	ConnectionTuple connection{ from_port_idx, to_node_id, to_port_idx };
	m_connections[from_node_id].push_back(connection);
	emit_changed();
}

void AudioStreamGraph::remove_connection(int from_node_id, int from_port_idx, int to_node_id, int to_port_idx) {
	Vector<ConnectionTuple> &node_connections = m_connections[from_node_id];
	ConnectionTuple connection{ from_port_idx, to_node_id, to_port_idx };
	int connection_idx = node_connections.find(connection);
	if (connection_idx != -1) {
		node_connections.remove_at(connection_idx);
		emit_changed();
	}
}

bool AudioStreamGraph::is_connection(int from_node_id, int from_port_idx, int to_node_id, int to_port_idx) const {
	ConnectionTuple connection{ from_port_idx, to_node_id, to_port_idx };
	return m_connections[from_node_id].find(connection) != -1;
}

Dictionary AudioStreamGraph::get_connections() const {
	Dictionary result;
	for (const KeyValue<int, Vector<ConnectionTuple>> &entry : m_connections) {
		Array connection_list;
		for (const ConnectionTuple &connection : entry.value) {
			Array result_tuple;
			result_tuple.push_back(connection.from_port);
			result_tuple.push_back(connection.to_node);
			result_tuple.push_back(connection.to_port);
			connection_list.push_back(result_tuple);
		}
		result[entry.key] = connection_list;
	}
	return result;
}

void AudioStreamGraph::set_connections(Dictionary connections) {
	m_connections.clear();
	m_max_id = -1;

	for (int i = 0; i < connections.size(); i++) {
		int node_id = connections.get_key_at_index(i);
		Vector<ConnectionTuple> &connection_tuples = m_connections[node_id];
		if (node_id > m_max_id) {
			m_max_id = node_id;
		}

		Array node_connections = connections.get_value_at_index(i);
		for (int j = 0; j < node_connections.size(); j++) {
			Array tuple_array = node_connections[j];
			if (tuple_array.size() == 3) {
				ConnectionTuple tuple{ tuple_array[0], tuple_array[1], tuple_array[2] };
				connection_tuples.push_back(tuple);
			}
		}
	}

	emit_changed();
}

Ref<AudioStreamGraphNode> AudioStreamGraph::get_node(int node_id) const {
	return m_nodes[node_id];
}

void AudioStreamGraph::set_node(int node_id, Ref<AudioStreamGraphNode> node) {
	if (node_id > m_max_id) {
		m_max_id = node_id;
	}
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

Dictionary AudioStreamGraph::get_nodes() const {
	Dictionary result;
	for (const KeyValue<int, Ref<AudioStreamGraphNode>> &entry : m_nodes) {
		result[entry.key] = entry.value;
	}
	return result;
}

PackedInt32Array AudioStreamGraph::get_connections_for_node(int node_id) const {
	PackedInt32Array result;

	for (const ConnectionTuple &connection : m_connections.get(node_id)) {
		result.push_back(node_id);
		result.push_back(connection.from_port);
		result.push_back(connection.to_node);
		result.push_back(connection.to_port);
	}

	return result;
}

void AudioStreamGraph::remove_connections_for_node(int node_id) {
	PackedInt32Array to_remove = get_connections_for_node(node_id);
	for (int i = 0; i < to_remove.size(); i += 4) {
		remove_connection(to_remove[i], to_remove[i + 1], to_remove[i + 2], to_remove[i + 3]);
	}
}

AudioStreamGraph::CompileResult AudioStreamGraph::compile() {
	CompileResult result{};
	_do_compile_traversal();
	return result;
}

void AudioStreamGraph::_get_property_list(List<PropertyInfo> *r_props) const {
	for (const KeyValue<int, Ref<AudioStreamGraphNode>> &entry : m_nodes) {
		StringName name = vformat("nodes/%d", entry.key);
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
}
