#include "web.h"
#include "db.h"
#include "netlib.h"
#include "bs_config.h"
struct db* db = NULL;

static void web_runtime(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        MG_INFO(("New request to: [%.*s], body size: %lu", (int) hm->uri.len,
                                                                 hm->uri.ptr, 
                                                 (unsigned long) hm->body.len));
        if (mg_http_match_uri(hm, "/data")){
            char* buf = web_print_db(db);
            mg_http_reply(c, 200, "","%s",buf);
            free(buf);
        }
    }
}

void* web_main(void* wa) {
  
    struct mg_mgr mgr;
    struct web_arg* _wa = wa;
    int* termination = _wa->termination;
    db = _wa->db;
    mg_mgr_init(&mgr);

    #ifdef DEBUG
    mg_log_set(3);
    #endif
    
    mg_http_listen(&mgr, "0.0.0.0:8000", web_runtime, NULL);

    for (;*termination != 1;){
        mg_mgr_poll(&mgr, 50);
    };
    mg_mgr_free(&mgr);
}