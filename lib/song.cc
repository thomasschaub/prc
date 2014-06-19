#include "song.h"

#include <cstring>

#include <portSMF/allegro.h>

std::vector<Note> loadSong(const char* path) {
    std::vector<Note> notes;

    // A quarter note is 1 time unit, a 1/64 note is 1/16 time units
    // -> we multiply by 16 to get a resolution of 1/64ths
    const unsigned TIME_MULTI = 16;

    Alg_seq seq(path, true);

    Alg_tracks& tracks = seq.track_list;
    for (int i = 0; i < tracks.length(); ++i) {
        Alg_track& track = tracks[i];
        for (int j = 0; j < track.length(); ++j) {
            Alg_event_ptr event = track[j];
            switch (event->get_type()) {
                case 'n':
                    {
                        Note n {
                            static_cast<int>(event->get_identifier()),
                            static_cast<unsigned>(TIME_MULTI * event->get_start_time()),
                            static_cast<unsigned>(TIME_MULTI * event->get_end_time())
                        };
                        notes.push_back(n);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    return notes;
}
