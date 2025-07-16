#include <iostream>
#include <dlfcn.h>
#include <clap/clap.h>

int main() {
    // Load the plugin bundle
    const char* plugin_path = "build/SimpleSynthCLAP.clap/Contents/MacOS/SimpleSynthCLAP";
    
    void* handle = dlopen(plugin_path, RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load plugin: " << dlerror() << std::endl;
        return 1;
    }
    
    // Get the entry point
    const clap_plugin_entry_t* entry = (const clap_plugin_entry_t*)dlsym(handle, "clap_entry");
    
    if (!entry) {
        std::cerr << "Failed to find clap_entry symbol: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }
    if (!entry) {
        std::cerr << "clap_entry returned null" << std::endl;
        dlclose(handle);
        return 1;
    }
    
    std::cout << "Plugin loaded successfully!" << std::endl;
    std::cout << "CLAP version: " << entry->clap_version.major << "." 
              << entry->clap_version.minor << "." << entry->clap_version.revision << std::endl;
    
    // Initialize the plugin
    if (!entry->init(plugin_path)) {
        std::cerr << "Failed to initialize plugin" << std::endl;
        dlclose(handle);
        return 1;
    }
    
    // Get the factory
    const clap_plugin_factory_t* factory = 
        (const clap_plugin_factory_t*)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
    
    if (!factory) {
        std::cerr << "Failed to get plugin factory" << std::endl;
        entry->deinit();
        dlclose(handle);
        return 1;
    }
    
    uint32_t plugin_count = factory->get_plugin_count(factory);
    std::cout << "Plugin count: " << plugin_count << std::endl;
    
    if (plugin_count > 0) {
        const clap_plugin_descriptor_t* desc = factory->get_plugin_descriptor(factory, 0);
        if (desc) {
            std::cout << "Plugin ID: " << desc->id << std::endl;
            std::cout << "Plugin Name: " << desc->name << std::endl;
            std::cout << "Plugin Vendor: " << desc->vendor << std::endl;
            std::cout << "Plugin Version: " << desc->version << std::endl;
        }
    }
    
    entry->deinit();
    dlclose(handle);
    
    std::cout << "Plugin test completed successfully!" << std::endl;
    return 0;
}