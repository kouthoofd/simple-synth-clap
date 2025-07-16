#include "simple_synth.h"
// #include "ui.h"  // Disabled for now
#include <cstring>
#include <algorithm>

SimpleSynth::SimpleSynth(const clap_host_t* host)
    : host_(host)
    , sample_rate_(44100.0)
    , is_active_(false)
    , is_processing_(false)
    , attack_(0.01)
    , decay_(0.1)
    , sustain_(0.7)
    , release_(0.3)
    , volume_(0.8)
    , waveform_(0.0)
    , next_voice_index_(0)
{
    voices_.reserve(MAX_VOICES);
    for (int i = 0; i < MAX_VOICES; ++i) {
        voices_.push_back(std::make_unique<Voice>());
    }
}

SimpleSynth::~SimpleSynth() = default;

bool SimpleSynth::init() {
    // Create UI (disabled for now)
    // ui_ = std::make_unique<SimpleSynthUI>(this, host_);
    return true;
}

void SimpleSynth::destroy() {
    // Cleanup handled by destructor
}

bool SimpleSynth::activate(double sample_rate, uint32_t min_frames, uint32_t max_frames) {
    sample_rate_ = sample_rate;
    is_active_ = true;
    
    // Filter removed for now
    
    return true;
}

void SimpleSynth::deactivate() {
    is_active_ = false;
}

bool SimpleSynth::start_processing() {
    is_processing_ = true;
    return true;
}

void SimpleSynth::stop_processing() {
    is_processing_ = false;
}

void SimpleSynth::reset() {
    for (auto& voice : voices_) {
        voice->note_off();
    }
    
    // Filter removed for now
}

clap_process_status SimpleSynth::process(const clap_process_t* process) {
    if (!is_processing_) {
        return CLAP_PROCESS_SLEEP;
    }

    // Process input events
    process_events(process->in_events);

    // Get audio output buffer
    float* output_left = process->audio_outputs[0].data32[0];
    float* output_right = process->audio_outputs[0].data32[1];
    uint32_t frame_count = process->frames_count;

    // Clear output buffer
    std::memset(output_left, 0, frame_count * sizeof(float));
    std::memset(output_right, 0, frame_count * sizeof(float));

    // Process voices
    for (uint32_t frame = 0; frame < frame_count; ++frame) {
        double sample = 0.0;
        
        for (auto& voice : voices_) {
            if (voice->is_active()) {
                sample += voice->process();
            }
        }
        
        sample *= volume_;
        
        // Apply to both channels (mono to stereo)
        output_left[frame] = static_cast<float>(sample);
        output_right[frame] = static_cast<float>(sample);
    }

    return CLAP_PROCESS_CONTINUE;
}

void SimpleSynth::process_events(const clap_input_events_t* events) {
    uint32_t event_count = events->size(events);
    
    for (uint32_t i = 0; i < event_count; ++i) {
        const clap_event_header_t* event = events->get(events, i);
        
        switch (event->type) {
            case CLAP_EVENT_NOTE_ON: {
                const clap_event_note_t* note_event = 
                    reinterpret_cast<const clap_event_note_t*>(event);
                handle_note_on(note_event->key, note_event->velocity);
                break;
            }
            
            case CLAP_EVENT_NOTE_OFF: {
                const clap_event_note_t* note_event = 
                    reinterpret_cast<const clap_event_note_t*>(event);
                handle_note_off(note_event->key);
                break;
            }
            
            case CLAP_EVENT_MIDI: {
                // Handle MIDI events (Bitwig might send these instead of CLAP note events)
                const clap_event_midi_t* midi_event = 
                    reinterpret_cast<const clap_event_midi_t*>(event);
                
                uint8_t status = midi_event->data[0];
                uint8_t note = midi_event->data[1];
                uint8_t velocity = midi_event->data[2];
                
                if ((status & 0xF0) == 0x90 && velocity > 0) {
                    // Note On
                    handle_note_on(note, velocity / 127.0);
                } else if ((status & 0xF0) == 0x80 || ((status & 0xF0) == 0x90 && velocity == 0)) {
                    // Note Off - make sure we handle this properly
                    handle_note_off(note);
                }
                break;
            }
            
            case CLAP_EVENT_PARAM_VALUE: {
                const clap_event_param_value_t* param_event = 
                    reinterpret_cast<const clap_event_param_value_t*>(event);
                
                switch (param_event->param_id) {
                    case PARAM_ATTACK:
                        attack_ = param_event->value;
                        break;
                    case PARAM_DECAY:
                        decay_ = param_event->value;
                        break;
                    case PARAM_SUSTAIN:
                        sustain_ = param_event->value;
                        break;
                    case PARAM_RELEASE:
                        release_ = param_event->value;
                        break;
                    case PARAM_VOLUME:
                        volume_ = param_event->value;
                        break;
                    case PARAM_WAVEFORM:
                        waveform_ = param_event->value;
                        break;
                }
                break;
            }
        }
    }
}

