#include "modules/audio_stream_generative/editor/audio_stream_generative_graph_nodes.h"
#include "editor/editor_scale.h"
#include "modules/audio_stream_generative/editor/audio_stream_generative_editor.h"

AudioStreamGenerativeNodeStream::AudioStreamGenerativeNodeStream() {
	set_custom_minimum_size(Size2(200, 100) * EDSCALE);
	set_title(TTR("AudioStream"));
	m_stream_select = memnew(EditorResourcePicker);
	m_stream_select->set_base_type("AudioStream");
	m_stream_select->set_h_size_flags(SIZE_EXPAND_FILL);
	add_child(m_stream_select);
	set_slot_enabled_right(0, true);
	set_slot_type_right(0, AudioStreamGenerativeEditor::SLOT_TYPE_AUDIO);
	set_slot_color_right(0, AudioStreamGenerativeEditor::SLOT_COLOR_AUDIO);
}

////////////////////////////

AudioStreamGenerativeNodeOutput::AudioStreamGenerativeNodeOutput() {
	set_title(TTR("Output"));
	Label *label = memnew(Label);
	label->set_text(TTR("Output"));
	add_child(label);
	set_slot_enabled_left(0, true);
	set_slot_type_left(0, AudioStreamGenerativeEditor::SLOT_TYPE_AUDIO);
	set_slot_color_left(0, AudioStreamGenerativeEditor::SLOT_COLOR_AUDIO);
}
