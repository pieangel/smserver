#pragma once
#include "ServerDefine.h"
#include "SmCommon.h"
#include <memory>

class SmWebsocketSession : public std::enable_shared_from_this<SmWebsocketSession>
{
	websocket::stream<beast::tcp_stream> ws_;
	beast::flat_buffer buffer_;
	std::vector<boost::shared_ptr<std::string const>> queue_;
public:
	// Take ownership of the socket
	explicit
		SmWebsocketSession(tcp::socket && socket)
		: ws_(std::move(socket))
	{
	}

	void send(boost::shared_ptr<std::string const> const& ss);

	// Start the asynchronous accept operation
	template<class Body, class Allocator>
	void
		do_accept(http::request<Body, http::basic_fields<Allocator>> req)
	{
		// Set suggested timeout settings for the websocket
		ws_.set_option(
			websocket::stream_base::timeout::suggested(
				beast::role_type::server));

		// Set a decorator to change the Server of the handshake
		ws_.set_option(websocket::stream_base::decorator(
			[](websocket::response_type & res)
			{
				res.set(http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) +
					" advanced-server");
			}));

		// Accept the websocket handshake
		ws_.async_accept(
			req,
			beast::bind_front_handler(
				&SmWebsocketSession::on_accept,
				shared_from_this()));
	}

private:
	void
		on_send(boost::shared_ptr<std::string const> const& ss);
	void
		on_accept(beast::error_code ec)
	{
		if (ec)
			return SmCommon::fail(ec, "accept");

		// Read a message
		do_read();
	}

	void
		do_read()
	{
		// Read a message into our buffer
		ws_.async_read(
			buffer_,
			beast::bind_front_handler(
				&SmWebsocketSession::on_read,
				shared_from_this()));
	}

	void
		on_read(
			beast::error_code ec,
			std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		// This indicates that the websocket_session was closed
		if (ec == websocket::error::closed)
			return;

		if (ec)
			SmCommon::fail(ec, "read");

		// Echo the message
		ws_.text(ws_.got_text());
		ws_.async_write(
			buffer_.data(),
			beast::bind_front_handler(
				&SmWebsocketSession::on_write,
				shared_from_this()));
	}

	void
		on_write(
			beast::error_code ec,
			std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return SmCommon::fail(ec, "write");

		// Clear the buffer
		buffer_.consume(buffer_.size());

		// Do another read
		do_read();
	}
};
