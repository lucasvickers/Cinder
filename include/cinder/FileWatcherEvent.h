/*
 Copyright (c) 2017, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <string>
#include <ostream>

namespace cinder {

struct FileWatcherEvent
{
	enum EventType
	{
		NONE,
		REMOVED,		// file removed
		ADDED,			// file added
		MODIFIED,		// file changed
		RENAMED_OLD,	// file renamed, old name
		RENAMED_NEW		// file renamed, new name
	};

	FileWatcherEvent()
	: type( NONE ), id( 0 )
	{ }

	FileWatcherEvent( const boost::filesystem::path &p, EventType t, uint64_t id )
	: path( p ), type( t ), id( id )
	{ }

	boost::filesystem::path path;
	EventType 				type;
	uint64_t 				id;
};

inline std::ostream& operator << ( std::ostream& os, const FileWatcherEvent &ev )
{
	os << "FileWatcherEvent "
	<< []( int type ) {
		switch( type ) {
			case FileWatcherEvent::REMOVED: return "REMOVED";
			case FileWatcherEvent::ADDED: return "ADDED";
			case FileWatcherEvent::MODIFIED: return "MODIFIED";
			case FileWatcherEvent::RENAMED_OLD: return "RENAMED_OLD";
			case FileWatcherEvent::RENAMED_NEW: return "RENAMED_NEW";
			default: return "UNKNOWN";
		}
	} ( ev.type ) << " " << ev.path;
	return os;
}

}