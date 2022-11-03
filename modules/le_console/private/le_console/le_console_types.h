#pragma once

static constexpr auto LOG_CHANNEL = "le_console";
static constexpr auto PORT        = "3535";
static constexpr auto BACKLOG     = 3;

struct le_console_o {
	// members
	uint32_t use_count = 0;

	class channel {
		std::mutex              messages_mtx;
		std::deque<std::string> messages;
		uint32_t                max_messages_count = 32; // maximum number of messages to wait on this channel

	  public:
		bool fetch( std::string& msg ) {

			auto lock = std::scoped_lock( messages_mtx );
			if ( messages.empty() ) {
				return false;
			}
			std::swap( msg, messages.front() );
			messages.pop_front();
			return true;
		}

		bool post( std::string const&& msg ) {

			bool enough_space = true;
			auto lock         = std::scoped_lock( messages_mtx );
			while ( messages.size() >= max_messages_count ) {
				messages.pop_front();
				enough_space = false;
			}
			messages.emplace_back( std::move( msg ) );
			return enough_space;
		}
	};

	struct connection_t {
		channel channel_out;
		channel channel_in;

		bool     wants_log_subscriber = false;
		bool     wants_close          = false; // to signal that we want to close this connection
		uint32_t log_level_mask       = 0;     // -1 means everything 0, means nothing

		enum class State {
			ePlain = 0,      // plain socket - this is how we start up
			eTelnetLineMode, // user-requested. telnet line mode
		};

		State state = State::ePlain;

		std::string input_buffer; // used for linemode

		enum class CharTable : uint32_t { // substitute local characters table
			CharTable_invalid,
			synch = 0x01, // synch
			brk   = 0x02, // break
			ip    = 0x03, // interrupt process
			ao    = 0x04, // abort output
			ayt   = 0x05, // are you there?
			eor   = 0x06, // end of record
			abort = 0x07, // abort
			eof   = 0x08, // end of file
			susp  = 0x09, // suspend
			ec    = 0x0a, // erase character (to the left)
			el    = 0x0b, // erase line
			ew    = 0x0c, // erase word
			rp    = 0x0d, // reprint line
			lnext = 0x0e, // literal next: next character to be taken literally, no mapping should be done
			xon   = 0x0f,
			xoff  = 0x10,
			forw1 = 0x11,
			forw2 = 0x12,
			mcl   = 0x13,
			mcr   = 0x14,
			mcwl  = 0x15,
			mcwr  = 0x16,
			mcboL = 0x17,
			mceoL = 0x18,
			insrT = 0x19,
			over  = 0x1a,
			ecr   = 0x1b,
			ewr   = 0x1c,
			ebol  = 0x1d,
			eeol  = 0x1e,
			CharTable_last,
		};

		char slc_remote[ uint32_t( CharTable::CharTable_last ) ] = {};
		// char slc_local[ uint32_t( CharTable::CharTable_last ) ]  = {};

		// In case we have linemode active,
		// we must respect SLC, substitute local characters
		// for which we must keep around a mapping table.
	};

	std::mutex                                                           connections_mutex;
	std::unordered_map<int, std::unique_ptr<le_console_o::connection_t>> connections; // socket filedescriptor -> connection

	struct le_console_server_o* server = nullptr;
};