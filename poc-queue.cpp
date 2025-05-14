#pragma leco tool

import hai;
import voo;

int main() {
  voo::device_and_queue dq { "poc-queue" };
  auto q = dq.queue();

  auto cpool = q->create_command_pool();

  vee::command_buffer cb = cpool.allocate_primary_command_buffer();
  voo::fence f {{}};

  for (auto i = 0; i < 5; i++) {
    f.wait_and_reset();

    voo::cmd_buf_one_time_submit::build(cb, [](auto cb) {});
    q->queue_submit({
      .fence = f,
      .command_buffer = cb,
    });
  }

  vee::device_wait_idle();
}
