#include <cstring>
#include <fstream>
#include <iostream>

#include <portSMF/allegro.h>

size_t readFile(const char* path, char** contents) {
    std::ifstream f(path);
    if (!f) {
        abort();
    }
    f.seekg(0, f.end);
    size_t size = f.tellg();
    *contents = new char[size + 1];
    f.seekg(0, f.beg);
    f.read(*contents, size);
    (*contents)[size] = '\0';
    return size;
}

int main(int argc, char* argv[]) {
    // Iterate all files (passed in as arguments)
    for (int i = 1; i < argc; ++i) {
        Alg_seq seq(argv[i], true);
        Alg_tracks& tracks = seq.track_list;
        for (int i = 0; i < tracks.length(); ++i) {
            Alg_track& track = tracks[i];
            for (int j = 0; j < track.length(); ++j) {
                Alg_event_ptr event = track[j];
                switch (event->get_type()) {
                case 'n':
                    std::cout << event->get_identifier() << " from " << event->get_start_time()<< std::endl;
                    break;
                default:
                    break;
                }
            }
        }
    }
}
