#pragma leco tool

import hai;
import stubby;
import voo;

int main() {
  voo::device_and_queue dq { "poc-imageload" };

  stbi::load("poc-imageload.png", nullptr, [&](auto ptr, auto & img) {
    unsigned sz = img.width * img.height * 4;
    voo::host_buffer host { dq.physical_device(), sz };
    {
      voo::memiter<unsigned char> c { host.memory() };
      for (auto i = 0; i < sz; i++) c[i] = (*img.data)[i];
    }

    voo::fence f { false };
    auto cpool = dq.queue()->create_command_pool();
    auto cb = cpool.allocate_primary_command_buffer();

    voo::cmd_buf_one_time_submit::build(cb, [&](auto cb) {
      // vee::cmd_pipeline_barrier(cb, img, vee::from_host_to_transfer);
      // vee::cmd_copy_buffer_to_image(cb, extent(), buffer(), img);
      // vee::cmd_pipeline_barrier(cb, img, vee::from_transfer_to_fragment);
    });
    dq.queue()->queue_submit({
      .fence = f,
      .command_buffer = cb,
    });

    f.wait();
  });

  vee::device_wait_idle();
}
