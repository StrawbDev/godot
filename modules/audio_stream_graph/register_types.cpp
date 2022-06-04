#include "register_types.h"
#include "audio_stream_graph.h"
#include "audio_stream_playback_graph.h"

#ifdef TOOLS_ENABLED
#include "editor/audio_stream_graph_editor_plugin.h"
#endif

void initialize_audio_stream_graph_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(AudioStreamGraph);
		GDREGISTER_CLASS(AudioStreamPlaybackGraph);

		GDREGISTER_ABSTRACT_CLASS(AudioStreamGraphNode);
		GDREGISTER_CLASS(AudioStreamGraphNodeStream);
		GDREGISTER_CLASS(AudioStreamGraphNodeOutput);
		GDREGISTER_CLASS(AudioStreamGraphNodeMix);
		GDREGISTER_CLASS(AudioStreamGraphNodeParameter);
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::add_by_type<AudioStreamGraphEditorPlugin>();
	}
#endif
}

void uninitialize_audio_stream_graph_module(ModuleInitializationLevel p_level) {
}
