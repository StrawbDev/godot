#ifndef AUDIO_STREAM_GRAPH_TRACK_EDITOR_H
#define AUDIO_STREAM_GRAPH_TRACK_EDITOR_H

#include "scene/gui/control.h"

class AudioStreamGraphTrackEditor : public Control {
	GDCLASS(AudioStreamGraphTrackEditor, Control);

protected:
	void _notification(int p_what);
};

#endif // AUDIO_STREAM_GRAPH_TRACK_EDITOR_H