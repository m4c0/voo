export module voo:offscreen;
import :buffers;
import :images;
import :mapmem;
import vee;
import wagen;

using namespace wagen;

export namespace voo::offscreen {
  class buffers {
    bound_image m_colour;
    bound_image m_depth;
    bound_buffer m_host;

    // TODO: voo::framebuffer with rp+fb+ext - they are always used together
    vee::render_pass m_rp;
    vee::framebuffer m_fb;
    vee::extent m_ext;

  public:
    buffers(VkExtent2D ext, VkFormat img)
        : m_colour { bound_image::create(ext, img, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT) }
        , m_depth { bound_image::create_depth(ext, 0) }
        , m_host { bound_buffer::create_from_host(ext.width * ext.height * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT) }
        , m_rp { vee::create_render_pass({
          .attachments {{
            vee::colour_attachment(img, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
            vee::depth_attachment(),
          }},
          .subpasses {{
            vee::subpass({
              .colours {{ vee::attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) }},
              .depth_stencil = vee::attachment_ref(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),
            }),
          }},
          .dependencies {{
            vee::colour_dependency(),
            vee::depth_dependency(),
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

    [[nodiscard]] auto map_host() const { return voo::mapmem(*m_host.memory); }

    void cmd_copy_to_host(vee::command_buffer cb) {
      vee::cmd_pipeline_barrier(cb, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_HOST_BIT, {
        .srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_HOST_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .image = *m_colour.img,
      });
      vee::cmd_copy_image_to_buffer(cb, {}, m_ext, *m_colour.img, *m_host.buffer);
    }

    auto render_pass_begin(vee::render_pass_begin rpb) const {
      rpb.render_pass = render_pass();
      rpb.framebuffer = framebuffer();
      rpb.extent = extent();
      return rpb;
    }
  };
} // namespace voo::offscreen
