#ifndef AUDIO_STREAM_GRAPH_GRAPH_NODES_H
#define AUDIO_STREAM_GRAPH_GRAPH_NODES_H

#include "editor/editor_resource_picker.h"
#include "scene/gui/graph_node.h"

class AudioStreamGraphNodeStream : public GraphNode {
	GDCLASS(AudioStreamGraphNodeStream, GraphNode);

	EditorResourcePicker *m_stream_select;

public:
	AudioStreamGraphNodeStream();
};

////////////////////////////////

class AudioStreamGraphNodeOutput : public GraphNode {
	GDCLASS(AudioStreamGraphNodeOutput, GraphNode);

public:
	AudioStreamGraphNodeOutput();
};

#endif // AUDIO_STREAM_GRAPH_GRAPH_NODES_H