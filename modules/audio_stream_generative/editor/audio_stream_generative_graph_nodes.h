#ifndef AUDIO_STREAM_GENERATIVE_GRAPH_NODES_H
#define AUDIO_STREAM_GENERATIVE_GRAPH_NODES_H

#include "editor/editor_resource_picker.h"
#include "scene/gui/graph_node.h"

class AudioStreamGenerativeNodeStream : public GraphNode {
	GDCLASS(AudioStreamGenerativeNodeStream, GraphNode);

	EditorResourcePicker *m_stream_select;

public:
	AudioStreamGenerativeNodeStream();
};

////////////////////////////////

class AudioStreamGenerativeNodeOutput : public GraphNode {
	GDCLASS(AudioStreamGenerativeNodeOutput, GraphNode);

public:
	AudioStreamGenerativeNodeOutput();
};

#endif