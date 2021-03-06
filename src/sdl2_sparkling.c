//
// sdl2_sparkling.c
// sdl2-sparkling
//
// Created by Arpad Goretity
// on 28/03/2015
//
// Licensed under the 2-clause BSD License
//

#define USE_DYNAMIC_LOADING 1

#include <SDL2/SDL.h>

#include "sdl2_sparkling.h"
#include "sdl2_window.h"
#include "sdl2_event.h"
#include "sdl2_timer.h"
#include "sdl2_extras.h"
#include "sdl2_audio.h"


/////////////////////////////////
// The returned library object //
/////////////////////////////////
static SpnHashMap *library = NULL;

/////////////////////////////////
////////  Prototyping  //////////
/////////////////////////////////
// This is necessary to let a certain class continue to act as a namespace
SpnValue spn_get_lib_prototype(const char *classname)
{
	return spn_hashmap_get_strkey(library, classname);
}

#define SPN_LIB_CREATE_NAMESPACE(class)                    \
	hm = spn_hashmap_new();                                \
	spnlib_SDL_methods_for_##class(hm);                    \
	spn_hashmap_set_strkey(                                \
		library,                                           \
		#class,                                            \
		&(SpnValue){ .type = SPN_TYPE_HASHMAP, .v.o = hm } \
	);                                                     \
	spn_object_release(hm)

//
// Library initialization and deinitialization
//

// the library has been loaded this many times
static unsigned init_refcount = 0;

// helper for building the hashmap representing the library
static void spn_SDL_construct_library(void)
{
	assert(library == NULL);
	library = spn_hashmap_new();

	// top-level library functions
	static const SpnExtFunc fns[] = {
		{ "OpenWindow",   spnlib_SDL_OpenWindow   },
		{ "PollEvent",    spnlib_SDL_PollEvent    },
		{ "StartTimer",   spnlib_SDL_StartTimer   },
		{ "StopTimer",    spnlib_SDL_StopTimer    },
		{ "OpenMusic",    spnlib_SDL_OpenMusic    },
		{ "OpenSample",   spnlib_SDL_OpenSample   },
		{ "OpenChannels", spnlib_SDL_OpenChannels },
		{ "GetError",     spnlib_SDL_GetError     },
		{ "SetError",     spnlib_SDL_SetError     },
		{ "GetMixError",  spnlib_SDL_GetMixError  },
		{ "SetMixError",  spnlib_SDL_SetMixError  },
		{ "GetPaths",     spnlib_SDL_GetPaths     },
		{ "GetVersions",  spnlib_SDL_GetVersions  },
		{ "GetPlatform",  spnlib_SDL_GetPlatform  },
		{ "GetCPUSpecs",  spnlib_SDL_GetCPUSpecs  },
		{ "GetPowerInfo", spnlib_SDL_GetPowerInfo },
		{ "Delay",        spnlib_SDL_Delay        }
	};

	for (size_t i = 0; i < COUNT(fns); i++) {
		SpnValue fnval = spn_makenativefunc(fns[i].name, fns[i].fn);
		spn_hashmap_set_strkey(library, fns[i].name, &fnval);
		spn_value_release(&fnval);
	}

	SpnHashMap *hm;
	SPN_LIB_CREATE_NAMESPACE(Window);
	SPN_LIB_CREATE_NAMESPACE(Music);
	SPN_LIB_CREATE_NAMESPACE(Sample);
	SPN_LIB_CREATE_NAMESPACE(Channels);
}

// when the last reference is gone to our library, we free the resources
static void spn_SDL_destroy_library(void)
{
	assert(library);
	spn_object_release(library);
	library = NULL;
}


// Library constructor and destructor
SPN_LIB_OPEN_FUNC(ctx) {
	if (init_refcount == 0) {
		// initialize SDL with all submodules
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
			fprintf(stderr, "can't initialize SDL: %s\n", SDL_GetError());
			return spn_nilval;
		}

		spn_SDL_construct_library();
	}

	init_refcount++;

	spn_object_retain(library);
	return (SpnValue){ .type = SPN_TYPE_HASHMAP, .v.o = library };
}

SPN_LIB_CLOSE_FUNC(ctx) {
	if (--init_refcount == 0) {
		// free hashmap representing library
		spn_SDL_destroy_library();

		// deinitialize SDL
		SDL_Quit();
	}
}
