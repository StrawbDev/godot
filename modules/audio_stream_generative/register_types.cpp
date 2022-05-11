#include "modules/audio_stream_generative/register_types.h"
#include "modules/audio_stream_generative/audio_stream_generative.h"

#ifdef TOOLS_ENABLED
#include "modules/audio_stream_generative/editor/audio_stream_generative_editor_plugin.h"
#endif

void initialize_audio_stream_generative_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(AudioStreamGenerative);
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::add_by_type<AudioStreamGenerativeEditorPlugin>();
	}
#endif
}

void uninitialize_audio_stream_generative_module(ModuleInitializationLevel p_level) {
}
