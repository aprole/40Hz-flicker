#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

// Timing constants for 40Hz flicker (25ms per cycle, 12.5ms on/off)
const float FREQUENCY_HZ = 40.0f;
const Uint64 CYCLE_TIME_MS = 25; // Full cycle time in milliseconds
const Uint64 HALF_CYCLE_MS = 12; // Half cycle time for flicker

// Audio parameters
const int SAMPLE_RATE = 44100;  // Standard audio sample rate
const float AUDIO_FREQUENCY = 40.0f;  // 40Hz tone frequency (matches flicker)
const float CARRIER_FREQUENCY = 10000.0f; // High-frequency carrier for clicks

// Global variables for audio callback
Uint64 globalSamplePosition = 0;
float amplitude = 0.4f;

void fillAudioBuffer(void* userdata, Uint8* stream, int len) {
    float* buffer = (float*)stream;
    int samples = len / sizeof(float);
    
    // Calculate the period in samples for 40Hz (25ms)
    const int PERIOD_SAMPLES = (int)(SAMPLE_RATE / AUDIO_FREQUENCY); 
    
    // For each sample in the buffer
    for (int i = 0; i < samples; ++i) {
        // Check if we're within a click burst period (1ms at 40Hz = 44 samples at 44.1kHz)
        int sampleInPeriod = globalSamplePosition % PERIOD_SAMPLES;
        
        // Click duration: ~1ms (44 samples)
        if (sampleInPeriod < 44) {
            // Generate a high-frequency sine wave for the click
            double phase = (double)(sampleInPeriod) * 2.0 * M_PI * CARRIER_FREQUENCY / SAMPLE_RATE;
            buffer[i] = amplitude * sin(phase);
        } else {
            // Silence outside of click burst
            buffer[i] = 0.0f;
        }
        
        globalSamplePosition++;
    }
}

int main() {
    // Initialize SDL with video and audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Get display information for fullscreen
    SDL_DisplayMode displayMode;
    SDL_GetDesktopDisplayMode(0, &displayMode);
    
    // Create window (fullscreen)
    SDL_Window* window = SDL_CreateWindow(
        "40Hz Flicker with Audio",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        displayMode.w, displayMode.h,
        SDL_WINDOW_FULLSCREEN
    );
    
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    
    // Create renderer for hardware accelerated rendering with vsync
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Audio initialization
    SDL_AudioSpec wanted, obtained;
    SDL_AudioDeviceID audio_device;

    // Setup audio spec for 40Hz click generation at 44.1kHz sample rate
    wanted.freq = SAMPLE_RATE;
    wanted.format = AUDIO_F32SYS;  // Using float format
    wanted.channels = 1;           // Mono
    wanted.samples = 512;          // Audio buffer size (small for low latency)
    wanted.callback = fillAudioBuffer;     // Use callback instead of manual queuing
    
    audio_device = SDL_OpenAudioDevice(NULL, 0, &wanted, &obtained, 0);
    
    if (audio_device == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        // Continue without audio - still have visual flicker
        audio_device = 0;  // Flag that audio failed to initialize
    } else {
        // Play audio (callback will handle playback)
        SDL_PauseAudioDevice(audio_device, 0);
    }

    // Timing variables
    Uint64 startTime = SDL_GetPerformanceCounter();
    bool flickerOn = true;
    bool running = true;

    // Print startup info
    std::cout << "Starting 40Hz Flicker Application with Audio" << std::endl;
    std::cout << "Press ESC to quit" << std::endl;
    std::cout << "Flicker frequency: " << FREQUENCY_HZ << " Hz" << std::endl;
    std::cout << "Period: " << CYCLE_TIME_MS << " ms" << std::endl;

    // Main loop
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
                (event.key.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
        }

        // Get current time for precise timing
        Uint64 currentTime = SDL_GetPerformanceCounter();
        double elapsedMs = ((double)(currentTime - startTime) * 1000.0) / (double)SDL_GetPerformanceFrequency();

        // Switch flicker state every half cycle (12ms)
        if (elapsedMs >= HALF_CYCLE_MS) {
            flickerOn = !flickerOn;
            startTime = currentTime; // Reset timer
        }

        // Set render color based on flicker state and clear screen
        SDL_SetRenderDrawColor(renderer, 
                              flickerOn ? 255 : 0,  // Red channel (white or black)
                              flickerOn ? 255 : 0,  // Green channel (white or black)
                              flickerOn ? 255 : 0,  // Blue channel (white or black)
                              255);                  // Alpha channel
        SDL_RenderClear(renderer);

        // Present the rendered frame to screen
        SDL_RenderPresent(renderer);
        
        // Small delay to prevent excessive CPU usage while maintaining timing precision
        SDL_Delay(1);
    }

    // Clean up resources
    if (audio_device) {
        SDL_CloseAudioDevice(audio_device);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    std::cout << "40Hz Flicker Application Exited" << std::endl;
    return 0;
}
