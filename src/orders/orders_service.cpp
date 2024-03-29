#include "../common/base_types.hpp"
#include "../deps/logger.hpp"
#include "../deps/websocket.hpp"
#include "../deps/zero.hpp"

#include "../common/macros.hpp"

constexpr let default_timer = 100ms;

def orders_service(AtomicBool is_running, String ws_uri,
                   String work_bind, String pub_bind)
    -> void {
  let ctx = make_context(1);
  let worker = make_pull(ctx, work_bind);
  let publisher = make_publisher(ctx, pub_bind);
  let wsock = make_websocket(ws_uri);

  // loop until ctrl-c or forever
  while (is_running) {
    this_thread::sleep_for(default_timer);
  }
  logger::info("got shutdown signal");

  // then stop the sink
  ctx->shutdown();
  logger::info("shutdown complete");
}
