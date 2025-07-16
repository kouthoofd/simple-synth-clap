#include "ui.h"
#include "simple_synth.h"
#include <cstring>

#ifdef __APPLE__
#include <Cocoa/Cocoa.h>

@interface SliderTarget : NSObject
@property (nonatomic, assign) SimpleSynthUI* ui;
@property (nonatomic, assign) uint32_t param_id;
- (void)sliderChanged:(NSSlider*)sender;
@end

@implementation SliderTarget
- (void)sliderChanged:(NSSlider*)sender {
    if (self.ui) {
        self.ui->on_parameter_changed(self.param_id, [sender doubleValue]);
    }
}
@end

// Pimpl implementation for macOS
struct SimpleSynthUI::Impl {
    NSView* parent_view_;
    NSView* main_view_;
    NSSlider* attack_slider_;
    NSSlider* decay_slider_;
    NSSlider* sustain_slider_;
    NSSlider* release_slider_;
    NSSlider* volume_slider_;
    NSSlider* filter_cutoff_slider_;
    
    NSTextField* attack_label_;
    NSTextField* decay_label_;
    NSTextField* sustain_label_;
    NSTextField* release_label_;
    NSTextField* volume_label_;
    NSTextField* filter_cutoff_label_;
    
    NSTextField* attack_value_;
    NSTextField* decay_value_;
    NSTextField* sustain_value_;
    NSTextField* release_value_;
    NSTextField* volume_value_;
    NSTextField* filter_cutoff_value_;
    
    Impl() : parent_view_(nullptr), main_view_(nullptr),
             attack_slider_(nullptr), decay_slider_(nullptr),
             sustain_slider_(nullptr), release_slider_(nullptr),
             volume_slider_(nullptr), filter_cutoff_slider_(nullptr),
             attack_label_(nullptr), decay_label_(nullptr),
             sustain_label_(nullptr), release_label_(nullptr),
             volume_label_(nullptr), filter_cutoff_label_(nullptr),
             attack_value_(nullptr), decay_value_(nullptr),
             sustain_value_(nullptr), release_value_(nullptr),
             volume_value_(nullptr), filter_cutoff_value_(nullptr) {}
};

#else
// Stub implementation for non-macOS platforms
struct SimpleSynthUI::Impl {
    Impl() {}
};
#endif

SimpleSynthUI::SimpleSynthUI(SimpleSynth* synth, const clap_host_t* host)
    : synth_(synth)
    , host_(host)
    , impl_(new Impl())
    , created_(false)
    , scale_(1.0)
    , width_(400)
    , height_(300)
{
}

SimpleSynthUI::~SimpleSynthUI() {
    destroy();
    delete impl_;
}

bool SimpleSynthUI::is_api_supported(const char* api, bool is_floating) {
#ifdef __APPLE__
    return std::strcmp(api, CLAP_WINDOW_API_COCOA) == 0;
#else
    return false;
#endif
}

bool SimpleSynthUI::get_preferred_api(const char** api, bool* is_floating) {
#ifdef __APPLE__
    *api = CLAP_WINDOW_API_COCOA;
    *is_floating = false;
    return true;
#else
    return false;
#endif
}

bool SimpleSynthUI::create(const char* api, bool is_floating) {
#ifdef __APPLE__
    if (std::strcmp(api, CLAP_WINDOW_API_COCOA) != 0) {
        return false;
    }
    
    if (created_) {
        return true;
    }
    
    // Create main view
    NSRect frame = NSMakeRect(0, 0, width_, height_);
    impl_->main_view_ = [[NSView alloc] initWithFrame:frame];
    [impl_->main_view_ setWantsLayer:YES];
    impl_->main_view_.layer.backgroundColor = [[NSColor colorWithRed:0.2 green:0.2 blue:0.25 alpha:1.0] CGColor];
    
    create_controls();
    created_ = true;
    return true;
#else
    return false;
#endif
}

void SimpleSynthUI::destroy() {
#ifdef __APPLE__
    if (impl_->main_view_) {
        [impl_->main_view_ removeFromSuperview];
        impl_->main_view_ = nullptr;
    }
    impl_->parent_view_ = nullptr;
#endif
    created_ = false;
}

bool SimpleSynthUI::set_scale(double scale) {
    scale_ = scale;
    return true;
}

bool SimpleSynthUI::get_size(uint32_t* width, uint32_t* height) {
    *width = static_cast<uint32_t>(width_ * scale_);
    *height = static_cast<uint32_t>(height_ * scale_);
    return true;
}

