#include "audio_stream_playback_graph.h"

#warning "TODO: this all needs to be protected against ill-formed bytecode/out of space problems"
#warning "TODO: eliminate allocations during mix"

int AudioStreamPlaybackGraph::get_sample_rate() const {
#warning "TODO: proper sample rate support. Currently everything must be 44100Hz to work"
	return 44100;
}

AudioStreamPlaybackGraph::~AudioStreamPlaybackGraph() {
	if (m_parameters) {
		memdelete_arr(m_parameters);
		m_parameters = nullptr;
	}
}

void AudioStreamPlaybackGraph::set_resource(Ref<AudioStreamGraph> resource) {
	m_resource = resource;
	m_program = m_resource->compile();
	for (Ref<AudioStream> &stream : m_program.audio_inputs) {
		Ref<AudioStreamPlayback> playback = stream->instance_playback();
		playback->start();
		m_input_playbacks.push_back(playback);
	}

	if (m_parameters) {
		memdelete_arr(m_parameters);
	}
	m_parameters = memnew_arr(std::atomic<float>, m_program.parameters.size());
	int i = 0;
	for (const KeyValue<StringName, float> &entry : m_program.parameters) {
		m_parameter_lookup[entry.key] = i;
		m_parameters[i] = entry.value;
		i++;
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

void AudioStreamPlaybackGraph::set_parameter(StringName parameter, float value) {
	int idx = m_parameter_lookup[parameter];
	m_parameters[idx] = value;
}

void AudioStreamPlaybackGraph::_run_program(AudioFrame *buffer, float rate_scale, int num_frames) {
	using Opcode = AudioStreamGraph::Opcode;
	int total_frames = num_frames;
	int to_mix = total_frames;
	int offset = 0;

	while (to_mix > 0) {
		int next_size = MIN(to_mix, 512);

		m_current_buffer = 0;
		for (Ref<AudioStreamPlayback> &input : m_input_playbacks) {
			input->mix(m_buffers[m_current_buffer], rate_scale, next_size);
			m_current_buffer++;
		}

		for (const AudioStreamGraph::Bytecode &instruction : m_program.bytecode) {
			if (!m_execution_ok) {
				break;
			}

			switch (instruction.op) {
				case Opcode::COPY_TO_OUTPUT:
					_op_copy_to_output(instruction, buffer, offset, next_size);
					break;

				case Opcode::DUP:
					_op_dup(instruction);
					break;

				case Opcode::MIX:
					_op_mix(instruction, next_size);
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
}

int AudioStreamPlaybackGraph::mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) {
	m_execution_ok = true;

	if (m_resource.is_null() || !m_program.ok || !m_playing) {
		m_execution_ok = false;
	}

	_run_program(p_buffer, p_rate_scale, p_frames);

	if (!m_execution_ok) {
		memset(p_buffer, 0, sizeof(AudioFrame) * p_frames);
	}

	return p_frames;
}

AudioStreamPlaybackGraph::StackValue AudioStreamPlaybackGraph::_pop() {
	if (m_program_stack.size() == 0) {
		StackValue result;
		result.type = STACK_VALUE_INVALID;
		result.value.i = 0;
		m_execution_ok = false;
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
	} else {
		m_execution_ok = false;
	}
}

void AudioStreamPlaybackGraph::_op_dup(const AudioStreamGraph::Bytecode &instruction) {
	StackValue dup = _pop();
	m_program_stack.push_back(dup);
	m_program_stack.push_back(dup);
}

void AudioStreamPlaybackGraph::_op_mix(const AudioStreamGraph::Bytecode &instruction, int num_frames) {
	StackValue audio2 = _pop();
	StackValue mix2 = _pop();
	StackValue audio1 = _pop();
	StackValue mix1 = _pop();

	int result_buffer = m_current_buffer;
	m_current_buffer++;

	AudioFrame *audio1_buf = m_buffers[audio1.value.i];
	AudioFrame *audio2_buf = m_buffers[audio2.value.i];
	AudioFrame *result_buf = m_buffers[result_buffer];

	for (int i = 0; i < num_frames; i++) {
		result_buf[i] = mix1.value.f * audio1_buf[i];
		result_buf[i] += mix2.value.f * audio2_buf[i];
	}

	StackValue result;
	result.type = STACK_VALUE_AUDIO_REF;
	result.value.i = result_buffer;
	m_program_stack.push_back(result);
}

void AudioStreamPlaybackGraph::_op_push_audio(const AudioStreamGraph::Bytecode &instruction) {
	StackValue ref;
	ref.type = STACK_VALUE_AUDIO_REF;
	ref.value.i = instruction.operand;
	m_program_stack.push_back(ref);
}

void AudioStreamPlaybackGraph::_op_push_const(const AudioStreamGraph::Bytecode &instruction) {
	StackValue constant;
	constant.type = STACK_VALUE_FLOAT;
	constant.value.f = instruction.operand;
	m_program_stack.push_back(constant);
}

void AudioStreamPlaybackGraph::_op_push_param(const AudioStreamGraph::Bytecode &instruction) {
	StringName param_name = instruction.operand;
	int param_idx = m_parameter_lookup[param_name];
	StackValue param_value;
	param_value.type = STACK_VALUE_FLOAT;
	param_value.value.f = m_parameters[param_idx];
	m_program_stack.push_back(param_value);
}

void AudioStreamPlaybackGraph::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_parameter", "parameter", "value"), &AudioStreamPlaybackGraph::set_parameter);
}
