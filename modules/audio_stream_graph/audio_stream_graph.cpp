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

void AudioStreamGraph::_do_compile_traversal(int current, const HashMap<int, Vector<ConnectionTuple>> &compile_graph, CompileResult &bytecode_out) {
	Vector<ConnectionTuple> connections = compile_graph.has(current) ? compile_graph[current] : Vector<ConnectionTuple>();
	Ref<AudioStreamGraphNode> node = get_node(current);
	ERR_FAIL_COND_MSG(node.is_null(), "Encountered a nonexistent node during compilation");
	int num_inputs = node->get_num_input_ports();
	PackedInt32Array optional_inputs = node->get_optional_input_ports();
	for (int port = 0; port < num_inputs; port++) {
		ConnectionTuple current_connection;
		bool has_current_connection = false;
		for (const ConnectionTuple &connection : connections) {
			if (connection.from_port == port) {
				current_connection = connection;
				has_current_connection = true;
				break;
			}
		}

		if (!has_current_connection && optional_inputs.has(port)) {
			float default_value = node->get_input_port_default_value(port);
			Bytecode default_op;
			default_op.op = Opcode::PUSH_CONST;
			default_op.operand = default_value;
			bytecode_out.bytecode.push_back(default_op);
		} else if (!has_current_connection) {
			ERR_FAIL_MSG("A required input connection is missing");
		} else {
			int next = current_connection.to_node;
			_do_compile_traversal(next, compile_graph, bytecode_out);
		}
	}
	print_line("AudioStreamGraph visited ", current);
	_add_bytecode(current, bytecode_out);
}

void AudioStreamGraph::_add_bytecode(int node_id, CompileResult &bytecode_out) {
	Ref<AudioStreamGraphNode> node = get_node(node_id);
	if (node.is_null()) {
		bytecode_out.ok = false;
		ERR_FAIL_MSG("Encountered nonexistent node while compiling");
	}

	StringName node_class = node->get_class_name();
	if (node_class == "AudioStreamGraphNodeOutput") {
		Bytecode copy_op;
		copy_op.op = Opcode::COPY_TO_OUTPUT;
		copy_op.operand = 0;
		bytecode_out.bytecode.push_back(copy_op);
	} else if (node_class == "AudioStreamGraphNodeStream") {
		Ref<AudioStreamGraphNodeStream> stream_node = node;
		Ref<AudioStream> stream = stream_node->get_stream();
		if (stream.is_null()) {
			bytecode_out.ok = false;
			ERR_FAIL_MSG("Encountered null AudioStream during compilation");
		}

		int stream_id = bytecode_out.audio_inputs.size();
		Bytecode push_op;
		push_op.op = Opcode::PUSH_AUDIO;
		push_op.operand = stream_id;
		bytecode_out.audio_inputs.push_back(stream);
		bytecode_out.bytecode.push_back(push_op);
	} else if (node_class == "AudioStreamGraphNodeMix") {
		Bytecode mix_op;
		mix_op.op = Opcode::MIX;
		mix_op.operand = 0;
		bytecode_out.bytecode.push_back(mix_op);
	} else if (node_class == "AudioStreamGraphNodeParameter") {
		Ref<AudioStreamGraphNodeParameter> param_node = node;
		StringName param_name = param_node->get_parameter_name();
		if (!m_parameters.has(param_name)) {
			bytecode_out.ok = false;
			ERR_FAIL_MSG("Encountered undefined parameter during compilation");
		}
		Bytecode push_op;
		push_op.op = Opcode::PUSH_PARAM;
		push_op.operand = param_name;
		bytecode_out.bytecode.push_back(push_op);
	} else {
		bytecode_out.ok = false;
		ERR_FAIL_MSG("Encountered unknown node type while compiling");
	}
}

Ref<AudioStreamPlayback> AudioStreamGraph::instance_playback() {
	print_line("DEBUG: AudioStreamPlaybackGraph instanced");
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
	result.parameters = m_parameters;
	int start = _find_output_node();
	HashMap<int, Vector<ConnectionTuple>> compile_graph = _get_inverted_connections_sorted();
	print_line("\nAudioStreamGraph starting traversal");
	_do_compile_traversal(start, compile_graph, result);
	if (result.ok) {
		print_line("AUDIO INPUT DUMP:");
		for (const Ref<AudioStream> &input : result.audio_inputs) {
			print_line(input);
		}

		print_line("BYTECODE DUMP:");
		for (const Bytecode &code : result.bytecode) {
			switch (code.op) {
				case Opcode::PUSH_AUDIO:
					print_line(vformat("PUSH_AUDIO %d", code.operand));
					break;
				case Opcode::PUSH_PARAM:
					print_line(vformat("PUSH_PARAM %s", code.operand));
					break;
				case Opcode::PUSH_CONST:
					print_line(vformat("PUSH_CONST %f", code.operand));
					break;
				case Opcode::DUP:
					print_line("DUP");
					break;
				case Opcode::MIX:
					print_line("MIX");
					break;
				case Opcode::COPY_TO_OUTPUT:
					print_line("COPY_TO_OUTPUT");
					break;
			}
		}
	}
	return result;
}