bool SimpleSynthUI::can_resize() {
    return false;
}

bool SimpleSynthUI::get_resize_hints(clap_gui_resize_hints_t* hints) {
    return false;
}

bool SimpleSynthUI::adjust_size(uint32_t* width, uint32_t* height) {
    return false;
}

bool SimpleSynthUI::set_size(uint32_t width, uint32_t height) {
    return false;
}

bool SimpleSynthUI::set_parent(const clap_window_t* window) {
#ifdef __APPLE__
    if (!created_ || !window) {
        return false;
    }
    
    impl_->parent_view_ = static_cast<NSView*>(window->cocoa);
    if (impl_->parent_view_ && impl_->main_view_) {
        [impl_->parent_view_ addSubview:impl_->main_view_];
        return true;
    }
#endif
    return false;
}

bool SimpleSynthUI::set_transient(const clap_window_t* window) {
    return true;
}

void SimpleSynthUI::suggest_title(const char* title) {
    // Not implemented for embedded view
}

bool SimpleSynthUI::show() {
#ifdef __APPLE__
    if (impl_->main_view_) {
        [impl_->main_view_ setHidden:NO];
        return true;
    }
#endif
    return false;
}

bool SimpleSynthUI::hide() {
#ifdef __APPLE__
    if (impl_->main_view_) {
        [impl_->main_view_ setHidden:YES];
        return true;
    }
#endif
    return false;
}

void SimpleSynthUI::create_controls() {
#ifdef __APPLE__
    if (!impl_->main_view_) return;
    
    // Title
    NSTextField* title = [[NSTextField alloc] initWithFrame:NSMakeRect(20, height_ - 40, 360, 25)];
    [title setStringValue:@"Simple Synth"];
    [title setFont:[NSFont boldSystemFontOfSize:16]];
    [title setTextColor:[NSColor whiteColor]];
    [title setBezeled:NO];
    [title setDrawsBackground:NO];
    [title setEditable:NO];
    [title setSelectable:NO];
    [impl_->main_view_ addSubview:title];
    
    // ADSR Section
    NSTextField* adsr_title = [[NSTextField alloc] initWithFrame:NSMakeRect(20, height_ - 70, 100, 20)];
    [adsr_title setStringValue:@"ENVELOPE"];
    [adsr_title setFont:[NSFont boldSystemFontOfSize:12]];
    [adsr_title setTextColor:[NSColor lightGrayColor]];
    [adsr_title setBezeled:NO];
    [adsr_title setDrawsBackground:NO];
    [adsr_title setEditable:NO];
    [adsr_title setSelectable:NO];
    [impl_->main_view_ addSubview:adsr_title];
    
    // Create ADSR controls
    setup_slider(impl_->attack_slider_, impl_->attack_label_, impl_->attack_value_, "Attack", 0.001, 5.0, 0.01, 20, height_ - 120);
    setup_slider(impl_->decay_slider_, impl_->decay_label_, impl_->decay_value_, "Decay", 0.001, 5.0, 0.1, 120, height_ - 120);
    setup_slider(impl_->sustain_slider_, impl_->sustain_label_, impl_->sustain_value_, "Sustain", 0.0, 1.0, 0.7, 220, height_ - 120);
    setup_slider(impl_->release_slider_, impl_->release_label_, impl_->release_value_, "Release", 0.001, 5.0, 0.3, 320, height_ - 120);
    
    // Filter Section
    NSTextField* filter_title = [[NSTextField alloc] initWithFrame:NSMakeRect(20, height_ - 180, 100, 20)];
    [filter_title setStringValue:@"FILTER"];
    [filter_title setFont:[NSFont boldSystemFontOfSize:12]];
    [filter_title setTextColor:[NSColor lightGrayColor]];
    [filter_title setBezeled:NO];
    [filter_title setDrawsBackground:NO];
    [filter_title setEditable:NO];
    [filter_title setSelectable:NO];
    [impl_->main_view_ addSubview:filter_title];
    
    setup_slider(impl_->filter_cutoff_slider_, impl_->filter_cutoff_label_, impl_->filter_cutoff_value_, "Cutoff", 20.0, 20000.0, 1000.0, 20, height_ - 230);
    
    // Volume Section
    NSTextField* main_title = [[NSTextField alloc] initWithFrame:NSMakeRect(220, height_ - 180, 100, 20)];
    [main_title setStringValue:@"MAIN"];
    [main_title setFont:[NSFont boldSystemFontOfSize:12]];
    [main_title setTextColor:[NSColor lightGrayColor]];
    [main_title setBezeled:NO];
    [main_title setDrawsBackground:NO];
    [main_title setEditable:NO];
    [main_title setSelectable:NO];
    [impl_->main_view_ addSubview:main_title];
    
    setup_slider(impl_->volume_slider_, impl_->volume_label_, impl_->volume_value_, "Volume", 0.0, 1.0, 0.8, 220, height_ - 230);
#endif
}

