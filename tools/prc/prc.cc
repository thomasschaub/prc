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
    //SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

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

        // Update view
        view.clear();
        int frameStart = now(nullptr);
        //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Time: %d\n", frameStart);
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
                    note.end = 0;
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

        view.update();

        auto elapsed = now(nullptr) - frameStart;
        auto delay = 1000 / 60 - elapsed;
        if (delay > 0) {
            SDL_Delay(delay);
        }
    }

    Pm_Close(stream);
    Pm_Terminate();
}
