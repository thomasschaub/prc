#ifndef SONG_H__
#define SONG_H__

#include <vector>

#include "note.h"

std::vector<Note> loadSong(const char* path);

#endif
