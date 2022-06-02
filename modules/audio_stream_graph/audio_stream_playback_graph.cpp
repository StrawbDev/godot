#include "audio_stream_playback_graph.h"

#warning "TODO: this all needs to be protected against ill-formed bytecode/out of space problems"
#warning "TODO: eliminate allocations during mix"

int AudioStreamPlaybackGraph::get_sample_rate() const {
#warning "TODO: proper sample rate support. Currently everything must be 44100Hz to work"
	return 44100;
}

void AudioStreamPlaybackGraph::set_resource(Ref<AudioStreamGraph> resource) {
	m_resource = resource;
	m_program = m_resource->compile();
	for (Ref<AudioStream> &stream : m_program.audio_inputs) {
		Ref<AudioStreamPlayback> playback = stream->instance_playback();
		playback->start();
		m_input_playbacks.push_back(playback);
	}
}

void AudioStreamPlaybackGraph::start(float p_from_pos) {
	seek(p_from_pos);
	m_playing = true;
}

void AudioStreamPlaybackGraph::stop() {
	m_playing = false;
}

bool AudioStreamPlaybackGraph::is_playing() const {
	return m_playing;
}

int AudioStreamPlaybackGraph::get_loop_count() const {
	return 0;
}

float AudioStreamPlaybackGraph::get_playback_position() const {
	return m_position / float(get_sample_rate());
}

void AudioStreamPlaybackGraph::seek(float p_time) {
	if (p_time < 0) {
		m_position = 0;
	} else {
		m_position = p_time * get_sample_rate();
	}
}

int AudioStreamPlaybackGraph::mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) {
	using Opcode = AudioStreamGraph::Opcode;

	if (m_resource.is_null() || !m_program.ok || !m_playing) {
		for (int i = 0; i < p_frames; i++) {
			p_buffer[i] = AudioFrame{ 0, 0 };
		}
		return p_frames;
	}

	int total_frames = p_frames;
	int to_mix = total_frames;
	int offset = 0;

	while (to_mix > 0) {
		int next_size = MIN(to_mix, 512);

		m_current_buffer = 0;
		for (Ref<AudioStreamPlayback> &input : m_input_playbacks) {
			input->mix(m_buffers[m_current_buffer], p_rate_scale, next_size);
			m_current_buffer++;
		}

		for (const AudioStreamGraph::Bytecode &instruction : m_program.bytecode) {
			switch (instruction.op) {
				case Opcode::COPY_TO_OUTPUT:
					_op_copy_to_output(instruction, p_buffer, offset, next_size);
					break;

				case Opcode::DUP:
					_op_dup(instruction);
					break;

				case Opcode::MIX:
					_op_mix(instruction);
					break;

				case Opcode::PUSH_AUDIO:
					_op_push_audio(instruction);
					break;

				case Opcode::PUSH_CONST:
					_op_push_const(instruction);
					break;

				case Opcode::PUSH_PARAM:
					_op_push_param(instruction);
					break;
			}
		}

		to_mix -= next_size;
		offset += next_size;
	}

	return total_frames;
}

AudioStreamPlaybackGraph::StackValue AudioStreamPlaybackGraph::_pop() {
	if (m_program_stack.size() == 0) {
		StackValue result;
		result.type = STACK_VALUE_INVALID;
		result.value.i = 0;
		return result;
	}

	StackValue result = m_program_stack.get(m_program_stack.size() - 1);
	m_program_stack.remove_at(m_program_stack.size() - 1);
	return result;
}

void AudioStreamPlaybackGraph::_op_copy_to_output(const AudioStreamGraph::Bytecode &instruction, AudioFrame *output, int offset, int num_frames) {
	StackValue op = _pop();
	if (op.type == STACK_VALUE_AUDIO_REF) {
		int to_copy = op.value.i;
		memcpy(output + offset, m_buffers[to_copy], num_frames * sizeof(AudioFrame));
	}
}

void AudioStreamPlaybackGraph::_op_dup(const AudioStreamGraph::Bytecode &instruction) {
	StackValue dup = _pop();
	m_program_stack.push_back(dup);
	m_program_stack.push_back(dup);
}

void AudioStreamPlaybackGraph::_op_mix(const AudioStreamGraph::Bytecode &instruction) {
	StackValue audio1 = _pop();
	StackValue audio2 = _pop();
	StackValue mix1 = _pop();
	StackValue mix2 = _pop();
	// TODO
	StackValue result;
	result.type = STACK_VALUE_AUDIO_REF;
	result.value.i = 0;
	m_program_stack.push_back(result);
}

void AudioStreamPlaybackGraph::_op_push_audio(const AudioStreamGraph::Bytecode &instruction) {
	StackValue ref;
	ref.type = STACK_VALUE_AUDIO_REF;
	ref.value.i = instruction.operand.i;
	m_program_stack.push_back(ref);
}

void AudioStreamPlaybackGraph::_op_push_const(const AudioStreamGraph::Bytecode &instruction) {
	StackValue constant;
	constant.type = STACK_VALUE_FLOAT;
	constant.value.f = instruction.operand.f;
	m_program_stack.push_back(constant);
}

void AudioStreamPlaybackGraph::_op_push_param(const AudioStreamGraph::Bytecode &instruction) {
	// TODO
}
