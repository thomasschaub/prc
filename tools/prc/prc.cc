#include <array>
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "cli.h"
#include "note.h"
#include "midi.h"
#include "song.h"
#include "timing.h"
#include "view.h"

enum PlaybackState {
    PLAY,
    PAUSE,
    SEEK_L,
    SEEK_R
};

PmStream* getInputStream(PmDeviceID id) {
    const PmDeviceInfo* info = Pm_GetDeviceInfo(id);
    std::cout << info->name << " " << info->input << std::endl;
    PmStream* stream;
    PmError err = Pm_OpenInput(&stream, id, nullptr, 8, wallTime, nullptr);
    if (err != pmNoError) {
        std::cerr << "Error opening input device" << std::endl;
    }
    return stream;
}

PmStream* getOutputStream(PmDeviceID id) {
    PmStream* stream;
    PmError err = Pm_OpenOutput(&stream, id, nullptr, 8, wallTime, nullptr, 0);
    if (err != pmNoError) {
        std::cerr << "Error opening ouput device" << std::endl;
    }
    return stream;
}

int main(int argc, const char* argv[]) {
    // Read CLI args
    CliArgs args = readCliArgs(argc, argv);
    if (args.songPath == nullptr) {
        std::cerr << "Usage: " << argv[0] << " <midi>" << std::endl;
        return 1;
    }

    Pm_Initialize();
    listDevices();

    // Open Piano Input
    PmStream* pianoStream = nullptr;
    if (args.inputDevice != -1) {
        pianoStream = getInputStream(args.inputDevice);
    }
    while (pianoStream == nullptr) {
        if (args.inputDevice != -1) {
            std::cerr << "Could not open input device" << std::endl;
        }

        std::cout << "Enter input device ID: " << std::flush;
        std::cin >> args.inputDevice;
        pianoStream = getInputStream(args.inputDevice);
    }

    // Open output
    PmStream* outputStream = nullptr;
    if (args.outputDevice != -1) {
        outputStream = getOutputStream(args.outputDevice);
    }
    while (outputStream == nullptr) {
        if (args.outputDevice != -1) {
            std::cerr << "Could not open output device" << std::endl;
        }

        std::cout << "Enter output device ID: " << std::flush;
        std::cin >> args.outputDevice;
        outputStream = getOutputStream(args.outputDevice);
    }

    // Init SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

    // Prepare user input
    std::array<Note, 128> activeNotes;
    for (unsigned char i = 0; i < 128; ++i) {
        activeNotes[i].pitch = i;
    }
    std::vector<Note> playedNotes;

    // Open song
    std::vector<Note> song = loadSong(args.songPath);
    setBpm(96);

    // Maintain expected notes for training mode
    std::array<bool, 128> expectedNotes;
    for (int i = 0; i < 128; ++i) {
        expectedNotes[i] = false;
    }

    // Create output
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint32 windowFlags = 0;
    if (args.fullscreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    SDL_CreateWindowAndRenderer(800, 600, windowFlags, &window, &renderer);
    View view(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Move song a few seconds into the future and find min/max pitch
    int minPitch = 128;
    int maxPitch = 0;
    auto songOffset = beatTime() + 4;
    for (auto& note: song) {
        note.start += songOffset;
        note.end += songOffset;

        minPitch = std::min(minPitch, note.pitch);
        maxPitch = std::max(maxPitch, note.pitch);
    }
    minPitch = (minPitch - 12) / 12 * 12;
    maxPitch = (maxPitch + 24) / 12 * 12 - 1;
    view.minPitch = std::max(0, minPitch);
    view.maxPitch = std::min(127, maxPitch);

    bool run = true;
    PlaybackState playbackState = PLAY;

    while (run) {
        // Check SDL events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                case SDLK_0:
                    resetBeatTime();
                    playedNotes.clear();
                    for (int i = 0; i < 128; ++i) {
                        expectedNotes[i] = false;
                    }
                    putAllOff(outputStream, 0);
                    putAllOff(outputStream, 1);
                    break;
                case SDLK_SPACE:
                    if (playbackState == PAUSE) {
                        playbackState = PLAY;
                    }
                    else {
                        playbackState = PAUSE;
                        putAllOff(outputStream, 1); // only mute song
                    }
                    break;
                case SDLK_LEFT:
                    playbackState = SEEK_L;
                    break;
                case SDLK_RIGHT:
                    playbackState = SEEK_R;
                    break;
                case SDLK_q:
                    run = false;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    playbackState = PLAY;
                    break;
                }
                break;
            case SDL_QUIT:
                run = false;
                break;
            }
        }

        // Play/pause/seek
        float playbackSpeed = 1;
        switch (playbackState) {
        case PLAY:
            break;
        case PAUSE:
            playbackSpeed = 0;
            break;
        case SEEK_L:
            playbackSpeed = -16;
            playedNotes.clear();
            break;
        case SEEK_R:
            playbackSpeed = 16;
            break;
        }

        bool allCorrect = true;
        // If training is enabled, only forward time when the right notes are played
        for (int i = 0; i < 128; ++i) {
            bool active = activeNotes[i].start != -1;
            bool expected = expectedNotes[i];

            if (!active && expected) {
                allCorrect = false;
                break;
            }
        }

        // Update time
        auto lastBeatTime = beatTime();
        if (args.mode == Mode_Training && playbackState == PLAY && !allCorrect) {
            updateBeatTime(0);
        }
        else {
            updateBeatTime(playbackSpeed);
        }
        int frameStart = wallTime(nullptr);


        // Prepare view
        view.background();
        view.setTime(beatTime());

        // Process song
        SDL_SetRenderDrawColor(renderer, 255, 127, 0, 255);
        for (const auto& note: song) {
            // TODO send the inverse signal if we seek backwards
            bool on = (lastBeatTime < note.start && note.start <= beatTime())
                || ((beatTime() < note.end && note.end <= lastBeatTime));
            bool off = (lastBeatTime < note.end && note.end <= beatTime())
                || (beatTime() < note.start && note.start <= lastBeatTime);
            if (on) {
                NoteEvent e {
                    1,
                    static_cast<unsigned char>(note.pitch),
                    static_cast<unsigned char>(note.velocity),
                    ON
                };
                putNoteEvent(outputStream, e);

                expectedNotes[note.pitch] = true;
            }
            if (off) {
                NoteEvent e {
                    1,
                    static_cast<unsigned char>(note.pitch),
                    static_cast<unsigned char>(note.velocity),
                    OFF
                };
                putNoteEvent(outputStream, e);

                expectedNotes[note.pitch] = false;
            }
            view.drawHollow(note);
        }

        // Draw piano input
        for (auto& note: activeNotes) {
            if (note.start != -1) {
                SDL_SetRenderDrawColor(renderer, 30, 158, 255, 180);
                view.line(note);
                SDL_SetRenderDrawColor(renderer, 30, 158, 255, 255);
                view.draw(note);
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        for (auto& note: playedNotes) {
            view.draw(note);
        }

        // Read and replay piano input
        NoteEvent noteEvents[8];
        int n = getNoteEvent(pianoStream, noteEvents, 8);
        for (int i = 0; i < n; ++i) {
            putNoteEvent(outputStream, noteEvents[i]);
            const NoteEvent& e = noteEvents[i];
            Note& note = activeNotes[e.pitch];
            switch (e.type) {
                case ON:
                    note.start = e.time;
                    break;
                case OFF:
                    note.end = e.time;
                    playedNotes.push_back(activeNotes[e.pitch]);
                    note.reset();
                    break;
            }
        }

        view.finish();

        // Limit frame rate
        auto elapsed = wallTime(nullptr) - frameStart;
        auto delay = 1000 / 60 - elapsed;
        if (delay > 0) {
            SDL_Delay(delay);
        }
    }

    putAllOff(outputStream, 0);
    putAllOff(outputStream, 1);

    Pm_Close(pianoStream);
    Pm_Close(outputStream);
    Pm_Terminate();
}
