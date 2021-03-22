#pragma once
#include <cpprest/ws_client.h>
#include <memory>
#include "../common/base_types.hpp"

#include "../common/macros.hpp"

using WebSocketIncomingMessage = const web::websockets::client::websocket_incoming_message&;
using web::websockets::client::websocket_callback_client, web::websockets::client::websocket_outgoing_message;

#include "../common/unmacros.hpp"
