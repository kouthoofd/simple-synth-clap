#include <clap/clap.h>
#include "simple_synth.h"
#include <cstring>

// Plugin descriptor
static const clap_plugin_descriptor_t plugin_descriptor = {
    .clap_version = CLAP_VERSION_INIT,
    .id = "com.polarity.simple-synth",
    .name = "Simple Synth",
    .vendor = "Polarity Music",
    .url = "https://github.com/polarity/polarity-music-tools",
    .manual_url = "",
    .support_url = "",
    .version = "1.0.0",
    .description = "A simple sine wave synthesizer with ADSR envelope",
    .features = (const char*[]){
        CLAP_PLUGIN_FEATURE_INSTRUMENT,
        CLAP_PLUGIN_FEATURE_SYNTHESIZER,
        nullptr
    }
};

// Plugin implementation
struct PluginData {
    SimpleSynth* synth;
    const clap_host_t* host;
};

static bool plugin_init(const clap_plugin_t* plugin) {
    PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
    return data->synth->init();
}

static void plugin_destroy(const clap_plugin_t* plugin) {
    PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
    data->synth->destroy();
    delete data->synth;
    delete data;
}

static bool plugin_activate(const clap_plugin_t* plugin, double sample_rate, 
                           uint32_t min_frames, uint32_t max_frames) {
    PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
    return data->synth->activate(sample_rate, min_frames, max_frames);
}

static void plugin_deactivate(const clap_plugin_t* plugin) {
    PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
    data->synth->deactivate();
}

static bool plugin_start_processing(const clap_plugin_t* plugin) {
    PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
    return data->synth->start_processing();
}

static void plugin_stop_processing(const clap_plugin_t* plugin) {
    PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
    data->synth->stop_processing();
}

static void plugin_reset(const clap_plugin_t* plugin) {
    PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
    data->synth->reset();
}

static clap_process_status plugin_process(const clap_plugin_t* plugin, 
                                         const clap_process_t* process) {
    PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
    return data->synth->process(process);
}

static const void* plugin_get_extension(const clap_plugin_t* plugin, const char* id) {
    PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
    
    if (std::strcmp(id, CLAP_EXT_PARAMS) == 0) {
        static const clap_plugin_params_t params_ext = {
            .count = [](const clap_plugin_t* plugin) -> uint32_t {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                return data->synth->params_count();
            },
            .get_info = [](const clap_plugin_t* plugin, uint32_t param_index, 
                          clap_param_info_t* param_info) -> bool {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                return data->synth->params_get_info(param_index, param_info);
            },
            .get_value = [](const clap_plugin_t* plugin, clap_id param_id, 
                           double* value) -> bool {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                return data->synth->params_get_value(param_id, value);
            },
            .value_to_text = [](const clap_plugin_t* plugin, clap_id param_id, 
                               double value, char* display, uint32_t size) -> bool {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                return data->synth->params_value_to_text(param_id, value, display, size);
            },
            .text_to_value = [](const clap_plugin_t* plugin, clap_id param_id, 
                               const char* display, double* value) -> bool {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                return data->synth->params_text_to_value(param_id, display, value);
            },
            .flush = [](const clap_plugin_t* plugin, const clap_input_events_t* in, 
                       const clap_output_events_t* out) {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                data->synth->params_flush(in, out);
            }
        };
        return &params_ext;
    }
    
    if (std::strcmp(id, CLAP_EXT_NOTE_PORTS) == 0) {
        static const clap_plugin_note_ports_t note_ports_ext = {
            .count = [](const clap_plugin_t* plugin, bool is_input) -> uint32_t {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                return data->synth->note_ports_count(is_input);
            },
            .get = [](const clap_plugin_t* plugin, uint32_t index, bool is_input, 
                     clap_note_port_info_t* info) -> bool {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                return data->synth->note_ports_get(index, is_input, info);
            }
        };
        return &note_ports_ext;
    }
    
    if (std::strcmp(id, CLAP_EXT_AUDIO_PORTS) == 0) {
        static const clap_plugin_audio_ports_t audio_ports_ext = {
            .count = [](const clap_plugin_t* plugin, bool is_input) -> uint32_t {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                return data->synth->audio_ports_count(is_input);
            },
            .get = [](const clap_plugin_t* plugin, uint32_t index, bool is_input, 
                     clap_audio_port_info_t* info) -> bool {
                PluginData* data = static_cast<PluginData*>(plugin->plugin_data);
                return data->synth->audio_ports_get(index, is_input, info);
            }
        };
        return &audio_ports_ext;
    }
    
    // GUI extension disabled for now
    /*
    if (std::strcmp(id, CLAP_EXT_GUI) == 0) {
        // GUI implementation would go here
        return nullptr;
    }
    */
    
    return nullptr;
}

static void plugin_on_main_thread(const clap_plugin_t* plugin) {
    // Not used in this simple implementation
}

// Plugin factory
static const clap_plugin_t* plugin_factory_create_plugin(
    const clap_plugin_factory_t* factory,
    const clap_host_t* host,
    const char* plugin_id) {
    
    if (std::strcmp(plugin_id, plugin_descriptor.id) != 0) {
        return nullptr;
    }
    
    PluginData* data = new PluginData;
    data->host = host;
    data->synth = new SimpleSynth(host);
    
    clap_plugin_t* plugin = new clap_plugin_t;
    plugin->desc = &plugin_descriptor;
    plugin->plugin_data = data;
    plugin->init = plugin_init;
    plugin->destroy = plugin_destroy;
    plugin->activate = plugin_activate;
    plugin->deactivate = plugin_deactivate;
    plugin->start_processing = plugin_start_processing;
    plugin->stop_processing = plugin_stop_processing;
    plugin->reset = plugin_reset;
    plugin->process = plugin_process;
    plugin->get_extension = plugin_get_extension;
    plugin->on_main_thread = plugin_on_main_thread;
    
    return plugin;
}

static uint32_t plugin_factory_get_plugin_count(const clap_plugin_factory_t* factory) {
    return 1;
}

static const clap_plugin_descriptor_t* plugin_factory_get_plugin_descriptor(
    const clap_plugin_factory_t* factory, uint32_t index) {
    
    if (index == 0) {
        return &plugin_descriptor;
    }
    return nullptr;
}

static const clap_plugin_factory_t plugin_factory = {
    .get_plugin_count = plugin_factory_get_plugin_count,
    .get_plugin_descriptor = plugin_factory_get_plugin_descriptor,
    .create_plugin = plugin_factory_create_plugin
};

// Entry point
extern "C" {
    CLAP_EXPORT const clap_plugin_entry_t clap_entry = {
        .clap_version = CLAP_VERSION_INIT,
        .init = [](const char* plugin_path) -> bool {
            return true;
        },
        .deinit = []() {
            // Cleanup if needed
        },
        .get_factory = [](const char* factory_id) -> const void* {
            if (std::strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0) {
                return &plugin_factory;
            }
            return nullptr;
        }
    };
}