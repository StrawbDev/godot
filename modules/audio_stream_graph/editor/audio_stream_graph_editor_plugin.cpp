#include "audio_stream_graph_editor_plugin.h"
#include "editor/editor_scale.h"

AudioStreamGraphEditorPlugin::AudioStreamGraphEditorPlugin() {
	m_editor = memnew(AudioStreamGraphEditor);
	m_editor->set_custom_minimum_size(Size2(0, 300) * EDSCALE);
	m_bottom_button = add_control_to_bottom_panel(m_editor, TTR("AudioStreamGenerative"));
	m_bottom_button->hide();
}

bool AudioStreamGraphEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("AudioStreamGenerative");
}

void AudioStreamGraphEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		make_bottom_panel_item_visible(m_editor);
		m_bottom_button->show();
	} else {
		if (m_editor->is_visible_in_tree()) {
			hide_bottom_panel();
		}
		m_bottom_button->hide();
	}
}

void AudioStreamGraphEditorPlugin::edit(Object *p_object) {
	AudioStreamGraph *resource = Object::cast_to<AudioStreamGraph>(p_object);
	ERR_FAIL_COND(resource == nullptr);
	m_editor->edit(resource);
}
