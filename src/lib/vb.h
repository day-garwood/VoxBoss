#ifndef _vb_h
#define _vb_h

#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <sapi.h>

/* Public API */

/* Structures */

typedef struct vb_speaker vb_speaker;
typedef struct vb_config vb_config;
typedef struct vb_handler vb_handler;
typedef struct vb_handler_interface vb_handler_interface;

/* Public facing structures */

struct vb_config
{
char* handler_preference;
int handler_fallback;
};

/* Enums */

typedef enum
{
vbr_ok=0,
vbr_internal,
vbr_memory,
vbr_unsupported,
vbr_invalid_args,
vbr_not_initialised,
vbr_init_failed,
vbr_handler_invalid,
vbr_handler_id_invalid,
vbr_handler_id_taken,
vbr_handler_failed,
vbr_speaking,
}
vb_result;

/* Functions */

vb_result vb_config_initialise(vb_config* config);
vb_result vb_speaker_initialise(vb_speaker* voice, vb_config* config);
vb_result vb_speak(vb_speaker* voice, char* text, int interrupt);
vb_result vb_stop(vb_speaker* voice);
vb_result vb_is_speaking(vb_speaker* voice);
vb_result vb_handler_register(vb_speaker* voice, char* id, vb_handler* handler);
void vb_speaker_cleanup(vb_speaker* voice);

/* Internal data */

/* Structures */

typedef struct vb_registry vb_registry;

/* Full structure definitions */

struct vb_handler_interface
{
int (*initialise) (vb_handler* handler);
int (*speak) (vb_handler* handler, char* text, int interrupt);
int (*stop) (vb_handler* handler);
int (*is_speaking) (vb_handler* handler);
void (*cleanup) (vb_handler* handler);
};

struct vb_handler
{
char* id;
vb_handler_interface implementation;
void* data;
};

struct vb_registry
{
vb_handler* handler;
int count;
};

struct vb_speaker
{
vb_config config;
vb_registry registry;
vb_handler* current_handler;
};

/* Functions */

int _vb_find_handler_by_id(vb_registry* manager, char* id);
int _vb_handler_is_valid_id(char* id);
void _vb_registry_cleanup(vb_registry* manager);
void _vb_handler_unregister(vb_handler* handler);
void _vb_handler_cleanup(vb_handler* handler);
void _vb_registry_reset(vb_registry* manager);
vb_result _vb_initialise_handler(vb_speaker* voice);
vb_result _vb_initialise_preferred_handler(vb_speaker* voice);
vb_result _vb_initialise_any_handler(vb_speaker* voice);
int _vb_handler_is_usable(vb_handler* handler);
int _vb_handler_prepare_registration(vb_speaker* voice);

/* Builtin handler implementations */

/* Structures */

typedef struct _vb_sapi_handler _vb_sapi_handler;
typedef struct _vb_com _vb_com;

/* Full structure definitions */

struct _vb_com
{
HMODULE ole;
HRESULT(WINAPI* CoInitialize)(LPVOID);
HRESULT(WINAPI* CoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
HRESULT(WINAPI* CoUninitialize)(void);
};

struct _vb_sapi_handler
{
_vb_com com;
ISpVoice* voice;
WCHAR* text_to_speak;
};

int _vb_com_initialise(_vb_com* com);
int _vb_com_create_instance(_vb_com* com, CLSID* clsid, IID* iid, void** data);
int _vb_com_cleanup(_vb_com* com);

int _vb_sapi_initialise(vb_handler* handler);
int _vb_sapi_speak(vb_handler* handler, char* text, int interrupt);
int _vb_sapi_stop(vb_handler* handler);
int _vb_sapi_is_speaking(vb_handler* handler);
void _vb_sapi_cleanup(vb_handler* handler);

/* Builtin handler registrations */

vb_result _vb_register_internal_handlers(vb_speaker* voice);
vb_result _vb_sapi_register_handler(vb_speaker* voice);

/* Helper functions */

int _vb_char_is_alpha(char c);
int _vb_char_is_digit(char c);
int _vb_char_is_alpha_numeric(char c);
int _vb_string_is_alpha_numeric(char* c);
int _vb_char_is_upper(char c);
int _vb_char_is_lower(char c);
int _vb_strcmp(char* a, char* b, int cs);
int _vb_strncmp(char* a, char* b, int count, int cs);
char _vb_char_to_lower(char x);

#endif
