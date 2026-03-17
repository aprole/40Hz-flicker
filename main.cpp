#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
#include <cstdlib>  // For atoi
#include <vector>
#include <algorithm>

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

void fillAudioBuffer(void* /*userdata*/, Uint8* stream, int len) {
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

int main(int argc, char* argv[]) {
    // Parse command line arguments for session duration
    int sessionDurationMinutes = 1; // Default to 1 minute (shorter for testing)
    bool useAllMonitors = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--duration") == 0 || strcmp(argv[i], "-d") == 0) {
            if (i + 1 < argc) {
                sessionDurationMinutes = atoi(argv[i + 1]);
                if (sessionDurationMinutes <= 0) {
                    std::cout << "Warning: Invalid duration, using default of 1 minute" << std::endl;
                    sessionDurationMinutes = 1;
                }
                i++; // Skip the next argument as it was consumed
            } else {
                std::cout << "Warning: --duration flag requires a value. Using default of 1 minute." << std::endl;
            }
        }
        else if (strcmp(argv[i], "--all-monitors") == 0 || strcmp(argv[i], "-a") == 0) {
            useAllMonitors = true;
        }
    }

    // Initialize SDL with video and audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Get number of displays
    int displayCount = SDL_GetNumVideoDisplays();
    if (displayCount <= 0) {
        std::cerr << "Failed to get display count" << std::endl;
        SDL_Quit();
        return -1;
    }

    int numDisplays = useAllMonitors ? std::min(2, displayCount) : 1;
    std::cout << "Using " << numDisplays << " display(s): ";
    for (int j = 0; j < numDisplays; ++j) {
        std::cout << j;
        if (j < numDisplays - 1) std::cout << ",";
    }
    std::cout << std::endl;

    // Create windows for all available displays
    std::vector<SDL_Window*> windows(numDisplays);
    std::vector<SDL_Renderer*> renderers(numDisplays);
    
    for (int i = 0; i < numDisplays; ++i) {
        SDL_Rect bounds;
        if (SDL_GetDisplayBounds(i, &bounds) < 0) {
            std::cerr << "Failed to get bounds for display " << i << ": " << SDL_GetError() << std::endl;
            // Clean up already created windows and quit
            for (int j = 0; j < i; ++j) {
                SDL_DestroyRenderer(renderers[j]);
                SDL_DestroyWindow(windows[j]);
            }
            SDL_Quit();
            return -1;
        }

        // Create window on each display
        windows[i] = SDL_CreateWindow(
            "40Hz Flicker with Audio",
            bounds.x, bounds.y,  // Use bounds for position
            bounds.w, bounds.h,  // Use bounds for size
            SDL_WINDOW_FULLSCREEN_DESKTOP  // Changed from FULLSCREEN
        );

        if (!windows[i]) {
            std::cerr << "Failed to create window on display " << i << ": " << SDL_GetError() << std::endl;
            // Clean up already created windows and quit
            for (int j = 0; j < i; ++j) {
                SDL_DestroyRenderer(renderers[j]);
                SDL_DestroyWindow(windows[j]);
            }
            SDL_Quit();
            return -1;
        }

        // Create renderer for each window with hardware accelerated rendering and vsync
        renderers[i] = SDL_CreateRenderer(windows[i], -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderers[i]) {
            std::cerr << "Failed to create renderer on display " << i << ": " << SDL_GetError() << std::endl;
            // Clean up windows and quit
            for (int j = 0; j <= i; ++j) {
                SDL_DestroyRenderer(renderers[j]);
                SDL_DestroyWindow(windows[j]);
            }
            SDL_Quit();
            return -1;
        }
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
    Uint64 sessionStart = SDL_GetPerformanceCounter(); // For session timer
    bool flickerOn = true;
    bool running = true;

    // Print startup info - removed session time lines that cause constant printing
    std::cout << "Starting 40Hz Flicker Application with Audio" << std::endl;
    std::cout << "Mode: " << (useAllMonitors ? "All monitors" : "Primary monitor only") << std::endl;
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
        double sessionElapsedSeconds = ((double)(currentTime - sessionStart) * 1.0) / (double)SDL_GetPerformanceFrequency();

        // Check if session has reached maximum duration
        if (sessionElapsedSeconds >= sessionDurationMinutes * 60.0) {
            running = false;
        }

        // Switch flicker state every half cycle (12ms)
        if (elapsedMs >= HALF_CYCLE_MS) {
            flickerOn = !flickerOn;
            startTime = currentTime; // Reset timer
        }

        // Update all displays with the same flicker pattern
        for (int i = 0; i < numDisplays; ++i) {
            // Set render color based on flicker state and clear screen
            SDL_SetRenderDrawColor(renderers[i],
                                  flickerOn ? 255 : 0,  // Red channel (white or black)
                                  flickerOn ? 255 : 0,  // Green channel (white or black)
                                  flickerOn ? 255 : 0,  // Blue channel (white or black)
                                  255);                  // Alpha channel
            SDL_RenderClear(renderers[i]);

            // Present the rendered frame to screen
            SDL_RenderPresent(renderers[i]);
        }

        // Small delay to prevent excessive CPU usage while maintaining timing precision
        SDL_Delay(1);
    }

    // Clean up resources
    if (audio_device) {
        SDL_CloseAudioDevice(audio_device);
    }
    for (int i = numDisplays - 1; i >= 0; --i) {
        SDL_DestroyRenderer(renderers[i]);
        SDL_DestroyWindow(windows[i]);
    }
    
    SDL_Quit();
    
    std::cout << "40Hz Flicker Application Exited" << std::endl;
    return 0;
}
