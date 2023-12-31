#include "vb.h"

/* Public API */

/* Functions */

vb_result vb_config_initialise(vb_config* config, char* preference)
{
if(!config) return vbr_invalid_args;
config->handler_preference=NULL;
config->handler_fallback=1;
if(!preference) return vbr_ok;
if(preference[0]==0) return vbr_ok;
char* prefstore=malloc(strlen(preference)+1);
if(!prefstore) return vbr_memory;
strcpy(prefstore, preference);
config->handler_preference=prefstore;
return vbr_ok;
}
vb_result vb_speaker_initialise(vb_speaker* voice, vb_config* config)
{
if(!voice) return vbr_invalid_args;
vb_config c;
if(!config)
{
vb_result rc=vb_config_initialise(&c, NULL);
if(rc!=vbr_ok) return rc;
config=&c;
}
_vb_registry_reset(&voice->registry);
vb_result rc=_vb_register_internal_handlers(voice);
if(rc!=vbr_ok) return rc;
voice->config=*config;
return vbr_ok;
}
vb_result vb_handler_register(vb_speaker* voice, char* id, vb_handler* handler)
{
if(!voice) return vbr_invalid_args;
if(!id) return vbr_invalid_args;
if(id[0]==0) return vbr_invalid_args;
if(!_vb_handler_is_valid_id(id)) return vbr_handler_id_invalid;
if(_vb_find_handler_by_id(&voice->registry, id)>-1) return vbr_handler_id_taken;
if(!handler) return vbr_invalid_args;
if(!_vb_handler_is_usable(handler)) return vbr_handler_invalid;
char* new=malloc(strlen(id)+1);
if(!new) return vbr_memory;
strcpy(new, id);
vb_result rc=_vb_handler_prepare_registration(&voice->registry);
if(rc!=vbr_ok)
{
free(new);
return rc;
}
handler->id=new;
voice->registry.handler[voice->registry.count-1]=*handler;
return vbr_ok;
}
vb_result vb_speaker_start(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
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
vb_result vb_pause(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
if(!voice->current_handler) return vbr_not_initialised;
if(!voice->current_handler->implementation.pause) return vbr_unsupported;
if(!voice->current_handler->implementation.pause(voice->current_handler)) return vbr_handler_failed;
return vbr_ok;
}
vb_result vb_resume(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
if(!voice->current_handler) return vbr_not_initialised;
if(!voice->current_handler->implementation.resume) return vbr_unsupported;
if(!voice->current_handler->implementation.resume(voice->current_handler)) return vbr_handler_failed;
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
vb_result vb_speaker_stop(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
if(!voice->current_handler) return vbr_ok;
_vb_handler_cleanup(voice->current_handler);
voice->current_handler=NULL;
return vbr_ok;
}
void vb_speaker_cleanup(vb_speaker* voice)
{
if(!voice) return;
vb_speaker_stop(voice);
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
for(int x=0; x<strlen(id); x++)
{
if(_vb_char_is_alpha_numeric(id[x])) continue;
char c=id[x];
if(c==45) continue; /* dash */
if(c==46) continue; /* dot */
if(c==95) continue; /* underscore */
return 0;
}
return 1;
}
void _vb_registry_cleanup(vb_registry* manager)
{
if(!manager) return;
if(!manager->handler)
{
_vb_registry_reset(manager);
return;
}
for(int x=0; x<manager->count; x++)
{
_vb_handler_unregister(&manager->handler[x]);
}
free(manager->handler);
_vb_registry_reset(manager);
}
void _vb_handler_unregister(vb_handler* handler)
{
if(!handler) return;
if(handler->id) free(handler->id);
handler->id=NULL;
_vb_handler_implementation_reset(&handler->implementation);
}
void _vb_handler_implementation_reset(vb_handler_interface* i)
{
if(!i) return;
i->initialise=NULL;
i->speak=NULL;
i->stop=NULL;
i->is_speaking=NULL;
i->cleanup=NULL;
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
vb_result _vb_handler_prepare_registration(vb_registry* registry)
{
if(!registry) return vbr_invalid_args;
int c=registry->count+1;
vb_handler* handler=realloc(registry->handler, sizeof(vb_handler)*c);
if(!handler) return vbr_memory;
registry->handler=handler;
registry->count=c;
return vbr_ok;
}

/* Builtin handler implementations */

/* Windows specific handlers */

#ifdef _WIN32

/* Com helpers, for those handlers that need it. */

int _vb_com_initialise(_vb_com* com)
{
if(!com) return 0;
_vb_com_reset(com);
com->ole=LoadLibrary("Ole32.dll");
if(!com->ole) return 0;
com->CoInitializeEx=(HRESULT(WINAPI*)(LPVOID, DWORD)) GetProcAddress(com->ole, "CoInitializeEx");
if(!com->CoInitializeEx)
{
FreeLibrary(com->ole);
_vb_com_reset(com);
return 0;
}
com->CoCreateInstance=(HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*)) GetProcAddress(com->ole, "CoCreateInstance");
if(!com->CoCreateInstance)
{
FreeLibrary(com->ole);
_vb_com_reset(com);
return 0;
}
com->CoUninitialize=(HRESULT(WINAPI*)(void)) GetProcAddress(com->ole, "CoUninitialize");
if(!com->CoUninitialize)
{
FreeLibrary(com->ole);
_vb_com_reset(com);
return 0;
}
HRESULT hr=com->CoInitializeEx(NULL, COINIT_MULTITHREADED);
if(hr==S_OK)
{
com->autoinit=1; /* This will tell us whether we need to uninitialise. */
return 1; /* Initialisation succeeded. */
}
if((hr==S_FALSE)||(hr==RPC_E_CHANGED_MODE)) return 1; /* Already initialised. */
FreeLibrary(com->ole);
_vb_com_reset(com);
return 0;
}
void _vb_com_reset(_vb_com* com)
{
if(!com) return;
com->ole=NULL;
com->CoInitializeEx=NULL;
com->CoCreateInstance=NULL;
com->CoUninitialize=NULL;
com->autoinit=0;
}
int _vb_com_create_instance(_vb_com* com, CLSID* clsid, IID* iid, void** data)
{
if(!com) return 0;
HRESULT hr=com->CoCreateInstance(clsid, NULL, CLSCTX_ALL, iid, data);
if(FAILED(hr)) return 0;
return 1;
}
void _vb_com_cleanup(_vb_com* com)
{
if(!com) return;
if(com->autoinit) com->CoUninitialize();
FreeLibrary(com->ole);
_vb_com_reset(com);
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
int _vb_sapi_pause(vb_handler* handler)
{
if(!handler) return 0;
_vb_sapi_handler* data=handler->data;
if(!data) return 0;
HRESULT hr=data->voice->lpVtbl->Pause(data->voice);
if(FAILED(hr)) return 0;
return 1;
}
int _vb_sapi_resume(vb_handler* handler)
{
if(!handler) return 0;
_vb_sapi_handler* data=handler->data;
if(!data) return 0;
HRESULT hr=data->voice->lpVtbl->Resume(data->voice);
if(FAILED(hr)) return 0;
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

#endif

/* Apple-specific handlers */

#ifdef __APPLE__

int _vb_objc_initialise(_vb_objc* objc)
{
if(!objc) return 0;
_vb_objc_reset(objc);
objc->objc=dlopen("/usr/lib/libobjc.dylib", RTLD_NOW);
if(!objc->objc) return 0;
objc->getClass=dlsym(objc->objc, "objc_getClass");
if(!objc->getClass)
{
dlclose(objc->objc);
_vb_objc_reset(objc);
return 0;
}
objc->msgSend=dlsym(objc->objc, "objc_msgSend");
if(!objc->msgSend)
{
dlclose(objc->objc);
_vb_objc_reset(objc);
return 0;
}
objc->sel_registerName=dlsym(objc->objc, "sel_registerName");
if(!objc->sel_registerName)
{
dlclose(objc->objc);
_vb_objc_reset(objc);
return 0;
}
return 1;
}
void _vb_objc_reset(_vb_objc* objc)
{
if(!objc) return;
objc->objc=NULL;
objc->getClass=NULL;
objc->msgSend=NULL;
objc->sel_registerName=NULL;
}
void _vb_objc_cleanup(_vb_objc* objc)
{
if(!objc) return;
if(!objc->objc) return;
dlclose(objc->objc);
_vb_objc_reset(objc);
}

int _vb_mac_initialise(vb_handler* handler)
{
if(!handler) return 0;
_vb_mac_handler* data=malloc(sizeof(_vb_mac_handler));
if(!data) return 0;
if(!_vb_objc_initialise(&data->objc))
{
free(data);
return 0;
}
data->foundation=dlopen("/System/Library/Frameworks/Foundation.framework/Foundation", RTLD_LOCAL);
if(!data->foundation)
{
_vb_objc_cleanup(&data->objc);
free(data);
return 0;
}
data->appkit=dlopen("/System/Library/Frameworks/AppKit.framework/AppKit", RTLD_LOCAL);
if(!data->appkit)
{
dlclose(data->foundation);
_vb_objc_cleanup(&data->objc);
free(data);
return 0;
}
Class synthobj=data->objc.getClass("NSSpeechSynthesizer");
if(!synthobj)
{
dlclose(data->appkit);
dlclose(data->foundation);
_vb_objc_cleanup(&data->objc);
free(data);
return 0;
}
SEL alloc_selector=data->objc.sel_registerName("alloc");
if(!alloc_selector)
{
dlclose(data->appkit);
dlclose(data->foundation);
_vb_objc_cleanup(&data->objc);
free(data);
return 0;
}
SEL init_selector=data->objc.sel_registerName("init");
if(!init_selector)
{
dlclose(data->appkit);
dlclose(data->foundation);
_vb_objc_cleanup(&data->objc);
free(data);
return 0;
}
data->voice=data->objc.msgSend((id) synthobj, alloc_selector);
if(!data->voice)
{
dlclose(data->appkit);
dlclose(data->foundation);
_vb_objc_cleanup(&data->objc);
free(data);
return 0;
}
id init_voice=data->objc.msgSend(data->voice, init_selector);
if(!init_voice)
{
SEL release_selector=data->objc.sel_registerName("release");
data->objc.msgSend(data->voice, release_selector);
data->voice=NULL;
dlclose(data->appkit);
dlclose(data->foundation);
_vb_objc_cleanup(&data->objc);
free(data);
return 0;
}
data->voice=init_voice;
handler->data=data;
return 1;
}

int _vb_mac_speak(vb_handler* handler, char* text, int interrupt)
{
if(!handler) return 0;
if(!text) return 0;
if(text[0]==0) return 0;
if((interrupt<0)||(interrupt>1)) return 0;
_vb_mac_handler* data=handler->data;
if(!data) return 0;
if(!data->voice) return 0;
if(interrupt) _vb_mac_stop(handler);
Class stringobj=data->objc.getClass("NSString");
if(!stringobj) return 0;
SEL utf_selector=data->objc.sel_registerName("stringWithUTF8String:");
id wtext=data->objc.msgSend((id) stringobj, utf_selector, text);
if(!wtext) return 0;
SEL speak_selector=data->objc.sel_registerName("startSpeakingString:");
if(!speak_selector) return 0;
BOOL result=(BOOL) (intptr_t) data->objc.msgSend(data->voice, speak_selector, wtext);
return result;
}

int _vb_mac_stop(vb_handler* handler)
{
if(!handler) return 0;
_vb_mac_handler* data=handler->data;
if(!data) return 0;
if(!data->voice) return 0;
SEL stop_selector=data->objc.sel_registerName("stopSpeaking");
if(!stop_selector) return 0;
data->objc.msgSend(data->voice, stop_selector);
return 1;
}

int _vb_mac_pause(vb_handler* handler)
{
if(!handler) return 0;
_vb_mac_handler* data=handler->data;
if(!data) return 0;
if(!data->voice) return 0;
SEL pause_selector=data->objc.sel_registerName("pauseSpeakingAtBoundary");
if(!pause_selector) return 0;
data->objc.msgSend(data->voice, pause_selector, 0);
return 1;
}
int _vb_mac_resume(vb_handler* handler)
{
if(!handler) return 0;
_vb_mac_handler* data=handler->data;
if(!data) return 0;
if(!data->voice) return 0;
SEL continue_selector=data->objc.sel_registerName("continueSpeaking");
if(!continue_selector) return 0;
data->objc.msgSend(data->voice, continue_selector);
return 1;
}
int _vb_mac_is_speaking(vb_handler* handler)
{
if(!handler) return 0;
_vb_mac_handler* data=handler->data;
if(!data) return 0;
if(!data->voice) return 0;
SEL stat_selector=data->objc.sel_registerName("isSpeaking");
if(!stat_selector) return 0;
BOOL result=(BOOL) (intptr_t) data->objc.msgSend(data->voice, stat_selector);
return result;
}
void _vb_mac_cleanup(vb_handler* handler)
{
if(!handler) return;
_vb_mac_handler* data=handler->data;
if(!data) return;
if(!data->voice) return;
SEL release_selector=data->objc.sel_registerName("release");
data->objc.msgSend(data->voice, release_selector);
data->voice=NULL;
dlclose(data->appkit);
dlclose(data->foundation);
_vb_objc_cleanup(&data->objc);
free(data);
handler->data=NULL;
}

#endif

/* Builtin handler registrations */

vb_result _vb_register_internal_handlers(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
vb_result result=vbr_ok;
vb_result rc=result;
rc=_vb_sapi_register_handler(voice);
rc=_vb_mac_register_handler(voice);
return result;
}

/* Windows specific handlers */

vb_result _vb_sapi_register_handler(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
#ifdef _WIN32
vb_handler sapi;
sapi.implementation.initialise=_vb_sapi_initialise;
sapi.implementation.speak=_vb_sapi_speak;
sapi.implementation.stop=_vb_sapi_stop;
sapi.implementation.pause=_vb_sapi_pause;
sapi.implementation.resume=_vb_sapi_resume;
sapi.implementation.is_speaking=_vb_sapi_is_speaking;
sapi.implementation.cleanup=_vb_sapi_cleanup;
return vb_handler_register(voice, "sapi", &sapi);
#else
return vbr_unsupported;
#endif
}

/* Apple specific handlers. */

vb_result _vb_mac_register_handler(vb_speaker* voice)
{
if(!voice) return vbr_invalid_args;
#ifdef __APPLE__
vb_handler mac;
mac.implementation.initialise=_vb_mac_initialise;
mac.implementation.speak=_vb_mac_speak;
mac.implementation.stop=_vb_mac_stop;
mac.implementation.pause=_vb_mac_pause;
mac.implementation.resume=_vb_mac_resume;
mac.implementation.is_speaking=_vb_mac_is_speaking;
mac.implementation.cleanup=_vb_mac_cleanup;
return vb_handler_register(voice, "Mac-TTS", &mac);
#else
return vbr_unsupported;
#endif
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
char _vb_char_to_lower(char x)
{

/* If char is not uppercase alpha, leave as is. */
if(x<65) return x;
if(x>90) return x;
return x|=0x20;
}