#ifdef __APPLE__
void SimpleSynthUI::setup_slider(NSSlider* &slider, NSTextField* &label, NSTextField* &value_field,
                                const char* name, double min_val, double max_val, double default_val,
                                int x, int y) {
    // Label
    label = [[NSTextField alloc] initWithFrame:NSMakeRect(x, y + 60, 80, 15)];
    [label setStringValue:[NSString stringWithUTF8String:name]];
    [label setFont:[NSFont systemFontOfSize:10]];
    [label setTextColor:[NSColor lightGrayColor]];
    [label setBezeled:NO];
    [label setDrawsBackground:NO];
    [label setEditable:NO];
    [label setSelectable:NO];
    [impl_->main_view_ addSubview:label];
    
    // Slider
    slider = [[NSSlider alloc] initWithFrame:NSMakeRect(x, y + 20, 80, 20)];
    [slider setMinValue:min_val];
    [slider setMaxValue:max_val];
    [slider setDoubleValue:default_val];
    [slider setSliderType:NSSliderTypeLinear];
    [impl_->main_view_ addSubview:slider];
    
    // Value display
    value_field = [[NSTextField alloc] initWithFrame:NSMakeRect(x, y, 80, 15)];
    [value_field setFont:[NSFont systemFontOfSize:9]];
    [value_field setTextColor:[NSColor whiteColor]];
    [value_field setBezeled:NO];
    [value_field setDrawsBackground:NO];
    [value_field setEditable:NO];
    [value_field setSelectable:NO];
    [value_field setAlignment:NSTextAlignmentCenter];
    [impl_->main_view_ addSubview:value_field];
    
    // Set up target-action
    SliderTarget* target = [[SliderTarget alloc] init];
    target.ui = this;
    
    // Determine parameter ID based on slider
    if (slider == impl_->attack_slider_) target.param_id = 0;
    else if (slider == impl_->decay_slider_) target.param_id = 1;
    else if (slider == impl_->sustain_slider_) target.param_id = 2;
    else if (slider == impl_->release_slider_) target.param_id = 3;
    else if (slider == impl_->volume_slider_) target.param_id = 4;
    else if (slider == impl_->filter_cutoff_slider_) target.param_id = 5;
    
    [slider setTarget:target];
    [slider setAction:@selector(sliderChanged:)];
    
    // Update initial display
    update_parameter_display(target.param_id);
}
#endif

void SimpleSynthUI::update_parameter_display(uint32_t param_id) {
#ifdef __APPLE__
    double value;
    if (!synth_->params_get_value(param_id, &value)) {
        return;
    }
    
    char display[64];
    synth_->params_value_to_text(param_id, value, display, sizeof(display));
    
    NSTextField* value_field = nullptr;
    NSSlider* slider = nullptr;
    
    switch (param_id) {
        case 0: // Attack
            value_field = impl_->attack_value_;
            slider = impl_->attack_slider_;
            break;
        case 1: // Decay
            value_field = impl_->decay_value_;
            slider = impl_->decay_slider_;
            break;
        case 2: // Sustain
            value_field = impl_->sustain_value_;
            slider = impl_->sustain_slider_;
            break;
        case 3: // Release
            value_field = impl_->release_value_;
            slider = impl_->release_slider_;
            break;
        case 4: // Volume
            value_field = impl_->volume_value_;
            slider = impl_->volume_slider_;
            break;
        case 5: // Filter Cutoff
            value_field = impl_->filter_cutoff_value_;
            slider = impl_->filter_cutoff_slider_;
            break;
    }
    
    if (value_field) {
        [value_field setStringValue:[NSString stringWithUTF8String:display]];
    }
    
    if (slider) {
        [slider setDoubleValue:value];
    }
#endif
}

void SimpleSynthUI::on_parameter_changed(uint32_t param_id, double value) {
    // Send parameter change to host
    if (host_ && host_->request_callback) {
        host_->request_callback(host_);
    }
    
    // Update display
    update_parameter_display(param_id);
    
    // This would normally send a parameter change event to the plugin
    // For now, we'll directly update the synth (not ideal for real plugins)
    // In a proper implementation, this should go through the host's parameter system
}