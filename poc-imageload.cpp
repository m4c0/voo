#pragma leco tool

import hai;
import stubby;
import voo;

int main() {
  voo::device_and_queue dq { "poc-imageload" };

  auto cpool = dq.queue()->create_command_pool();

  struct bits {
    vee::command_buffer cb;
    voo::fence f { voo::fence::signaled {} };
    voo::queue * q;
  } data {
    .cb = cpool.allocate_primary_command_buffer(),
    .q  = dq.queue(),
  };
  stbi::load_from_resource("poc-imageload.png", &data, [](auto ptr, auto & img) {
    bits * b = static_cast<bits *>(ptr);

    b->f.wait_and_reset();

    voo::cmd_buf_one_time_submit::build(b->cb, [](auto cb) {
    });
    b->q->queue_submit({
      .fence = b->f,
      .command_buffer = b->cb,
    });
  });

  vee::device_wait_idle();
}
