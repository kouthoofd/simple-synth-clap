# Simple Synth CLAP

A polyphonic synthesizer plugin in CLAP format for macOS, featuring multiple waveforms and ADSR envelope control.

![Simple Synth CLAP](https://img.shields.io/badge/CLAP-Plugin-blue) ![macOS](https://img.shields.io/badge/macOS-Compatible-green) ![C++17](https://img.shields.io/badge/C%2B%2B-17-orange)

## Features

- **5 Waveforms**: Sine, Square, Saw, Triangle, Pulse
- **ADSR Envelope**: Full Attack, Decay, Sustain, Release control
- **16-Voice Polyphony** with intelligent voice management
- **Real-time Parameter Automation**
- **MIDI Input Support**
- **Native macOS Bundle** (.clap format)

## Parameters

### Envelope
- **Attack** (0.001s - 5s) - Note fade-in time
- **Decay** (0.001s - 5s) - Time to reach sustain level
- **Sustain** (0% - 100%) - Held note level
- **Release** (0.001s - 5s) - Note fade-out time

### Oscillator
- **Waveform** - Choose from 5 different waveforms:
  - **Sine** - Pure, smooth tone
  - **Square** - Hollow, woody sound
  - **Saw** - Bright, buzzy sound
  - **Triangle** - Softer than square, warmer than sine
  - **Pulse** - Narrow pulse wave (25% duty cycle)

### Main
- **Volume** (0% - 100%) - Overall output level

## Requirements

- macOS 10.15 or later
- CLAP-compatible DAW (Logic Pro, Reaper, Bitwig Studio, etc.)
- Xcode Command Line Tools

## Building from Source

### Prerequisites

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Clone the repository
git clone https://github.com/yourusername/simple-synth-clap.git
cd simple-synth-clap
```

### Build

```bash
# Make build script executable
chmod +x build.sh

# Build the plugin
./build.sh
```

### Install

```bash
# Make install script executable
chmod +x install.sh

# Install to system plugin directory
./install.sh
```

Or manually copy the built plugin:
```bash
cp -R build/SimpleSynthCLAP.clap ~/Library/Audio/Plug-Ins/CLAP/
```

## Usage

1. **Load the plugin** in your CLAP-compatible DAW
2. **Play MIDI notes** to trigger the synthesizer
3. **Adjust parameters** in real-time:
   - Change waveforms for different tonal characteristics
   - Modify ADSR envelope for different note behaviors
   - Adjust volume for mix balance
4. **Automate parameters** for dynamic performances

## Technical Details

- **Format**: CLAP (CLever Audio Plugin)
- **Polyphony**: 16 voices with round-robin voice stealing
- **Sample Rate**: All standard rates supported
- **Bit Depth**: 32-bit float internal processing
- **Latency**: Zero latency
- **Voice Management**: Intelligent allocation with anti-hanging protection

## Project Structure

```
simple-synth-clap/
├── src/
│   ├── simple_synth.h      # Main synthesizer class
│   ├── simple_synth.cpp    # Synthesizer implementation
│   ├── voice.h             # Voice class definition
│   ├── voice.cpp           # Voice implementation with waveforms
│   └── plugin.cpp          # CLAP plugin interface
├── build.sh                # Build script
├── install.sh              # Installation script
├── Makefile                # Build configuration
├── Info.plist             # macOS bundle info
└── README.md               # This file
```

## Development

### Architecture

- **SimpleSynth**: Main plugin class handling CLAP interface
- **Voice**: Individual voice with oscillator and envelope
- **Plugin Interface**: CLAP entry point and factory

### Key Components

1. **CLAP Integration**: Full CLAP specification compliance
2. **Voice Management**: Polyphonic voice allocation and cleanup
3. **Waveform Generation**: Multiple oscillator types
4. **Envelope Processing**: ADSR envelope with proper state management
5. **MIDI Handling**: Note on/off and parameter automation

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is open source and available under the [MIT License](LICENSE).

## Acknowledgments

- Built using the [CLAP SDK](https://github.com/free-audio/clap)
- Developed as part of the polarity-music-tools collection
- Thanks to the CLAP community for the excellent plugin format

## Support

- **Issues**: Report bugs and feature requests on GitHub Issues
- **Discussions**: Join conversations in GitHub Discussions
- **Documentation**: Check the wiki for detailed documentation

---

**Made with ❤️ for the music production community**