#ifndef NOTE_H__
#define NOTE_H__

struct Note {
    Note();

    void reset();

    int pitch, velocity;
    float start, end;
};

#endif
