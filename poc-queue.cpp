#pragma leco tool

import hai;
import voo;

int main() {
  voo::device_and_queue dq { "poc-queue" };
  auto q = dq.queue();

  auto cpool = q->create_command_pool();
  auto cb = cpool.allocate_primary_command_buffer();

  voo::cmd_buf_one_time_submit::build(cb, [](auto cb) {});

  q->queue_submit({ .command_buffer = cb });

  vee::device_wait_idle();
}
