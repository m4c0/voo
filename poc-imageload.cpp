#pragma leco tool

import hai;
import stubby;
import voo;

int main() {
  voo::device_and_queue dq { "poc-imageload" };

  vee::image m_img;
  vee::device_memory m_mem;
  vee::image_view m_iv;

  stbi::load("poc-imageload.png", nullptr, [&](auto ptr, auto & img) {
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
    m_img = vee::create_image(ext, fmt);
    m_mem = vee::create_local_image_memory(dq.physical_device(), *m_img);
    vee::bind_image_memory(*m_img, *m_mem);
    m_iv = vee::create_image_view(*m_img, fmt);

    voo::fence f { false };
    auto cpool = dq.queue()->create_command_pool();
    auto cb = cpool.allocate_primary_command_buffer();

    voo::cmd_buf_one_time_submit::build(cb, [&](auto cb) {
      vee::cmd_pipeline_barrier(cb, *m_img, vee::from_host_to_transfer);
      vee::cmd_copy_buffer_to_image(cb, ext, host.buffer(), *m_img);
      vee::cmd_pipeline_barrier(cb, *m_img, vee::from_transfer_to_fragment);
    });
    dq.queue()->queue_submit({
      .fence = f,
      .command_buffer = cb,
    });

    f.wait();
  });

  vee::device_wait_idle();
}
