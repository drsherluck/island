#include "traffic_light/traffic_light.h"
#include <type_traits>
#include <assert.h>
#include <dlfcn.h>

// declare state machine object
struct pal_traffic_light_o {
	State currentState = State::eInitial;
};

// ----------------------------------------------------------------------

static pal_traffic_light_o *create() {
	pal_traffic_light_o *newObj = new pal_traffic_light_o{};
	return newObj;
}

// ----------------------------------------------------------------------

static void destroy( pal_traffic_light_o *obj ) {
	delete ( obj );
}

// ----------------------------------------------------------------------

static const State &get_state( pal_traffic_light_o *instance ) {
	return instance->currentState;
}

// ----------------------------------------------------------------------

static void nextState( pal_traffic_light_o *instance ) {
	auto &s = instance->currentState;

	switch ( s ) {
	case State::eInitial:
		s = State::eGreen;
	    break;
	case State::eGreen:
		s = State::eBlink;
	    break;
	case State::eBlink:
		s = State::eYellow;
	    break;
	case State::eYellow:
		s = State::eRed;
	    break;
	case State::eRed:
		s = State::eGreen;

	    break;
	}
}

// ----------------------------------------------------------------------

static void resetState( pal_traffic_light_o *instance ) {
	instance->currentState = State::eInitial;
}

// ----------------------------------------------------------------------

static const char *get_state_as_string( pal_traffic_light_o *instance ) {
	const char *names[ 5 ] =
	    {
	        "Initial",
	        "\033[32;1mGREEN\033[0m",
	        "\033[33;1mYELLOW\033[0m",
	        "\033[32;1mB\033[0mL\033[32;1mI\033[0mN\033[32;1mK\033[0m",
	        "\033[31;1mRED\033[0m",
	    };

	auto index =
	    static_cast<std::underlying_type<decltype( instance->currentState )>::type>( instance->currentState );

	return names[ index ];
}

// ----------------------------------------------------------------------

void register_traffic_light_api( void *api ) {
	auto a = static_cast<pal_traffic_light_api *>( api );

	a->traffic_light_i.create              = create;
	a->traffic_light_i.destroy             = destroy;
	a->traffic_light_i.get_state           = get_state;
	a->traffic_light_i.step                = nextState;
	a->traffic_light_i.reset_state         = resetState;
	a->traffic_light_i.get_state_as_string = get_state_as_string;
}

// ----------------------------------------------------------------------
