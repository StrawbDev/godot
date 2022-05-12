#include "register_types.h"
#include "audio_stream_graph.h"

#ifdef TOOLS_ENABLED
#include "editor/audio_stream_graph_editor_plugin.h"
#endif

void initialize_audio_stream_graph_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(AudioStreamGraph);
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::add_by_type<AudioStreamGraphEditorPlugin>();
	}
#endif
}

void uninitialize_audio_stream_graph_module(ModuleInitializationLevel p_level) {
}
