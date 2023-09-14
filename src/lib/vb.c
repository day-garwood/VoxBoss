#include "vb.h"

/* Public API */

/* Functions */

vb_result vb_config_initialise(vb_config* config)
{
if(!config) return vbr_invalid_args;
config->handler_preference=NULL;
config->handler_fallback=1;
return vbr_ok;
}
vb_result vb_speaker_initialise(vb_speaker* voice, vb_config* config)
{
if(!voice) return vbr_invalid_args;
vb_config c;
if(!config)
{
vb_result rc=vb_config_initialise(&c);
if(rc!=vbr_ok) return rc;
config=&c;
}
_vb_registry_reset(&voice->registry);
vb_result rc=_vb_register_internal_handlers(voice);
if(rc!=vbr_ok) return rc;
voice->config=*config;
return _vb_initialise_handler(voice);
}
vb_result vb_speak(vb_speaker* voice, char* text, int interrupt)
{
if(!voice) return vbr_invalid_args;
if(!text) return vbr_invalid_args;
if((interrupt<0)||(interrupt>1)) return vbr_invalid_args;
if(!voice->current_handler) return vbr_not_initialised;
if(!voice->current_handler->implementation.speak) return vbr_unsupported;
if(!voice->current_handler->implementation.speak(voice->current_handler, text, interrupt)) return vbr_handler_failed;
return vbr_ok;
}
vb_result vb_stop(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
if(!voice->current_handler) return vbr_not_initialised;
if(!voice->current_handler->implementation.stop) return vbr_unsupported;
if(!voice->current_handler->implementation.stop(voice->current_handler)) return vbr_handler_failed;
return vbr_ok;
}
vb_result vb_is_speaking(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
if(!voice->current_handler) return vbr_not_initialised;
if(!voice->current_handler->implementation.is_speaking) return vbr_unsupported;
if(!voice->current_handler->implementation.is_speaking(voice->current_handler)) return vbr_ok;
return vbr_speaking;
}
vb_result vb_handler_register(vb_speaker* voice, char* id, vb_handler* handler)
{
if(!voice) return vbr_invalid_args;
if(!id) return vbr_invalid_args;
if(id[0]==0) return vbr_invalid_args;
if(_vb_find_handler_by_id(&voice->registry, id)>-1) return vbr_handler_id_taken;
if(!_vb_handler_is_valid_id(id)) return vbr_handler_id_invalid;
if(!handler) return vbr_invalid_args;
if(!_vb_handler_is_usable(handler)) return vbr_handler_invalid;
char* new=malloc(strlen(id)+1);
if(!new) return vbr_memory;
strcpy(new, id);
vb_result rc=_vb_handler_prepare_registration(voice);
if(rc!=vbr_ok)
{
free(new);
return rc;
}
handler->id=new;
voice->registry.handler[voice->registry.count-1]=*handler;
return vbr_ok;
}
void vb_speaker_cleanup(vb_speaker* voice)
{
if(!voice) return;
_vb_handler_cleanup(voice->current_handler);
_vb_registry_cleanup(&voice->registry);
}

/* Internal implementation */

int _vb_find_handler_by_id(vb_registry* manager, char* id)
{
if(!manager) return -1;
for(int x=0; x<manager->count; x++)
{
if(_vb_strcmp(id, manager->handler[x].id, 0)!=0) continue;
return x;
}
return -1;
}
int _vb_handler_is_valid_id(char* id)
{
if(!id) return 0;
if(id[0]==0) return 0;
if(!_vb_string_is_alpha_numeric(id)) return 0;
return 1;
}
void _vb_registry_cleanup(vb_registry* manager)
{
if(!manager) return;
for(int x=0; x<manager->count; x++)
{
_vb_handler_unregister(&manager->handler[x]);
}
_vb_registry_reset(manager);
}
void _vb_handler_unregister(vb_handler* handler)
{
if(!handler) return;
if(handler->id) free(handler->id);
handler->id=NULL;
handler->implementation.initialise=NULL;
handler->implementation.cleanup=NULL;
}
void _vb_handler_cleanup(vb_handler* handler)
{
if(!handler) return;
if(!handler->implementation.cleanup) return;
handler->implementation.cleanup(handler);
}
void _vb_registry_reset(vb_registry* manager)
{
if(!manager) return;
manager->handler=NULL;
manager->count=0;
}
vb_result _vb_initialise_handler(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
vb_result rc=_vb_initialise_preferred_handler(voice);
if(rc==vbr_ok) return rc;
if(!voice->config.handler_fallback) return vbr_init_failed;
return _vb_initialise_any_handler(voice);
}
vb_result _vb_initialise_preferred_handler(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
if(!voice->config.handler_preference) return vbr_init_failed;
int id=_vb_find_handler_by_id(&voice->registry, voice->config.handler_preference);
if(id<0) return vbr_init_failed;
if(!voice->registry.handler[id].implementation.initialise) return vbr_init_failed;
if(!voice->registry.handler[id].implementation.initialise(&voice->registry.handler[id])) return vbr_init_failed;
voice->current_handler=&voice->registry.handler[id];
return vbr_ok;
}
vb_result _vb_initialise_any_handler(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
for(int x=0; x<voice->registry.count; x++)
{
if(!voice->registry.handler[x].implementation.initialise) continue;
if(!voice->registry.handler[x].implementation.initialise(&voice->registry.handler[x])) continue;
voice->current_handler=&voice->registry.handler[x];
return vbr_ok;
}
return vbr_init_failed;
}
int _vb_handler_is_usable(vb_handler* handler)
{
if(!handler) return 0;
if(!handler->implementation.initialise) return 0;
if(!handler->implementation.cleanup) return 0;
return 1;
}
int _vb_handler_prepare_registration(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
int c=voice->registry.count+1;
vb_handler* handler=realloc(voice->registry.handler, sizeof(vb_handler)*c);
if(!handler) return vbr_memory;
voice->registry.handler=handler;
voice->registry.count=c;
return vbr_ok;
}

