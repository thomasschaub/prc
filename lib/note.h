#ifndef NOTE_H__
#define NOTE_H__

struct Note {
    bool operator<(const Note& rhs) const;

    void reset();

    int pitch = 0, velocity = 0;
    float start = -1, end = -1;
};

#endif
