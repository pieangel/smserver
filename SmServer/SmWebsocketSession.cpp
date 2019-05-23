
#include "SmWebsocketSession.h"
#include "ServerDefine.h"
#include <iostream>

void
SmWebsocketSession::
on_send(boost::shared_ptr<std::string const> const& ss)
{
	// Always add to queue
	queue_.push_back(ss);

	// Are we already writing?
	if (queue_.size() > 1)
		return;

	// We are not currently writing, so send this immediately
	ws_.async_write(
		net::buffer(*queue_.front()),
		beast::bind_front_handler(
			&SmWebsocketSession::on_write,
			shared_from_this()));
}


void SmWebsocketSession::send(boost::shared_ptr<std::string const> const& ss)
{
	// Post our work to the strand, this ensures
	// that the members of `this` will not be
	// accessed concurrently.

	net::post(
		ws_.get_executor(),
		beast::bind_front_handler(
			&SmWebsocketSession::on_send,
			shared_from_this(),
			ss));
}
