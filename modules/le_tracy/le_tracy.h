#ifndef GUARD_le_tracy_H
#define GUARD_le_tracy_H

#include "le_core.h"

/*
 * # Low-Overhead Profiling via Tracy
 *
 * Profiling is only enabled (and Tracy only compiled) if you add the
 * following compiler definition into your topmost CMakeLists.txt file:
 *
 * 	add_compile_definitions( TRACY_ENABLE )
 *
 * To enable passing log messages to Tracy, add the single statement:
 * `LE_TRACY_ENABLE_LOG(-1)` to where you initialize your main app
 * (this is most likely your `app::initialize()` method).
 *
 * Every module which uses Tracy must load the tracy library; you
 * must do this via an explicit library load, by adding
 *
 *  	LE_LOAD_TRACING_LIBRARY
 *
 * to where you initialize the module's api pointers in its cpp file. In case
 * Tracy is not used this statement melts away to a no-op.
 *
 * # TRACY PROFILER
 *
 * To view Tracy captures, you need to run the Tracy Profiler application, which
 * you first must compile from source. You find the source for the Profiler
 * application under `le_tracy/3rdparty/tracy/profiler/build`
 *
 * Note that if you build the profiler on Windows, it is recommended that you
 * first execute `le_tracy/3rdparty/tracy/vcpkg/install_vcpkg_dependencies.bat`
 * so that you have all necessary profiler dependencies installed before
 * compiling.
 *
 * For more information on Tracy, and on how to use the Tracy Profiler, see
 * the [Tracy repository on github](https://github.com/wolfpld/tracy)
 *
 */

#if defined( PLUGINS_DYNAMIC ) and defined( TRACY_ENABLE )
#	define LE_LOAD_TRACING_LIBRARY \
		le_core_load_library_persistently( "libTracyClient.so" )
#else
#	define LE_LOAD_TRACING_LIBRARY
#endif

#if defined( TRACY_ENABLE )
#	define LE_TRACY_ENABLE_LOG( l ) \
		le_tracy::api->le_tracy_i.enable_log( l )
#else
#	define LE_TRACY_ENABLE_LOG( l ) \
		( l )
#endif

// We break our rule here which says that header files are not allowed
// to include other header files.

// this is because this header file will only be included from cpp files
// anyway and we want to have an unique point at which we control what
// gets included for tracy and which defines etc are set.

//----------------------------------------------------------------------
// here, we essentially mirror the .hpp file that is provided with Tracy
// but we make some changes to make tracing slightly more generic,

#include "3rdparty/tracy/public/tracy/Tracy.hpp"

//----------------------------------------------------------------------

// we allow ourselves a tracy context object so that we can store
// any auxiliary information associated with tracing in one place.
// this includes a subscriber to the logger, if we so wish

struct le_tracy_o;

// clang-format off
struct le_tracy_api {
	
	struct le_tracy_interface_t {
		void            ( * enable_log                ) ( uint32_t log_messages_mask);
		
	};
	
	le_tracy_interface_t       le_tracy_i;
	le_tracy_o* le_tracy_singleton;
};
// clang-format on

LE_MODULE( le_tracy );
LE_MODULE_LOAD_DEFAULT( le_tracy );

#ifdef __cplusplus

namespace le_tracy {
static const auto& api        = le_tracy_api_i;
static const auto& le_tracy_i = api->le_tracy_i;
} // namespace le_tracy

#	if ( WIN32 ) and defined( PLUGINS_DYNAMIC ) and defined( TRACY_ENABLE )
#		pragma comment( lib, "modules/TracyClient.lib" )
#	endif

#endif // __cplusplus

#endif