void AudioStreamGraph::add_parameter(StringName name, float default_value) {
	m_parameters[name] = default_value;
}

void AudioStreamGraph::remove_parameter(StringName name) {
	m_parameters.erase(name);
}

PackedStringArray AudioStreamGraph::get_parameter_names() {
	PackedStringArray result;
	for (const KeyValue<StringName, float> &parameter : m_parameters) {
		result.push_back(parameter.key);
	}
	return result;
}

void AudioStreamGraph::add_track(StringName name) {
	if (!m_tracks.has(name)) {
		m_tracks.insert(name, Track());
	}
}

void AudioStreamGraph::add_item_to_track(StringName track_name, Ref<AudioStreamGraphTrackItem> item) {
	ERR_FAIL_COND(!m_tracks.has(track_name));
	m_tracks[track_name].items.append(item);
}

void AudioStreamGraph::remove_item_from_track(Ref<AudioStreamGraphTrackItem> item) {
	for (KeyValue<StringName, Track> &track : m_tracks) {
		int idx = track.value.items.find(item);
		if (idx != -1) {
			track.value.items.remove_at(idx);
			break;
		}
	}
}

void AudioStreamGraph::_get_property_list(List<PropertyInfo> *r_props) const {
	for (const KeyValue<int, Ref<AudioStreamGraphNode>> &entry : m_nodes) {
		StringName name = vformat("nodes/%d", entry.key);
		r_props->push_back(PropertyInfo(Variant::OBJECT, name, PROPERTY_HINT_RESOURCE_TYPE, "AudioStreamGraphNode"));
	}
	for (const KeyValue<StringName, float> &parameter : m_parameters) {
		StringName name = vformat("parameters/%s", parameter.key);
		r_props->push_back(PropertyInfo(Variant::FLOAT, name));
	}
	for (const KeyValue<StringName, Track> &track : m_tracks) {
		String base_path = vformat("tracks/%s/", track.key);
		for (int i = 0; i < track.value.items.size(); i++) {
			r_props->push_back(PropertyInfo(Variant::OBJECT, vformat("%sitems/%d", base_path, i), PROPERTY_HINT_RESOURCE_TYPE, "AudioStreamTrackItem"));
		}
	}
}

bool AudioStreamGraph::_get(const StringName &p_property, Variant &r_value) const {
	String name = p_property;
	if (name.begins_with("nodes/")) {
		int id = name.get_slicec('/', 1).to_int();
		r_value = get_node(id);
		return true;
	} else if (name.begins_with("parameters/")) {
		StringName param_name = name.get_slicec('/', 1);
		if (m_parameters.has(param_name)) {
			r_value = m_parameters[param_name];
			return true;
		}
	} else if (name.begins_with("tracks/")) {
		StringName track_name = name.get_slicec('/', 1);
		int item_num = name.get_slicec('/', 2).to_int();
		if (m_tracks.has(track_name)) {
			const Track &track = m_tracks[track_name];
			if (item_num >= 0 && item_num < track.items.size()) {
				r_value = track.items[item_num];
				return true;
			}
		}
	}

	return false;
}

bool AudioStreamGraph::_set(const StringName &p_property, const Variant &p_value) {
	String name = p_property;
	if (name.begins_with("nodes/")) {
		int id = name.get_slicec('/', 1).to_int();
		set_node(id, p_value);
		return true;
	} else if (name.begins_with("parameters/")) {
		StringName param_name = name.get_slicec('/', 1);
		m_parameters[param_name] = p_value;
		return true;
	} else if (name.begins_with("tracks/")) {
		StringName track_name = name.get_slicec('/', 1);
		int item_num = name.get_slicec('/', 2).to_int();
		if (!m_tracks.has(track_name)) {
			m_tracks.insert(track_name, Track());
		} else {
			Track &track = m_tracks[track_name];
			if (item_num <= 0 && item_num < track.items.size()) {
				track.items.write[item_num] = p_value;
				return true;
			} else if (item_num == track.items.size()) {
				track.items.push_back(p_value);
				return true;
			}
		}
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

	ClassDB::bind_method(D_METHOD("add_parameter", "name", "default_value"), &AudioStreamGraph::add_parameter);
	ClassDB::bind_method(D_METHOD("remove_parameter", "name"), &AudioStreamGraph::remove_parameter);

	ClassDB::bind_method(D_METHOD("add_item_to_track", "track_name", "item"), &AudioStreamGraph::add_item_to_track);
	ClassDB::bind_method(D_METHOD("remove_item_from_track", "item"), &AudioStreamGraph::remove_item_from_track);
}
