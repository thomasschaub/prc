#ifndef TIME_H__
#define TIME_H__

void setBpm(float bpm);

/**
 * Returns the current "time" in beats
 */
extern float beatTime();

/**
 * Computes the dt since the last update and sets beat time accordingly
 *
 * Use speed = -1 to rewind.
 */
void updateBeatTime(float speed);

/**
 * Current time in ms.
 *
 * Takes a void* so it can be used with PortMidi
 */
int wallTime(void*);

#endif
