export module voo:swapchain_and_stuff;
import :guards;
import :device_and_queue;
import :frame_sync_stuff;
import :offscreen;
import :queue;
import :single_cb;
import :swapchain;
import hai;
import vee;

namespace voo {
template <typename T, typename A>
concept is_fn_taking_const_ref = requires(T t, const A &a) { t(a); };

export class swapchain_and_stuff {
  vee::render_pass::type m_rp;

  voo::frame_sync_stuff m_sync {};
  voo::swapchain m_swc;
  hai::array<vee::framebuffer> m_fbs {};

  voo::single_cb m_cb;

public:
  swapchain_and_stuff(const device_and_queue &dq, vee::render_pass::type rp,
                      hai::array<vee::image_view::type> extras = {})
      : swapchain_and_stuff(dq.physical_device(), dq.surface(),
                            rp, dq.queue_family(),
                            traits::move(extras)) {}

  swapchain_and_stuff(vee::physical_device pd, vee::surface::type s,
                      vee::render_pass::type rp, unsigned qf,
                      hai::array<vee::image_view::type> extras = {})
      : m_rp{rp}
      , m_swc { pd, s }
      , m_cb { qf } {
    m_fbs.set_capacity(m_swc.count());
    for (auto i = 0; i < m_swc.count(); i++) {
      hai::array<vee::image_view::type> attachments { extras.size() + 1 };
      for (auto j = 0; j < extras.size(); j++) {
        attachments[j + 1] = extras[j];
      }
      attachments[0] = m_swc.image_view(i);
      m_fbs[i] = vee::create_framebuffer({
          .render_pass = m_rp,
          .attachments = traits::move(attachments),
          .extent = m_swc.extent(),
      });
    }
  }

  [[nodiscard]] constexpr const auto command_buffer() const { return m_cb.cb(); }
  [[nodiscard]] constexpr const auto extent() const { return m_swc.extent(); }
  [[nodiscard]] constexpr const auto framebuffer() const { return *m_fbs[m_swc.index()]; }

  [[nodiscard]] constexpr auto aspect() const {
    return static_cast<float>(extent().width) / static_cast<float>(extent().height);
  }

  void acquire_next_image() {
    m_sync.wait_and_reset_fence();
    m_swc.acquire_next_image(m_sync.img_available_sema());
  }

  auto render_pass_begin() const {
    return vee::render_pass_begin {
      .command_buffer = m_cb.cb(),
      .render_pass = m_rp,
      .framebuffer = framebuffer(),
      .extent = extent(),
    };
  }
  auto render_pass_begin(vee::render_pass_begin rpb) const {
    if (!rpb.command_buffer) rpb.command_buffer = m_cb.cb();
    rpb.render_pass = m_rp;
    rpb.framebuffer = framebuffer();
    rpb.extent = extent();
    return rpb;
  }
  auto cmd_render_pass(vee::render_pass_begin rpb) const {
    return voo::cmd_render_pass(render_pass_begin(traits::move(rpb)));
  }
  auto cmd_render_pass() const {
    return voo::cmd_render_pass(render_pass_begin());
  }

  void queue_submit(queue *q) { m_sync.queue_submit(q, m_cb.cb()); }
  void queue_present(queue *q) { m_swc.queue_present(q, m_sync.rnd_finished_sema()); }

  void queue_one_time_submit(queue *q, auto &&fn) {
    {
      voo::cmd_buf_one_time_submit ots { m_cb.cb() };
      fn();
    }
    queue_submit(q);
  }
};
} // namespace voo
