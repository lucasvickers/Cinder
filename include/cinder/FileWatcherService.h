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

#if defined( CINDER_COCOA )
	#include "FileWatcherImplFsevents.h"
#else
	//fallback method
	#include "FileWatcherImplPolling.h"
#endif


// TODO get this into cinder's land
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

// TODO move to cinder's asio
//#include "asio/asio.hpp"

namespace cinder {

//template <typename FileWatcherImplementation = FileWatcherImpl>
class FileWatcherService
// move this service to cinder (optional based on who runloop)
	: public boost::asio::io_service::service
{
public:
	static boost::asio::io_service::id id;

	explicit FileWatcherService( boost::asio::io_service &io_service )
		: boost::asio::io_service::service( io_service ),
		  mAsyncMonitorWork( new boost::asio::io_service::work( mAsyncMonitorIoService ) ),
		  mAsyncMonitorThread( boost::bind( &boost::asio::io_service::run, &mAsyncMonitorIoService ) )
	{
	}

	~FileWatcherService()
	{
		// The asyncMonitor thread will finish when mAsyncMonitorWork is reset as all asynchronous
		// operations have been aborted and were discarded before (in destroy).
		mAsyncMonitorWork.reset();

		// Event processing is stopped to discard queued operations.
		mAsyncMonitorIoService.stop();

		// The asyncMonitor thread is joined to make sure the file monitor service is
		// destroyed _after_ the thread is finished (not that the thread tries to access
		// instance properties which don't exist anymore).
		mAsyncMonitorThread.join();
	}

	// TODO move to std
	typedef boost::shared_ptr<FileWatcherImpl> implementation_type;

	void construct( implementation_type &impl )
	{
		impl.reset( new FileWatcherImpl() );
	}

	void destroy( implementation_type &impl )
	{
		// If an asynchronous call is currently waiting for an event
		// we must interrupt the blocked call to make sure it returns.
		impl->destroy();

		impl.reset();
	}

	addPath( implementation_type &impl, const boost::filesystem::path &path )
	{
		if ( ! boost::filesystem::is_directory( path ) ) {
			// TODO migrate to a different exception
			throw std::invalid_argument("boost::asio::BasicFileMonitorService::addFile: \"" +
			                            path.string() + "\" is not a valid file or directory entry");
		}

		return impl->addPath( path, regexMatch );
	}

	uint64_t addFile( implementation_type &impl, const boost::filesystem::path &path )
	{
		if ( ! boost::filesystem::is_regular_file( path ) ) {
			// TODO migrate to a different exception
			throw std::invalid_argument("boost::asio::BasicFileMonitorService::addFile: \"" +
			                            path.string() + "\" is not a valid file or directory entry");
		}
		if ( boost::filesystem::is_symlink( path ) && boost::filesystem::read_symlink( path ) != path ) {
			// TODO migrate to a different exception
			throw std::invalid_argument("boost::asio::BasicFileMonitorService::addFile: \"" +
			                            path.string() + "\" this path is a symlink and must be resolved");
		}
		return impl->addFile( path );
	}

	void remove( implementation_type &impl, const boost::filesystem::path &path )
	{
		impl->remove( path );
	}

	/**
	 * Blocking event monitor.
	 */
	FileWatcherEvent monitor( implementation_type &impl, boost::system::error_code &ec )
	{
		return impl->popFrontEvent( ec );
	}

	template <typename Handler>
	class MonitorOperation
	{
	public:
		MonitorOperation( implementation_type &impl, boost::asio::io_service &ioService, Handler handler )
				: mImpl( impl ), mIoService( ioService ), mWork( ioService ), mHandler( handler )
		{
		}

		void operator()() const
		{
			implementation_type impl = mImpl.lock();
			if( impl ) {
				boost::system::error_code ec;
				FileWatcherEvent ev = impl->popFrontEvent( ec );
				this->mIoService.post( boost::asio::detail::bind_handler( mHandler, ec, ev ) );
			}
			else {
				this->mIoService.post( boost::asio::detail::bind_handler( mHandler,
				                                                          boost::asio::error::operation_aborted,
				                                                          FileWatcherEvent() ) );
			}
		}

	private:
		boost::weak_ptr<FileWatcherImpl> 	        mImpl;
		boost::asio::io_service 					&mIoService;
		boost::asio::io_service::work 				mWork;
		Handler 									mHandler;
	};

	/**
	 * Non-blocking event monitor.
	 */
	template <typename Handler>
	void asyncMonitor( implementation_type &impl, Handler handler )
	{
		this->mAsyncMonitorIoService.post( MonitorOperation<Handler>( impl, this->get_io_service(), handler ) );
	}

private:
	void shutdown_service()
	{
		//TODO need anything?
	}

	boost::asio::io_service 							mAsyncMonitorIoService;
	//! note: migrated from scoped_ptr.  remove comment if this works
	std::unique_ptr<boost::asio::io_service::work> 		mAsyncMonitorWork;
	std::thread 										mAsyncMonitorThread;
};

//template <typename FileWatcherImpl>
//boost::asio::io_service::id FileWatcherService<FileWatcherImpl>::id;
boost::asio::io_service::id FileWatcherService::id;

}
