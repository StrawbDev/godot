#ifndef AUDIO_STREAM_GENERATIVE_EDITOR_PLUGIN_H
#define AUDIO_STREAM_GENERATIVE_EDITOR_PLUGIN_H

#include "editor/editor_plugin.h"
#include "modules/audio_stream_generative/editor/audio_stream_generative_editor.h"

class AudioStreamGenerativeEditorPlugin : public EditorPlugin {
	GDCLASS(AudioStreamGenerativeEditorPlugin, EditorPlugin);

private:
	Button *m_bottom_button;
	AudioStreamGenerativeEditor *m_editor;

public:
	virtual bool handles(Object *p_object) const override;
	virtual void make_visible(bool p_visible) override;
	virtual void edit(Object *p_object) override;

	AudioStreamGenerativeEditorPlugin();
};

#endif
