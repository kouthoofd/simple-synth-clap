#include "voice.h"
#include <cmath>

Voice::Voice() 
    : active_(false)
    , note_(0)
    , velocity_(0.0)
    , frequency_(440.0)
    , sample_rate_(44100.0)
    , phase_(0.0)
    , phase_increment_(0.0)
    , env_state_(ENV_IDLE)
    , env_level_(0.0)
    , attack_time_(0.01)
    , decay_time_(0.1)
    , sustain_level_(0.7)
    , release_time_(0.3)
    , env_increment_(0.0)
    , safety_counter_(0)
    , waveform_(0)
{
}

void Voice::note_on(int note, double velocity, double sample_rate) {
    note_ = note;
    velocity_ = velocity;
    sample_rate_ = sample_rate;
    active_ = true;
    
    calculate_frequency();
    phase_ = 0.0;
    phase_increment_ = 2.0 * M_PI * frequency_ / sample_rate_;
    
    env_state_ = ENV_ATTACK;
    env_level_ = 0.0;
    safety_counter_ = 0;
    update_envelope();
}

void Voice::note_off() {
    if (active_ && env_state_ != ENV_RELEASE) {
        env_state_ = ENV_RELEASE;
        safety_counter_ = 0; // Reset safety counter for release phase
        update_envelope();
    }
}

void Voice::set_adsr(double attack, double decay, double sustain, double release) {
    attack_time_ = attack;
    decay_time_ = decay;
    sustain_level_ = sustain;
    release_time_ = release;
    
    if (active_) {
        update_envelope();
    }
}

void Voice::set_waveform(int waveform) {
    waveform_ = waveform;
}

double Voice::process() {
    if (!active_) {
        return 0.0;
    }
    
    // Safety counter only for extreme cases (30 seconds)
    safety_counter_++;
    if (safety_counter_ > static_cast<int>(sample_rate_ * 30)) {
        active_ = false;
        env_state_ = ENV_IDLE;
        env_level_ = 0.0;
        return 0.0;
    }
    
    // Generate waveform
    double sample = generate_waveform() * velocity_;
    phase_ += phase_increment_;
    if (phase_ >= 2.0 * M_PI) {
        phase_ -= 2.0 * M_PI;
    }
    
    // Apply envelope
    sample *= env_level_;
    
    // Update envelope
    switch (env_state_) {
        case ENV_ATTACK:
            env_level_ += env_increment_;
            if (env_level_ >= 1.0) {
                env_level_ = 1.0;
                env_state_ = ENV_DECAY;
                update_envelope();
            }
            break;
            
        case ENV_DECAY:
            env_level_ += env_increment_;
            if (env_level_ <= sustain_level_) {
                env_level_ = sustain_level_;
                env_state_ = ENV_SUSTAIN;
                env_increment_ = 0.0; // Stop changing in sustain
            }
            break;
            
        case ENV_SUSTAIN:
            env_level_ = sustain_level_;
            // Stay in sustain until note off
            break;
            
        case ENV_RELEASE:
            env_level_ += env_increment_;
            if (env_level_ <= 0.001) {
                env_level_ = 0.0;
                env_state_ = ENV_IDLE;
                active_ = false;
            }
            break;
            
        case ENV_IDLE:
            active_ = false;
            break;
    }
    
    return sample;
}

void Voice::calculate_frequency() {
    // Convert MIDI note to frequency: f = 440 * 2^((n-69)/12)
    frequency_ = 440.0 * std::pow(2.0, (note_ - 69) / 12.0);
    phase_increment_ = 2.0 * M_PI * frequency_ / sample_rate_;
}

void Voice::update_envelope() {
    switch (env_state_) {
        case ENV_ATTACK:
            if (attack_time_ > 0.0) {
                env_increment_ = 1.0 / (attack_time_ * sample_rate_);
            } else {
                env_increment_ = 1.0;
            }
            break;
            
        case ENV_DECAY:
            if (decay_time_ > 0.0) {
                env_increment_ = -(1.0 - sustain_level_) / (decay_time_ * sample_rate_);
            } else {
                env_increment_ = -(1.0 - sustain_level_);
            }
            break;
            
        case ENV_RELEASE:
            if (release_time_ > 0.0) {
                env_increment_ = -env_level_ / (release_time_ * sample_rate_);
            } else {
                env_increment_ = -env_level_;
            }
            break;
            
        default:
            env_increment_ = 0.0;
            break;
    }
}

double Voice::generate_waveform() {
    switch (waveform_) {
        case 0: // Sine
            return std::sin(phase_);
            
        case 1: // Square
            return (phase_ < M_PI) ? 1.0 : -1.0;
            
        case 2: // Saw
            return (2.0 * phase_ / (2.0 * M_PI)) - 1.0;
            
        case 3: // Triangle
            if (phase_ < M_PI) {
                return (2.0 * phase_ / M_PI) - 1.0;
            } else {
                return 3.0 - (2.0 * phase_ / M_PI);
            }
            
        case 4: // Pulse (25% duty cycle)
            return (phase_ < M_PI * 0.5) ? 1.0 : -1.0;
            
        default: // Default to sine
            return std::sin(phase_);
    }
}