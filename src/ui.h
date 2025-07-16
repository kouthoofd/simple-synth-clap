#pragma once

#include <clap/clap.h>

class SimpleSynth;

class SimpleSynthUI {
public:
    SimpleSynthUI(SimpleSynth* synth, const clap_host_t* host);
    ~SimpleSynthUI();

    // CLAP GUI interface
    bool is_api_supported(const char* api, bool is_floating);
    bool get_preferred_api(const char** api, bool* is_floating);
    bool create(const char* api, bool is_floating);
    void destroy();
    bool set_scale(double scale);
    bool get_size(uint32_t* width, uint32_t* height);
    bool can_resize();
    bool get_resize_hints(clap_gui_resize_hints_t* hints);
    bool adjust_size(uint32_t* width, uint32_t* height);
    bool set_size(uint32_t width, uint32_t height);
    bool set_parent(const clap_window_t* window);
    bool set_transient(const clap_window_t* window);
    void suggest_title(const char* title);
    bool show();
    bool hide();

    void on_parameter_changed(uint32_t param_id, double value);

private:
    SimpleSynth* synth_;
    const clap_host_t* host_;
    
    // Platform-specific implementation details hidden via pimpl
    struct Impl;
    Impl* impl_;

    bool created_;
    double scale_;
    uint32_t width_;
    uint32_t height_;
    
    void create_controls();
    void update_parameter_display(uint32_t param_id);
};