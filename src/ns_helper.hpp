#ifndef ns_helper_hpp
#define ns_helper_hpp

#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/ws_client.h>
#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

namespace ws {
using client = web::websockets::client::websocket_callback_client;
using in_message = web::websockets::client::websocket_incoming_message;
using out_message = web::websockets::client::websocket_outgoing_message;
using config = web::websockets::client::websocket_client_config;
}  // namespace ws

namespace logger {
using namespace spdlog;
}

namespace json {
using namespace rapidjson;
}

namespace rest {
using methods = web::http::methods;
using status_codes = web::http::status_codes;
using client = web::http::client::http_client;
using config = web::http::client::http_client_config;
using response = web::http::http_response;

}  // namespace rest

#endif