/* Builtin handler implementations */

/* Com helpers, for those handlers that need it. */

int _vb_com_initialise(_vb_com* com)
{
if(!com) return 0;
com->ole=NULL;
com->CoInitialize=NULL;
com->CoCreateInstance=NULL;
com->CoUninitialize=NULL;
com->ole=LoadLibrary("Ole32.dll");
if(!com->ole) return 0;
com->CoInitialize=(HRESULT(WINAPI*)(LPVOID)) GetProcAddress(com->ole, "CoInitialize");
if(!com->CoInitialize)
{
FreeLibrary(com->ole);
return 0;
}
com->CoCreateInstance=(HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*)) GetProcAddress(com->ole, "CoCreateInstance");
if(!com->CoCreateInstance)
{
FreeLibrary(com->ole);
return 0;
}
com->CoUninitialize=(HRESULT(WINAPI*)(void)) GetProcAddress(com->ole, "CoUninitialize");
if(!com->CoUninitialize)
{
FreeLibrary(com->ole);
return 0;
}
if(FAILED(com->CoInitialize(NULL)))
{
FreeLibrary(com->ole);
return 0;
}
return 1;
}
int _vb_com_create_instance(_vb_com* com, CLSID* clsid, IID* iid, void** data)
{
if(!com) return 0;
HRESULT hr=com->CoCreateInstance(clsid, NULL, CLSCTX_ALL, iid, data);
if(FAILED(hr)) return 0;
return 1;
}
int _vb_com_cleanup(_vb_com* com)
{
if(!com) return 0;
com->CoUninitialize();
FreeLibrary(com->ole);
return 1;
}

/* SAPI */

int _vb_sapi_initialise(vb_handler* handler)
{
if(!handler) return 0;
_vb_sapi_handler* data=malloc(sizeof(_vb_sapi_handler));
if(!data) return 0;

/* Unfortunately, because we're runtime linking, we have to specify the COM ID data manually. */
CLSID CLSID_SpVoice={0x96749377, 0x3391, 0x11D2, {0x9E, 0xE3, 0x00, 0xC0, 0x4F, 0x79, 0x73, 0x96}};
IID IID_ISpVoice={0x6C44DF74, 0x72B9, 0x4992, {0xA1, 0xEC, 0xEF, 0x99, 0x6E, 0x04, 0x22, 0xD4}};
if(!_vb_com_initialise(&data->com))
{
free(data);
return 0;
}
if(!_vb_com_create_instance(&data->com, &CLSID_SpVoice, &IID_ISpVoice, (void**) &data->voice))
{
_vb_com_cleanup(&data->com);
free(data);
return 0;
}
data->text_to_speak=NULL;
handler->data=data;
return 1;
}
int _vb_sapi_speak(vb_handler* handler, char* text, int interrupt)
{
if(!handler) return 0;
if(!text) return 0;
if(text[0]==0) return 0;
_vb_sapi_handler* data=handler->data;
if(!data) return 0;
int source_length=strlen(text);
int destination_length=MultiByteToWideChar(CP_ACP, 0, text, source_length, NULL, 0);
WCHAR* wtext=malloc((destination_length+1)*sizeof(WCHAR));
if(!wtext) return 0;
MultiByteToWideChar(CP_ACP, 0, text, source_length, wtext, destination_length);
wtext[destination_length]=0;
if(interrupt) _vb_sapi_stop(handler);
HRESULT hr=data->voice->lpVtbl->Speak(data->voice, wtext, SPF_DEFAULT|SPF_ASYNC, NULL);
if(FAILED(hr))
{
free(wtext);
return 0;
}
data->text_to_speak=wtext;
return 1;
}
int _vb_sapi_is_speaking(vb_handler* handler)
{
if(!handler) return 0;
_vb_sapi_handler* data=handler->data;
if(!data) return 0;
SPVOICESTATUS status;
HRESULT hr=data->voice->lpVtbl->GetStatus(data->voice, &status, NULL);
if(FAILED(hr)) return 0;
if(status.dwRunningState!=SPRS_DONE) return 1;
if(data->text_to_speak) free(data->text_to_speak);
data->text_to_speak=NULL;
return 0;
}
int _vb_sapi_stop(vb_handler* handler)
{
if(!handler) return 0;
_vb_sapi_handler* data=handler->data;
if(!data) return 0;
data->voice->lpVtbl->Speak(data->voice, NULL, SPF_PURGEBEFORESPEAK, NULL);
if(data->text_to_speak) free(data->text_to_speak);
data->text_to_speak=NULL;
return 1;
}
void _vb_sapi_cleanup(vb_handler* handler)
{
if(!handler) return;
_vb_sapi_handler* data=handler->data;
data->voice->lpVtbl->Release(data->voice);
data->voice = NULL;
_vb_com_cleanup(&data->com);
free(data);
handler->data=NULL;
}