void SimpleSynth::handle_note_on(int note, double velocity) {
    Voice* voice = get_free_voice();
    if (voice) {
        voice->set_adsr(attack_, decay_, sustain_, release_);
        voice->set_waveform(static_cast<int>(waveform_));
        voice->note_on(note, velocity, sample_rate_);
    }
}

void SimpleSynth::handle_note_off(int note) {
    // Turn off ALL voices playing this note (in case of duplicates)
    for (auto& voice : voices_) {
        if (voice->is_active() && voice->get_note() == note) {
            voice->note_off();
        }
    }
}

Voice* SimpleSynth::find_voice_for_note(int note) {
    // Find the most recent voice playing this note
    Voice* found_voice = nullptr;
    for (auto& voice : voices_) {
        if (voice->is_active() && voice->get_note() == note) {
            found_voice = voice.get();
            // Don't break - keep looking for the most recent one
        }
    }
    return found_voice;
}

Voice* SimpleSynth::get_free_voice() {
    // First, try to find an inactive voice
    for (auto& voice : voices_) {
        if (!voice->is_active()) {
            return voice.get();
        }
    }
    
    // If no free voice, steal the oldest one (round-robin)
    Voice* voice = voices_[next_voice_index_].get();
    next_voice_index_ = (next_voice_index_ + 1) % MAX_VOICES;
    return voice;
}

// Parameter interface implementation
uint32_t SimpleSynth::params_count() {
    return PARAM_COUNT;
}

bool SimpleSynth::params_get_info(uint32_t param_index, clap_param_info_t* param_info) {
    if (param_index >= PARAM_COUNT) {
        return false;
    }
    
    switch (param_index) {
        case PARAM_ATTACK:
            param_info->id = PARAM_ATTACK;
            std::strcpy(param_info->name, "Attack");
            std::strcpy(param_info->module, "Envelope");
            param_info->min_value = 0.001;
            param_info->max_value = 5.0;
            param_info->default_value = 0.01;
            param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
            break;
            
        case PARAM_DECAY:
            param_info->id = PARAM_DECAY;
            std::strcpy(param_info->name, "Decay");
            std::strcpy(param_info->module, "Envelope");
            param_info->min_value = 0.001;
            param_info->max_value = 5.0;
            param_info->default_value = 0.1;
            param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
            break;
            
        case PARAM_SUSTAIN:
            param_info->id = PARAM_SUSTAIN;
            std::strcpy(param_info->name, "Sustain");
            std::strcpy(param_info->module, "Envelope");
            param_info->min_value = 0.0;
            param_info->max_value = 1.0;
            param_info->default_value = 0.7;
            param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
            break;
            
        case PARAM_RELEASE:
            param_info->id = PARAM_RELEASE;
            std::strcpy(param_info->name, "Release");
            std::strcpy(param_info->module, "Envelope");
            param_info->min_value = 0.001;
            param_info->max_value = 5.0;
            param_info->default_value = 0.3;
            param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
            break;
            
        case PARAM_VOLUME:
            param_info->id = PARAM_VOLUME;
            std::strcpy(param_info->name, "Volume");
            std::strcpy(param_info->module, "Main");
            param_info->min_value = 0.0;
            param_info->max_value = 1.0;
            param_info->default_value = 0.8;
            param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
            break;
            
        case PARAM_WAVEFORM:
            param_info->id = PARAM_WAVEFORM;
            std::strcpy(param_info->name, "Waveform");
            std::strcpy(param_info->module, "Oscillator");
            param_info->min_value = 0.0;
            param_info->max_value = 4.0;
            param_info->default_value = 0.0;
            param_info->flags = CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_IS_STEPPED;
            break;
    }
    
    return true;
}

