#pragma once
#include <boost/asio.hpp>
#include <list>

namespace std
{
	namespace asio
	{
		namespace placeholders
		{
			static auto& error = std::placeholders::_1;
			static auto& bytes_transferred = std::placeholders::_2;
		};
	}
};

inline std::list<boost::asio::ip::tcp::endpoint>
asio_query_all_endpoint( boost::asio::io_service& ioservice, const std::string& host, const std::string& query_service )
{
	typedef boost::asio::ip::tcp tcp;
	typedef tcp::resolver resolver;

	std::list<tcp::endpoint> r;
	resolver resolver_( ioservice );
	resolver::query query( host, query_service );

	resolver::iterator end;
	resolver::iterator iter = resolver_.resolve( query );
	for( ; iter != end; ++iter )
	{
		r.push_back( *iter );
	}
	return r;
}

inline boost::asio::ip::tcp::endpoint
asio_query_endpoint( boost::asio::io_service& ioservice, const std::string& host, const std::string& query_service )
{
	typedef boost::asio::ip::tcp tcp;
	typedef tcp::resolver resolver;

	tcp::endpoint r;
	resolver resolver_( ioservice );
	resolver::query query( host, query_service );

	resolver::iterator end;
	resolver::iterator iter = resolver_.resolve( query );
	for( ; iter != end; ++iter )
	{
		r = *iter;
		return r;
	}
	return r;
}

inline boost::asio::ip::address asio_query_address( boost::asio::io_service& ioservice, const std::string& addr )
{
	return asio_query_endpoint( ioservice, addr, "" ).address();
}