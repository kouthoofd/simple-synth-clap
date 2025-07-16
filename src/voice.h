#pragma once

#include <cmath>

class Voice {
public:
    Voice();
    ~Voice() = default;

    void note_on(int note, double velocity, double sample_rate);
    void note_off();
    void set_adsr(double attack, double decay, double sustain, double release);
    void set_waveform(int waveform);
    bool is_active() const { return active_; }
    int get_note() const { return note_; }
    
    double process();

private:
    enum EnvelopeState {
        ENV_IDLE,
        ENV_ATTACK,
        ENV_DECAY,
        ENV_SUSTAIN,
        ENV_RELEASE
    };

    bool active_;
    int note_;
    double velocity_;
    double frequency_;
    double sample_rate_;
    
    // Oscillator
    double phase_;
    double phase_increment_;
    
    // Envelope
    EnvelopeState env_state_;
    double env_level_;
    double attack_time_;
    double decay_time_;
    double sustain_level_;
    double release_time_;
    double env_increment_;
    
    // Safety counter to prevent hanging notes
    int safety_counter_;
    
    // Waveform
    int waveform_;
    
    void calculate_frequency();
    void update_envelope();
    double generate_waveform();
};