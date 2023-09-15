#include "dummy.h"

int _vb_dummy_initialise(vb_handler* handler)
{
if(!handler) return 0;
_vb_dummy_handler* data=malloc(sizeof(_vb_dummy_handler));
if(!data) return 0;
handler->data=data;
return 1;
}
int _vb_dummy_speak(vb_handler* handler, char* text, int interrupt)
{
if(!handler) return 0;
if(!text) return 0;
if(text[0]==0) return 0;
if((interrupt<0)||(interrupt>1)) return 0;
if(!interrupt) return 0;
_vb_dummy_handler* data=handler->data;
if(!data) return 0;
data->time=(strlen(text)*50);
data->clock=clock();
return 1;
}
int _vb_dummy_stop(vb_handler* handler)
{
if(!handler) return 0;
_vb_dummy_handler* data=handler->data;
if(!data) return 0;
data->clock=0;
data->time=0;
return 1;
}
int _vb_dummy_is_speaking(vb_handler* handler)
{
if(!handler) return 0;
_vb_dummy_handler* data=handler->data;
if(!data) return 0;
if(data->time<=0) return 0;
clock_t elapsed=(clock()-data->clock)*1000/CLOCKS_PER_SEC;
if(elapsed<data->time) return 1;
_vb_dummy_stop(handler);
return 0;
}
void _vb_dummy_cleanup(vb_handler* handler)
{
if(!handler) return;
if(!handler->data) return;
free(handler->data);
}

vb_result _vb_dummy_handler_register(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
vb_handler handler;
handler.implementation.initialise=_vb_dummy_initialise;
handler.implementation.speak=_vb_dummy_speak;
handler.implementation.stop=_vb_dummy_stop;
handler.implementation.is_speaking=_vb_dummy_is_speaking;
handler.implementation.cleanup=_vb_dummy_cleanup;
return vb_handler_register(voice, "Dummy", &handler);
}
