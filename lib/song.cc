#include "song.h"

#include <cstring>

#include <portSMF/allegro.h>

std::vector<Note> loadSong(const char* path) {
    std::vector<Note> notes;

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
                            static_cast<int>(event->get_loud()),
                            static_cast<float>(event->get_start_time()),
                            static_cast<float>(event->get_end_time())
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
