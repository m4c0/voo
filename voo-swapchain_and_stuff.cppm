export module voo:swapchain_and_stuff;
import :guards;
import :device_and_queue;
import :frame_sync_stuff;
import :offscreen;
import :queue;
import :singles;
import :swapchain;
import hai;
import vee;

namespace voo {
template <typename T, typename A>
concept is_fn_taking_const_ref = requires(T t, const A &a) { t(a); };

export class swapchain_and_stuff {
  vee::render_pass::type m_rp;

  voo::swapchain m_swc;
  hai::array<vee::framebuffer> m_fbs;

  voo::single_cb m_cb;

public:
  swapchain_and_stuff(const device_and_queue &dq, vee::render_pass::type rp, auto *... ivs) :
    swapchain_and_stuff(dq.physical_device(), dq.surface(), rp, dq.queue_family(), ivs...)
  {}

  swapchain_and_stuff(vee::physical_device pd, vee::surface::type s,
                      vee::render_pass::type rp, unsigned qf,
                      auto *... ivs)
      : m_rp{rp}
      , m_swc { pd, s }
      , m_fbs { m_swc.create_framebuffers(rp, ivs...) }
      , m_cb { qf } {
  }

  [[nodiscard]] constexpr const auto command_buffer() const { return m_cb.cb(); }
  [[nodiscard]] constexpr const auto extent() const { return m_swc.extent(); }
  [[nodiscard]] constexpr const auto framebuffer() const { return *m_fbs[m_swc.index()]; }

  [[nodiscard]] constexpr auto aspect() const {
    return static_cast<float>(extent().width) / static_cast<float>(extent().height);
  }

  void acquire_next_image() {
    m_swc.acquire_next_image();
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

  void queue_submit() { m_swc.queue_submit(m_cb.cb()); }
  void queue_present() { m_swc.queue_present(); }

  void queue_one_time_submit(auto &&fn) {
    {
      voo::cmd_buf_one_time_submit ots { m_cb.cb() };
      fn();
    }
    queue_submit();
  }
};
} // namespace voo
