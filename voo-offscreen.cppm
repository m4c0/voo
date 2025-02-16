export module voo:offscreen;
import :mapmem;
import vee;

export namespace voo::offscreen {
  class colour_buffer {
    vee::image m_img;
    vee::device_memory m_mem;
    vee::image_view m_iv;

  public:
    constexpr colour_buffer() = default;
    colour_buffer(vee::physical_device pd, vee::extent ext, vee::format fmt, auto... usages) {
      m_img = vee::create_image(ext, fmt, vee::image_usage_colour_attachment, usages...);
      m_mem = vee::create_local_image_memory(pd, *m_img);
      vee::bind_image_memory(*m_img, *m_mem);
      m_iv = vee::create_image_view(*m_img, fmt);
    }

    [[nodiscard]] constexpr auto image_view() const { return *m_iv; }
    [[nodiscard]] constexpr auto image() const { return *m_img; }

    void cmd_copy_to_host(vee::command_buffer cb, vee::offset ofs, vee::extent ext, vee::buffer::type host) {
      vee::cmd_pipeline_barrier(cb, *m_img, vee::from_pipeline_to_host);
      vee::cmd_copy_image_to_buffer(cb, ofs, ext, *m_img, host);
    }
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
      m_iv = vee::create_depth_image_view(*m_img);
    }

    [[nodiscard]] constexpr auto image_view() const { return *m_iv; }
    [[nodiscard]] constexpr auto image() const { return *m_img; }
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

    [[nodiscard]] constexpr auto buffer() const { return *m_buf; }
    [[nodiscard]] auto map() const { return voo::mapmem(*m_mem); }
  };

  class buffers {
    colour_buffer m_colour;
    depth_buffer m_depth;
    host_buffer m_host;

    // TODO: voo::framebuffer with rp+fb+ext - they are always used together
    vee::render_pass m_rp;
    vee::framebuffer m_fb;
    vee::extent m_ext;

  public:
    buffers(vee::physical_device pd, vee::extent ext, vee::format img, vee::image_layout ly)
        : m_colour { pd, ext, img }
        , m_depth { pd, ext }
        , m_host { pd, ext }
        , m_rp { vee::create_render_pass(img, ly) }
        , m_fb { vee::create_framebuffer({
              .physical_device = pd,
              .render_pass = *m_rp,
              .attachments = { { m_colour.image_view(), m_depth.image_view() } },
              .extent = ext,
          }) }
        , m_ext { ext } {}

    [[nodiscard]] constexpr auto extent() const { return m_ext; }
    [[nodiscard]] constexpr auto framebuffer() const { return *m_fb; }
    [[nodiscard]] constexpr auto render_pass() const { return *m_rp; }

    [[nodiscard]] auto map_host() const { return m_host.map(); }

    void cmd_copy_to_host(vee::command_buffer cb) {
      m_colour.cmd_copy_to_host(cb, {}, m_ext, m_host.buffer());
    }

    auto render_pass_begin(vee::render_pass_begin rpb) {
      rpb.render_pass = render_pass();
      rpb.framebuffer = framebuffer();
      rpb.extent = extent();
      return rpb;
    }
  };
} // namespace voo::offscreen
