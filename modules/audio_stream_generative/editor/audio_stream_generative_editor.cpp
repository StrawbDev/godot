#include "modules/audio_stream_generative/editor/audio_stream_generative_editor.h"
#include "scene/gui/label.h"

#include "audio_stream_generative_graph_nodes.h"

const Color AudioStreamGenerativeEditor::SLOT_COLOR_AUDIO = Color(1, 0, 0);

void AudioStreamGenerativeEditor::_on_connection_request(StringName from, int from_slot, StringName to, int to_slot) {
	m_graph->connect_node(from, from_slot, to, to_slot);
}

void AudioStreamGenerativeEditor::_on_disconnection_request(StringName from, int from_slot, StringName to, int to_slot) {
	m_graph->disconnect_node(from, from_slot, to, to_slot);
}

void AudioStreamGenerativeEditor::edit(AudioStreamGenerative *resource) {
	m_current_resource = resource;
}

AudioStreamGenerativeEditor::AudioStreamGenerativeEditor() {
	m_graph = memnew(GraphEdit);
	m_graph->set_v_size_flags(SIZE_EXPAND_FILL);
	m_graph->add_valid_right_disconnect_type(SLOT_TYPE_AUDIO);
	m_graph->connect("connection_request", callable_mp(this, &AudioStreamGenerativeEditor::_on_connection_request));
	m_graph->connect("disconnection_request", callable_mp(this, &AudioStreamGenerativeEditor::_on_disconnection_request));
	add_child(m_graph);

	AudioStreamGenerativeNodeStream *test = memnew(AudioStreamGenerativeNodeStream);
	m_graph->add_child(test);

	AudioStreamGenerativeNodeOutput *test2 = memnew(AudioStreamGenerativeNodeOutput);
	m_graph->add_child(test2);
}
