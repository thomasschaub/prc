#include "song.h"

#include <algorithm>
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
                        Note n;
                        n.pitch = event->get_identifier();
                        n.velocity = event->get_loud();
                        n.start = event->get_start_time();
                        n.end = event->get_end_time();
                        notes.push_back(n);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    std::sort(notes.begin(), notes.end());
    return notes;
}
