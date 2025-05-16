#pragma leco tool

import hai;
import jute;
import silog;
import stubby;
import voo;

struct bound_image {
  vee::image img;
  vee::device_memory mem;
  vee::image_view iv;
};

void load_image(jute::view name, voo::device_and_queue & dq, bound_image & bi, auto && callback) {
  stbi::load(name, nullptr, [&](auto ptr, auto & img) {
    unsigned w = img.width;
    unsigned h = img.height;
    unsigned sz = w * h * 4;
    voo::host_buffer host { dq.physical_device(), sz };
    {
      voo::memiter<unsigned char> c { host.memory() };
      for (auto i = 0; i < sz; i++) c[i] = (*img.data)[i];
    }

    constexpr const auto fmt = VK_FORMAT_R8G8B8A8_SRGB;
    vee::extent ext { w, h };
    bi.img = vee::create_image(ext, fmt);
    bi.mem = vee::create_local_image_memory(dq.physical_device(), *bi.img);
    vee::bind_image_memory(*bi.img, *bi.mem);
    bi.iv = vee::create_image_view(*bi.img, fmt);

    voo::fence f { false };
    auto cpool = dq.queue()->create_command_pool();
    auto cb = cpool.allocate_primary_command_buffer();

    voo::cmd_buf_one_time_submit::build(cb, [&](auto cb) {
      vee::cmd_pipeline_barrier(cb, *bi.img, vee::from_host_to_transfer);
      vee::cmd_copy_buffer_to_image(cb, ext, host.buffer(), *bi.img);
      vee::cmd_pipeline_barrier(cb, *bi.img, vee::from_transfer_to_fragment);
    });
    dq.queue()->queue_submit({
      .fence = f,
      .command_buffer = cb,
    });

    f.wait();
    callback();
  });
}

int main() {
  voo::device_and_queue dq { "poc-imageload" };

  bound_image img {};
  load_image("poc-imageload.png", dq, img, [] {
    silog::log(silog::info, "Image loaded");
  });

  vee::device_wait_idle();
}
