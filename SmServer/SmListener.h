#pragma once
#include "SmHttpSession.h"


class SmListener : public std::enable_shared_from_this<SmListener>
{
	net::io_context& ioc_;
	tcp::acceptor acceptor_;
	std::shared_ptr<std::string const> doc_root_;

public:
	SmListener(
		net::io_context& ioc,
		tcp::endpoint endpoint,
		std::shared_ptr<std::string const> const& doc_root)
		: ioc_(ioc)
		, acceptor_(net::make_strand(ioc))
		, doc_root_(doc_root)
	{
		beast::error_code ec;

		// Open the acceptor
		acceptor_.open(endpoint.protocol(), ec);
		if (ec)
		{
			SmCommon::fail(ec, "open");
			return;
		}

		// Allow address reuse
		acceptor_.set_option(net::socket_base::reuse_address(true), ec);
		if (ec)
		{
			SmCommon::fail(ec, "set_option");
			return;
		}

		// Bind to the server address
		acceptor_.bind(endpoint, ec);
		if (ec)
		{
			SmCommon::fail(ec, "bind");
			return;
		}

		// Start listening for connections
		acceptor_.listen(
			net::socket_base::max_listen_connections, ec);
		if (ec)
		{
			SmCommon::fail(ec, "listen");
			return;
		}
	}

	// Start accepting incoming connections
	void
		run()
	{
		do_accept();
	}

private:
	void
		do_accept()
	{
		// The new connection gets its own strand
		acceptor_.async_accept(
			net::make_strand(ioc_),
			beast::bind_front_handler(
				&SmListener::on_accept,
				shared_from_this()));
	}

	void
		on_accept(beast::error_code ec, tcp::socket socket)
	{
		if (ec)
		{
			SmCommon::fail(ec, "accept");
		}
		else
		{
			// Create the http session and run it
			std::make_shared<SmHttpSession>(
				std::move(socket),
				doc_root_)->run();
		}

		// Accept another connection
		do_accept();
	}
};

