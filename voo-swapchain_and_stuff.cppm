export module voo:swapchain_and_stuff;
import :guards;
import :device_and_queue;
import :frame_sync_stuff;
import :offscreen;
import :queue;
import :single_cb;
import hai;
import vee;

namespace voo {
template <typename T, typename A>
concept is_fn_taking_const_ref = requires(T t, const A &a) { t(a); };

export class swapchain_and_stuff {
  voo::frame_sync_stuff m_sync;

  vee::render_pass::type m_rp;
  vee::extent m_ext;
  offscreen::depth_buffer m_depth;

  vee::swapchain m_swc;
  voo::single_cb m_cb;

  hai::array<vee::image_view> m_civs;
  hai::array<vee::framebuffer> m_fbs;

  unsigned m_idx;

public:
  explicit swapchain_and_stuff(const device_and_queue &dq,
                               hai::array<vee::image_view::type> extras = {})
      : swapchain_and_stuff(dq, dq.render_pass(), traits::move(extras)) {}

  swapchain_and_stuff(const device_and_queue &dq, vee::render_pass::type rp,
                      hai::array<vee::image_view::type> extras = {})
      : swapchain_and_stuff(dq.physical_device(), dq.surface(),
                            rp, dq.queue_family(),
                            traits::move(extras)) {}

  swapchain_and_stuff(vee::physical_device pd, vee::surface::type s,
                      vee::render_pass::type rp, unsigned qf,
                      hai::array<vee::image_view::type> extras = {})
      : m_rp{rp}
      , m_ext { extent_of(pd, s) }
      , m_depth { pd, m_ext }
      , m_swc { vee::create_swapchain(pd, s) }
      , m_cb { qf } {
    auto swc_imgs = vee::get_swapchain_images(*m_swc);
    m_civs = hai::array<vee::image_view>{swc_imgs.size()};
    m_fbs = hai::array<vee::framebuffer>{swc_imgs.size()};

    for (auto i = 0; i < swc_imgs.size(); i++) {
      m_civs[i] = vee::create_image_view_for_surface(swc_imgs[i], pd, s);
    }

    for (auto i = 0; i < swc_imgs.size(); i++) {
      hai::array<vee::image_view::type> attachments { extras.size() + 2 };
      for (auto j = 0; j < extras.size(); j++) {
        attachments[j + 1] = extras[j];
      }
      attachments[0] = *m_civs[i];
      attachments[extras.size() + 1] = m_depth.image_view();

      m_fbs[i] = vee::create_framebuffer({
          .physical_device = pd,
          .surface = s,
          .render_pass = m_rp,
          .attachments = traits::move(attachments),
      });
    }
  }

  [[nodiscard]] constexpr const auto command_buffer() const { return m_cb.cb(); }
  [[nodiscard]] constexpr const auto extent() const { return m_ext; }
  [[nodiscard]] constexpr const auto framebuffer() const { return *m_fbs[m_idx]; }

  [[nodiscard]] constexpr auto aspect() const {
    return static_cast<float>(m_ext.width) / static_cast<float>(m_ext.height);
  }

  auto acquire_next_image() {
    m_sync.wait_and_reset_fence();
    return m_idx = vee::acquire_next_image(*m_swc, m_sync.img_available_sema());
  }

  auto render_pass_begin(vee::render_pass_begin rpb) const {
    rpb.render_pass = m_rp;
    rpb.framebuffer = framebuffer();
    rpb.extent = extent();
    return rpb;
  }
  auto cmd_render_pass(vee::render_pass_begin rpb) const {
    return voo::cmd_render_pass(render_pass_begin(traits::move(rpb)));
  }

  void queue_submit(queue *q) { m_sync.queue_submit(q, m_cb.cb()); }
  void queue_present(queue *q) {
    q->queue_present({
        .swapchain = *m_swc,
        .wait_semaphore = m_sync.rnd_finished_sema(),
        .image_index = m_idx,
    });
  }

  void queue_one_time_submit(queue *q, auto &&fn) {
    fn(voo::cmd_buf_one_time_submit{m_cb.cb()});
    queue_submit(q);
  }
};
} // namespace voo
