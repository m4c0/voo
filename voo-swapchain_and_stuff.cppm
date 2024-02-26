export module voo:swapchain_and_stuff;
import :guards;
import :device_and_queue;
import :queue;
import hai;
import vee;

namespace voo {
template <typename T, typename A>
concept is_fn_taking_const_ref = requires(T t, const A &a) { t(a); };

export class swapchain_and_stuff {
  // Depth buffer
  vee::image m_dimg;
  vee::device_memory m_dmem;
  vee::image_view m_div;

  // Sync stuff
  vee::semaphore m_img_available_sema = vee::create_semaphore();
  vee::semaphore m_rnd_finished_sema = vee::create_semaphore();
  vee::fence m_f = vee::create_fence_signaled();

  vee::swapchain m_swc;
  vee::extent m_ext;
  vee::render_pass::type m_rp;

  hai::array<vee::image_view> m_civs;
  hai::array<vee::framebuffer> m_fbs;

  vee::command_pool m_cp;
  vee::command_buffer m_cb;

  unsigned m_idx;

public:
  swapchain_and_stuff(const device_and_queue &dq)
      : swapchain_and_stuff(dq.physical_device(), dq.surface(),
                            dq.render_pass(), dq.queue_family()) {}
  swapchain_and_stuff(vee::physical_device pd, vee::surface::type s,
                      vee::render_pass::type rp, unsigned qf)
      : m_rp{rp} {
    m_dimg = vee::create_depth_image(pd, s);
    m_dmem = vee::create_local_image_memory(pd, *m_dimg);
    vee::bind_image_memory(*m_dimg, *m_dmem);
    m_div = vee::create_depth_image_view(*m_dimg);

    m_swc = vee::create_swapchain(pd, s);
    m_ext = vee::get_surface_capabilities(pd, s).currentExtent;

    auto swc_imgs = vee::get_swapchain_images(*m_swc);
    m_civs = hai::array<vee::image_view>{swc_imgs.size()};
    m_fbs = hai::array<vee::framebuffer>{swc_imgs.size()};

    for (auto i = 0; i < swc_imgs.size(); i++) {
      m_civs[i] = vee::create_rgba_image_view(swc_imgs[i], pd, s);
      m_fbs[i] = vee::create_framebuffer({
          .physical_device = pd,
          .surface = s,
          .render_pass = m_rp,
          .image_buffer = *m_civs[i],
          .depth_buffer = *m_div,
      });
    }

    m_cp = vee::create_command_pool(qf);
    m_cb = vee::allocate_primary_command_buffer(*m_cp);
  }

  [[nodiscard]] constexpr const auto command_buffer() const noexcept {
    return m_cb;
  }
  [[nodiscard]] constexpr const auto extent() const noexcept { return m_ext; }
  [[nodiscard]] constexpr const auto framebuffer() const noexcept {
    return *m_fbs[m_idx];
  }

  [[nodiscard]] constexpr auto aspect() const noexcept {
    return static_cast<float>(m_ext.width) / static_cast<float>(m_ext.height);
  }

  auto acquire_next_image() {
    vee::wait_and_reset_fence(*m_f);
    return m_idx = vee::acquire_next_image(*m_swc, *m_img_available_sema);
  }

  auto cmd_render_pass(vee::render_pass_begin rpb) const {
    rpb.render_pass = m_rp;
    rpb.framebuffer = framebuffer();
    rpb.extent = extent();
    return voo::cmd_render_pass(rpb);
  }
  auto cmd_render_pass(const cmd_buf_one_time_submit &pcb) const {
    return cmd_render_pass({
        .command_buffer = *pcb,
        .clear_color = {{0.1, 0.2, 0.3, 1.0}},
    });
  }
  auto cmd_buf_render_pass_continue(vee::command_buffer cb) const {
    return voo::cmd_buf_render_pass_continue(cb, m_rp, extent());
  }
  void cmd_buf_render_pass_continue(vee::command_buffer cb, auto &&fn) const {
    auto cbg = cmd_buf_render_pass_continue(cb);
    fn(cbg);
  }

  void queue_submit(queue &dq) {
    dq.queue_submit({
        .fence = *m_f,
        .command_buffer = m_cb,
        .wait_semaphore = *m_img_available_sema,
        .signal_semaphore = *m_rnd_finished_sema,
    });
  }
  void queue_present(queue &dq) {
    dq.queue_present({
        .swapchain = *m_swc,
        .wait_semaphore = *m_rnd_finished_sema,
        .image_index = m_idx,
    });
  }

  void queue_one_time_submit(queue &dq, auto &&fn) {
    fn(voo::cmd_buf_one_time_submit{m_cb});
    queue_submit(dq);
  }
};
} // namespace voo
