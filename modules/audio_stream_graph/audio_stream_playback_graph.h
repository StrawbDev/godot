#ifndef AUDIO_STREAM_PLAYBACK_GRAPH_H
#define AUDIO_STREAM_PLAYBACK_GRAPH_H

#include "audio_stream_graph.h"
#include "servers/audio/audio_stream.h"
class AudioStreamPlaybackGraph : public AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackGraph, AudioStreamPlayback);

private:
	Ref<AudioStreamGraph> m_resource;
	uint64_t m_position = 0;
	bool m_playing = false;

	AudioStreamGraph::CompileResult m_program;

	using Buffer = AudioFrame[512];
	int m_current_buffer = 0;
	Buffer m_buffers[16];
	Vector<Ref<AudioStreamPlayback>> m_input_playbacks;

	enum StackValueType {
		STACK_VALUE_INVALID,
		STACK_VALUE_AUDIO_REF,
		STACK_VALUE_FLOAT,
	};

	struct StackValue {
		StackValueType type;
		union Inner {
			int i;
			float f;
		} value;
	};
	Vector<StackValue> m_program_stack;

	StackValue _pop();

	bool m_execution_ok;
	void _run_program(AudioFrame *buffer, float rate_scale, int num_frames);

	void _op_copy_to_output(const AudioStreamGraph::Bytecode &instruction, AudioFrame *output, int offset, int num_frames);
	void _op_dup(const AudioStreamGraph::Bytecode &instruction);
	void _op_mix(const AudioStreamGraph::Bytecode &instruction, int num_frames);
	void _op_push_audio(const AudioStreamGraph::Bytecode &instruction);
	void _op_push_const(const AudioStreamGraph::Bytecode &instruction);
	void _op_push_param(const AudioStreamGraph::Bytecode &instruction);

public:
	int get_sample_rate() const;

	virtual void start(float p_from_pos = 0.0) override;
	virtual void stop() override;
	virtual bool is_playing() const override;

	virtual int get_loop_count() const override;

	virtual float get_playback_position() const override;
	virtual void seek(float p_time) override;

	virtual int mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) override;

	void set_resource(Ref<AudioStreamGraph> resource);
};

#endif // AUDIO_STREAM_PLAYBACK_GRAPH_H