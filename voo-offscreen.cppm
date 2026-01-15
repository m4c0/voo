export module voo:offscreen;
import :images;
import :mapmem;
import vee;
import wagen;

using namespace wagen;

export namespace voo::offscreen {
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
    bound_image m_colour;
    bound_image m_depth;
    host_buffer m_host;

    // TODO: voo::framebuffer with rp+fb+ext - they are always used together
    vee::render_pass m_rp;
    vee::framebuffer m_fb;
    vee::extent m_ext;

  public:
    buffers(vee::physical_device pd, vee::extent ext, vee::format img)
        : m_colour { bound_image::create(ext, img, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT) }
        , m_depth { bound_image::create_depth(ext, 0) }
        , m_host { pd, ext }
        , m_rp { vee::create_render_pass({
          .attachments {{
            vee::create_colour_attachment(img, vee::image_layout_color_attachment_optimal),
            vee::create_depth_attachment(),
          }},
          .subpasses {{
            vee::create_subpass({
              .colours {{ vee::create_attachment_ref(0, vee::image_layout_color_attachment_optimal) }},
              .depth_stencil = create_attachment_ref(1, vee::image_layout_depth_stencil_attachment_optimal),
            }),
          }},
          .dependencies {{
            vee::create_colour_dependency(),
            vee::create_depth_dependency(),
          }},
        }) }
        , m_fb { vee::create_framebuffer({
              .render_pass = *m_rp,
              .attachments = { { *m_colour.iv, *m_depth.iv } },
              .extent = ext,
          }) }
        , m_ext { ext } {}

    [[nodiscard]] constexpr auto extent() const { return m_ext; }
    [[nodiscard]] constexpr auto framebuffer() const { return *m_fb; }
    [[nodiscard]] constexpr auto render_pass() const { return *m_rp; }

    [[nodiscard]] auto map_host() const { return m_host.map(); }

    void cmd_copy_to_host(vee::command_buffer cb) {
      vee::cmd_pipeline_barrier(cb, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_HOST_BIT, {
        .srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_HOST_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .image = *m_colour.img,
      });
      vee::cmd_copy_image_to_buffer(cb, {}, m_ext, *m_colour.img, m_host.buffer());
    }

    auto render_pass_begin(vee::render_pass_begin rpb) const {
      rpb.render_pass = render_pass();
      rpb.framebuffer = framebuffer();
      rpb.extent = extent();
      return rpb;
    }
  };
} // namespace voo::offscreen
