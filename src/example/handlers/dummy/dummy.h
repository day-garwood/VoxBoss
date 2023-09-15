#ifndef _vb_dummy_handler_h
#define _vb_dummy_handler_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vb.h"

typedef struct
{
clock_t clock;
int time;
}
_vb_dummy_handler;

int _vb_dummy_initialise(vb_handler* handler);
int _vb_dummy_speak(vb_handler* handler, char* text, int interrupt);
int _vb_dummy_stop(vb_handler* handler);
int _vb_dummy_is_speaking(vb_handler* handler);
void _vb_dummy_cleanup(vb_handler* handler);
vb_result _vb_dummy_handler_register(vb_speaker* voice);

#endif
