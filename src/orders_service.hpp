#pragma once
#include "tick.hpp"

let orders_service = [](exchange_name ex_name, String ws_uri, String work_bind, String pub_bind) -> void {
    let ctx = make_context(1);
    let worker = make_pull(ctx, work_bind);
    let publisher = make_publisher(ctx, pub_bind);
    let wsock = make_websocket(ws_uri);

};
