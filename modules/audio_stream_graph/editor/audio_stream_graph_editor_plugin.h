#ifndef AUDIO_STREAM_GRAPH_EDITOR_PLUGIN_H
#define AUDIO_STREAM_GRAPH_EDITOR_PLUGIN_H

#include "audio_stream_graph_editor.h"
#include "editor/editor_plugin.h"

class AudioStreamGraphEditorPlugin : public EditorPlugin {
	GDCLASS(AudioStreamGraphEditorPlugin, EditorPlugin);

private:
	AudioStreamGraph *m_resource;
	Button *m_bottom_button;
	AudioStreamGraphEditor *m_editor;

public:
	virtual bool handles(Object *p_object) const override;
	virtual void make_visible(bool p_visible) override;
	virtual void edit(Object *p_object) override;

	AudioStreamGraphEditorPlugin();
};

#endif // AUDIO_STREAM_GRAPH_EDITOR_PLUGIN_H
