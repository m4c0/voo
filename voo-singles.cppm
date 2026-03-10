export module voo:singles;
import :device_and_queue;
import :queue;
import vee;

namespace voo {
  export class single_cb {
    vee::command_pool m_cp;
    vee::command_buffer m_cb;
  public:
    explicit single_cb(bool primary = true)
      : single_cb { voo::queue::universal()->queue_family(), primary }
    {}
    explicit single_cb(unsigned qf, bool primary = true) :
      m_cp { vee::create_command_pool(qf) }
    , m_cb {
      primary
        ? vee::allocate_primary_command_buffer(*m_cp)
        : vee::allocate_secondary_command_buffer(*m_cp)
    }
    {}

    [[nodiscard]] constexpr auto cb() const { return m_cb; }
  };

  export class single_dset {
    vee::descriptor_set_layout m_dsl;
    vee::descriptor_pool m_pool;
    vee::descriptor_set m_dset;

  public:
    constexpr single_dset() = default;
    single_dset(auto dsl_bind, auto pool_size)
      : m_dsl { vee::create_descriptor_set_layout({ dsl_bind }) }
      , m_pool { vee::create_descriptor_pool(1, { pool_size }) }
      , m_dset { vee::allocate_descriptor_set(*m_pool, *m_dsl) } {}

    [[nodiscard]] constexpr auto descriptor_set() const { return m_dset; }
    [[nodiscard]] constexpr auto descriptor_set_layout() const { return *m_dsl; }
  };
  export class single_frag_dset : public single_dset {
  public:
    constexpr single_frag_dset() = default;
    explicit single_frag_dset(unsigned b_count) : single_dset {
      vee::dsl_fragment_sampler(),
      vee::combined_image_sampler(b_count),
    } {};
  };

  export inline auto single_att_render_pass(vee::physical_device pd, vee::surface::type s) {
    return vee::create_render_pass({
      .attachments {{
        vee::colour_attachment(pd, s),
      }},
      .subpasses {{
        vee::subpass({
          .colours {{ vee::attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) }},
        }),
      }},
      .dependencies {{
        vee::colour_dependency(),
      }},
    });
  }
  export inline auto single_att_render_pass(const voo::device_and_queue & dq) {
    return single_att_render_pass(dq.physical_device(), dq.surface());
  }

  export inline auto single_att_depth_render_pass(vee::physical_device pd, vee::surface::type s) {
    return vee::create_render_pass({
      .attachments {{
        vee::colour_attachment(pd, s),
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
    });
  }
  export inline auto single_att_depth_render_pass(const voo::device_and_queue & dq) {
    return single_att_depth_render_pass(dq.physical_device(), dq.surface());
  }
}
