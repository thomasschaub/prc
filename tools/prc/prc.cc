#include <array>
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "note.h"
#include "midi.h"
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
    PmError err = Pm_OpenInput(&stream, id, nullptr, 8, now, nullptr);
    if (err != pmNoError) {
        std::cerr << "Error opening input device" << std::endl;
    }
    return stream;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

    std::array<Note, 128> activeNotes;
    for (auto& note: activeNotes) {
        note = {0};
    }
    std::vector<Note> playedNotes;

    // Open user input
    Pm_Initialize();
    printMidiDevices();
    PmStream* stream = getInputStream(5);

    // Create output
    View view;

    bool run = true;
    while (run) {
        // Check SDL events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                run = false;
                break;
            }
        }

        // Update view time
        int frameStart = now(nullptr);
        view.setTime(frameStart);

        // Read song
        // TODO

        // Draw song
        // TODO

        // Read user input
        NoteEvent noteEvents[8];
        int n = getNoteEvent(stream, noteEvents, 8);
        for (int i = 0; i < n; ++i) {
            const NoteEvent& e = noteEvents[i];
            Note& note = activeNotes[e.note];
            switch (e.type) {
                case ON:
                    note.start = e.time;
                    break;
                case OFF:
                    note.end = e.time;
                    playedNotes.push_back(activeNotes[e.note]);

                    note.start = 0;
                    break;
            }
        }

        // Draw user input
        for (auto& note: activeNotes) {
            if (note.start != 0) {
                view.draw(note);
            }
        }
        for (auto& note: playedNotes) {
            view.draw(note);
        }

        auto elapsed = now(nullptr) - frameStart;
        auto delay = 1000 / 60 - elapsed;
        if (delay > 0) {
            SDL_Delay(delay);
        }


    }

    Pm_Close(stream);
    Pm_Terminate();
}
