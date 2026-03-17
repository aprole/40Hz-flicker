# 40 Hz GENUS Flicker – Visual + Audio Stimulation App

A simple, fullscreen SDL2 application delivering **40 Hz visual flicker** (white/black full-screen alternation) + synchronized **40 Hz audio clicks**, inspired by **Gamma Entrainment Using Sensory stimuli (GENUS)** research for potential benefits in Alzheimer's disease.

> ⚠️ **This is experimental, DIY code — not medical software.**

## Important Disclaimers (March 2026)

### Research Status
Recent 2025–2026 studies (MIT Picower Institute, open-label extensions, reviews in *Frontiers in Aging Neuroscience* and *Alzheimer's & Dementia*) indicate daily 40 Hz audiovisual stimulation is **safe**, **well-tolerated**, and may slow cognitive decline, reduce tau/amyloid biomarkers, and preserve function — especially in late-onset mild Alzheimer's over 1–2+ years. Benefits appear stronger with combined visual + auditory stimulation than light alone. (Small long-term follow-ups; largest effects in late-onset cases.)

### Not a Medical Device
This program is **research-inspired only** — **not** FDA-cleared, not a substitute for approved therapies or Cognito Therapeutics' Spectris™ (formerly Evie Light) device. Cognito raised $105M in early 2026 to advance pivotal trials and pursue FDA clearance (HOPE study topline data expected 2026, potential commercialization ~2027).

### Recommended Study Protocol
- **Duration**: ~60 minutes daily, eyes open  
- **Positioning**: Seated 30–60 cm from a bright screen in a dimly lit room  
- **Start**: Begin with 10–15 min sessions to test tolerance  

### Safety Warnings
- ⚠️ **Do NOT use** if you have photosensitive epilepsy, severe migraines, retinal disorders, or recent eye surgery  
- Harsh white/black flicker can cause discomfort in sensitive individuals — consider softer modes (gray/white or lower brightness)  
- ⚠️ **Stop immediately** if you experience dizziness, nausea, headache, eye strain, or visual disturbances  
- Always consult a neurologist or physician before regular use, especially for Alzheimer's or MCI patients

## Background & Research Summary

Since ~2016, MIT's Li-Huei Tsai lab (Picower Institute) and others have studied 40 Hz sensory stimulation (GENUS) to entrain gamma brain rhythms (~400 Hz).

### Key Findings
- **Mouse Models**: Reduced amyloid-β plaques, tau pathology, neurodegeneration, synapse loss; improved memory/cognition  
- **Human Studies** (2022–2025/2026): Safe/feasible in mild–moderate AD/MCI; trends toward slower cognitive decline, reduced brain atrophy, lower tau biomarkers, better sleep/memory (strongest in late-onset AD over ~202 years)

### Caveat
Still experimental. No large Phase 3 double-blind placebo-controlled results yet; no regulatory approval for Alzheimer's treatment.

### Main Sources
- MIT Picower Institute (2025 long-term follow-up in *Alzheimer's & Dementia*)  
- Reviews in *Frontiers in Aging Neuroscience* (2025/2026)  
- ClinicalTrials.gov (e.g., Cognito-related entries)

## Features

- Full-screen **40 Hz square-wave flicker** (~12.5 ms white ↔ 12.5 ms black)  
- Synchronized mono audio: ~1 ms 10 kHz carrier "click" bursts every 25 ms (40 Hz)  
- Hardware-accelerated SDL2 rendering with vsync for stable timing  
- Press **ESC** to exit  
- Runs indefinitely (add session timer in future versions)

## Requirements

- C++17 compiler (g++, clang, MSVC, etc.)  
- SDL2 development libraries  
  - Ubuntu/Debian: `sudo apt install libsdl2-dev`  
  - macOS (Homebrew): `brew install sdl2`  
  - Windows: vcpkg, MSYS2, or prebuilt libs  
- Optional: CMake for easy builds

## Build & Run

To build and run this application, you can use the following Make commands:

- `make all` - Build the project
- `make clean` - Clean compiled files  
- `make run` - Build and run the project
- `make install-deps` - Install required dependencies (SDL2)

The project requires SDL2 development libraries. On macOS with Homebrew, you can install them using:
```bash
make install-deps
```

On Ubuntu/Debian systems, use:
```bash
sudo apt install libsdl2-dev
```

After installing dependencies, build and run the application with:
```bash
make run
```

### Usage Examples

- `make run` - Default: Flicker on primary monitor only, 1 minute session
- `./40hz_genus --all-monitors` - Flicker on all available monitors (up to 2)
- `./40hz_genus --duration 60 --all-monitors` - 60 minute session on all monitors
- `./40hz_genus --duration 30` - 30 minute session on primary monitor only

To check your display setup, run:
```bash
make display_test && ./display_test
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

> 📝 *This software is provided "as is" without warranty of any kind.*o