#include "modules/audio_stream_generative/register_types.h"
#include "modules/audio_stream_generative/audio_stream_generative.h"

void initialize_audio_stream_generative_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(AudioStreamGenerative);
}

void uninitialize_audio_stream_generative_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}