bool SimpleSynth::params_get_value(clap_id param_id, double* value) {
    switch (param_id) {
        case PARAM_ATTACK:
            *value = attack_;
            return true;
        case PARAM_DECAY:
            *value = decay_;
            return true;
        case PARAM_SUSTAIN:
            *value = sustain_;
            return true;
        case PARAM_RELEASE:
            *value = release_;
            return true;
        case PARAM_VOLUME:
            *value = volume_;
            return true;
        case PARAM_WAVEFORM:
            *value = waveform_;
            return true;
        default:
            return false;
    }
}

bool SimpleSynth::params_value_to_text(clap_id param_id, double value, char* display, uint32_t size) {
    switch (param_id) {
        case PARAM_ATTACK:
        case PARAM_DECAY:
        case PARAM_RELEASE:
            std::snprintf(display, size, "%.3f s", value);
            return true;
        case PARAM_SUSTAIN:
        case PARAM_VOLUME:
            std::snprintf(display, size, "%.1f%%", value * 100.0);
            return true;
        case PARAM_WAVEFORM: {
            const char* waveforms[] = {"Sine", "Square", "Saw", "Triangle", "Pulse"};
            int wave_index = static_cast<int>(value);
            if (wave_index >= 0 && wave_index < 5) {
                std::strcpy(display, waveforms[wave_index]);
            } else {
                std::strcpy(display, "Sine");
            }
            return true;
        }
        default:
            return false;
    }
}

bool SimpleSynth::params_text_to_value(clap_id param_id, const char* display, double* value) {
    // Simple implementation - just parse the number
    *value = std::atof(display);
    return true;
}

void SimpleSynth::params_flush(const clap_input_events_t* in, const clap_output_events_t* out) {
    process_events(in);
}

// Note ports
uint32_t SimpleSynth::note_ports_count(bool is_input) {
    return is_input ? 1 : 0;
}

bool SimpleSynth::note_ports_get(uint32_t index, bool is_input, clap_note_port_info_t* info) {
    if (!is_input || index != 0) {
        return false;
    }
    
    info->id = 0;
    std::strcpy(info->name, "Note Input");
    info->supported_dialects = CLAP_NOTE_DIALECT_MIDI;
    info->preferred_dialect = CLAP_NOTE_DIALECT_MIDI;
    
    return true;
}

// Audio ports
uint32_t SimpleSynth::audio_ports_count(bool is_input) {
    return is_input ? 0 : 1;
}

bool SimpleSynth::audio_ports_get(uint32_t index, bool is_input, clap_audio_port_info_t* info) {
    if (is_input || index != 0) {
        return false;
    }
    
    info->id = 0;
    std::strcpy(info->name, "Audio Output");
    info->channel_count = 2;
    info->flags = CLAP_AUDIO_PORT_IS_MAIN;
    info->port_type = CLAP_PORT_STEREO;
    info->in_place_pair = CLAP_INVALID_ID;
    
    return true;
}