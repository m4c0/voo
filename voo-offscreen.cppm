export module voo:offscreen;
import :mapmem;
import vee;

export namespace voo::offscreen {
  class colour_buffer {
    vee::image m_img;
    vee::device_memory m_mem;
    vee::image_view m_iv;

  public:
    colour_buffer(vee::physical_device pd, vee::extent ext) {
      m_img = vee::create_renderable_image(ext);
      m_mem = vee::create_local_image_memory(pd, *m_img);
      vee::bind_image_memory(*m_img, *m_mem);
      m_iv = vee::create_srgba_image_view(*m_img);
    }

    [[nodiscard]] auto image_view() const { return *m_iv; }
    [[nodiscard]] auto image() const { return *m_img; }
  };

  class depth_buffer {
    vee::image m_img;
    vee::device_memory m_mem;
    vee::image_view m_iv;

  public:
    depth_buffer(vee::physical_device pd, vee::extent ext) {
      m_img = vee::create_depth_image(ext);
      m_mem = vee::create_local_image_memory(pd, *m_img);
      vee::bind_image_memory(*m_img, *m_mem);
      m_iv = vee::create_srgba_image_view(*m_img);
    }

    [[nodiscard]] auto image_view() const { return *m_iv; }
    [[nodiscard]] auto image() const { return *m_img; }
  };

  class host_buffer {
    vee::buffer m_buf;
    vee::device_memory m_mem;

  public:
    host_buffer(vee::physical_device pd, vee::extent ext) {
      m_buf = vee::create_transfer_dst_buffer(ext.width * ext.height * 4);
      m_mem = vee::create_host_buffer_memory(pd, *m_buf);
      vee::bind_buffer_memory(*m_buf, *m_mem);
    }

    [[nodiscard]] auto buffer() const { return *m_buf; }
    [[nodiscard]] auto map() const { return voo::mapmem(*m_mem); }
  };
} // namespace voo::offscreen
