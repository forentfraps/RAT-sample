#include "web.h"
#include "db.h"
#include "netlib.h"

static void cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    MG_INFO(("New request to: [%.*s], body size: %lu", (int) hm->uri.len,
             hm->uri.ptr, (unsigned long) hm->body.len));
    if (mg_http_match_uri(hm, "/data")){
        mg_http_reply(c, 200, "", "Thank you!");
    }
  }
}

int main(void) {
  struct mg_mgr mgr;

  mg_mgr_init(&mgr);
  mg_log_set(3);
  mg_http_listen(&mgr, "0.0.0.0:8000", cb, NULL);

  for (;;) mg_mgr_poll(&mgr, 50);
  mg_mgr_free(&mgr);

  return 0;
}