#include <array>
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "note.h"
#include "midi.h"
#include "song.h"
#include "timing.h"
#include "view.h"

void printMidiDevices() {
    int n = Pm_CountDevices();
    for (int i = 0; i < n; ++i) {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
        std::cout << i << ": " << info->name;
        if (info->input) {
            std::cout << " (input)";
        }
        std::cout << std::endl;
    }
}

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
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <midi>" << std::endl;
        return 1;
    }

    const char* songPath = argv[1];
    PmDeviceID inputDevice = 5;
    PmDeviceID outputDevice = 6;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

    // Prepare user input
    std::array<Note, 128> activeNotes;
    for (unsigned char i = 0; i < 128; ++i) {
        Note& note = activeNotes[i];
        note = {0};
        note.note = i;
    }
    std::vector<Note> playedNotes;

    // Open user input
    Pm_Initialize();
    printMidiDevices();
    PmStream* stream = getInputStream(inputDevice);

    // Open song
    std::vector<Note> song = loadSong(songPath);

    // Open output
    PmStream* outputStream = getOutputStream(outputDevice);

    // Create output
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);
    View view(renderer);

    // Move song a few seconds into the future and find min/max pitch
    int minPitch = 128;
    int maxPitch = 0;
    auto songOffset = beatTime() + 4;
    for (auto& note: song) {
        note.start += songOffset;
        note.end += songOffset;

        minPitch = std::min(minPitch, note.note);
        maxPitch = std::max(maxPitch, note.note);
    }
    minPitch = (minPitch - 12) / 12 * 12;
    maxPitch = (maxPitch + 24) / 12 * 12 - 1;
    view.minPitch = std::max(0, minPitch);
    view.maxPitch = std::min(127, maxPitch);

    bool run = true;
    while (run) {
        // Update time
        auto lastBeatTime = beatTime();
        updateBeatTime(1);
        int frameStart = wallTime(nullptr);

        // Check SDL events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_0:
                        resetBeatTime();
                        playedNotes.clear();
                        putAllOff(outputStream, 0);
                        putAllOff(outputStream, 1);
                        break;
                }
                break;
            case SDL_QUIT:
                run = false;
                break;
            }
        }

        // Prepare view
        view.background();
        view.setTime(beatTime());

        // Process song
        SDL_SetRenderDrawColor(renderer, 255, 127, 0, 255);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (const auto& note: song) {
            if (lastBeatTime < note.start && note.start <= beatTime()) {
                NoteEvent e {
                    1,
                    static_cast<unsigned char>(note.note),
                    127,
                    ON
                };
                putNoteEvent(outputStream, e);
            }
            else if (lastBeatTime < note.end && note.end <= beatTime()) {
                NoteEvent e {
                    1,
                    static_cast<unsigned char>(note.note),
                    0,
                    OFF
                };
                putNoteEvent(outputStream, e);
            }
            view.draw(note);
        }

        // Read and replay piano input
        NoteEvent noteEvents[8];
        int n = getNoteEvent(stream, noteEvents, 8);
        for (int i = 0; i < n; ++i) {
            putNoteEvent(outputStream, noteEvents[i]);
            const NoteEvent& e = noteEvents[i];
            Note& note = activeNotes[e.pitch];
            switch (e.type) {
                case ON:
                    note.start = e.time;
                    note.end = 0;
                    break;
                case OFF:
                    note.end = e.time;
                    playedNotes.push_back(activeNotes[e.pitch]);

                    note.start = 0;
                    break;
            }
        }

        // Draw piano input
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(renderer, 255, 128, 255, 255);
        for (auto& note: activeNotes) {
            if (note.start != 0) {
                view.draw(note);
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        for (auto& note: playedNotes) {
            view.draw(note);
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

    Pm_Close(stream);
    Pm_Close(outputStream);
    Pm_Terminate();
}