/* Builtin handler registrations */

vb_result _vb_register_internal_handlers(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
vb_result result=vbr_ok;
vb_result rc=result;
rc=_vb_sapi_register_handler(voice);
return result;
}
vb_result _vb_sapi_register_handler(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
vb_handler sapi;
sapi.implementation.initialise=_vb_sapi_initialise;
sapi.implementation.speak=_vb_sapi_speak;
sapi.implementation.stop=_vb_sapi_stop;
sapi.implementation.is_speaking=_vb_sapi_is_speaking;
sapi.implementation.cleanup=_vb_sapi_cleanup;
return vb_handler_register(voice, "sapi", &sapi);
}

/* Helper functions */

int _vb_char_is_alpha(char c)
{
if(_vb_char_is_upper(c)) return 1;
if(_vb_char_is_lower(c)) return 1;
return 0;
}
int _vb_char_is_digit(char c)
{
if(c<48) return 0;
if(c>57) return 0;
return 1;
}
int _vb_char_is_alpha_numeric(char c)
{
if(_vb_char_is_alpha(c)) return 1;
if(_vb_char_is_digit(c)) return 1;
return 0;
}
int _vb_string_is_alpha_numeric(char* c)
{
if(!c) return 0;
if(c[0]==0) return 0;
for(int x=0; x<strlen(c); x++)
{
if(!_vb_char_is_alpha_numeric(c[x])) return 0;
}
return 1;
}
int _vb_char_is_upper(char c)
{
if(c<65) return 0;
if(c>90) return 0;
return 1;
}
int _vb_char_is_lower(char c)
{
if(c<97) return 0;
if(c>122) return 0;
return 1;
}
int _vb_strcmp(char* a, char* b, int cs)
{
if((a==NULL)&&(b==NULL)) return 0;
if(!a) return -1;
if(!b) return 1;

if(cs) return strcmp(a, b);

/* We don't need a condition because the comparison checks for us. */
for(int x=0; ; x++)
{
char c1=_vb_char_to_lower(a[x]);
char c2=_vb_char_to_lower(b[x]);
if(c1<c2) return -1;
if(c1>c2) return 1;

/* If this far, the characters are equal, so we only need one check for NULL. */
if(c1==0) return 0;
}
return 0; /* We should theoretically never reach this point. */
}
int _vb_strncmp(char* a, char* b, int count, int cs)
{
if((a==NULL)&&(b==NULL)) return 0;
if(!a) return -1;
if(!b) return 1;

if(cs) return strncmp(a, b, count);

for(int x=0; x<count; x++)
{
char c1=_vb_char_to_lower(a[x]);
char c2=_vb_char_to_lower(b[x]);
if(c1<c2) return -1;
if(c1>c2) return 1;

/* If this far, the characters are equal, so we only need one check for NULL. */
if(c1==0) return 0;
}
return 0; /* We should only reach this point if count has been exceeded. */
}
char _vb_char_to_lower(char x)
{

/* If char is not uppercase alpha, leave as is. */
if(x<65) return x;
if(x>90) return x;
return x|=0x20;
}
