#include "src/tinycoffee.h"

int main(int argc, char ** argv) {

  tc_config config;
  tc_config_init(&config, "tico", 640, 380);
  tc_init(&config);

  while (!tc_should_close()) {
    tc_poll_events();
    tc_scripting_wren_update();

    tc_begin_draw();
    tc_scripting_wren_draw();
    tc_end_draw();
  }

  tc_terminate();

  return 0;
}