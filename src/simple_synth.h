#pragma once

#include <clap/clap.h>
#include <vector>
#include <memory>
#include "voice.h"

class SimpleSynthUI;

class SimpleSynth {
public:
    SimpleSynth(const clap_host_t* host);
    ~SimpleSynth();

    // CLAP plugin interface
    bool init();
    void destroy();
    bool activate(double sample_rate, uint32_t min_frames, uint32_t max_frames);
    void deactivate();
    bool start_processing();
    void stop_processing();
    void reset();
    clap_process_status process(const clap_process_t* process);

    // Parameters
    uint32_t params_count();
    bool params_get_info(uint32_t param_index, clap_param_info_t* param_info);
    bool params_get_value(clap_id param_id, double* value);
    bool params_value_to_text(clap_id param_id, double value, char* display, uint32_t size);
    bool params_text_to_value(clap_id param_id, const char* display, double* value);
    void params_flush(const clap_input_events_t* in, const clap_output_events_t* out);

    // Note ports
    uint32_t note_ports_count(bool is_input);
    bool note_ports_get(uint32_t index, bool is_input, clap_note_port_info_t* info);

    // Audio ports
    uint32_t audio_ports_count(bool is_input);
    bool audio_ports_get(uint32_t index, bool is_input, clap_audio_port_info_t* info);

    // GUI interface (disabled for now)
    // SimpleSynthUI* get_ui() { return ui_.get(); }

private:
    enum ParamIds {
        PARAM_ATTACK = 0,
        PARAM_DECAY,
        PARAM_SUSTAIN,
        PARAM_RELEASE,
        PARAM_VOLUME,
        PARAM_WAVEFORM,
        PARAM_COUNT
    };

    const clap_host_t* host_;
    double sample_rate_;
    bool is_active_;
    bool is_processing_;

    // Parameters
    double attack_;
    double decay_;
    double sustain_;
    double release_;
    double volume_;
    double waveform_;

    // Filter removed for now

    // Voice management
    static constexpr int MAX_VOICES = 16;
    std::vector<std::unique_ptr<Voice>> voices_;
    int next_voice_index_;

    // UI (disabled for now)
    // std::unique_ptr<SimpleSynthUI> ui_;

    void process_events(const clap_input_events_t* events);
    void handle_note_on(int note, double velocity);
    void handle_note_off(int note);
    Voice* find_voice_for_note(int note);
    Voice* get_free_voice();
};