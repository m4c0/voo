export module voo:swapchain_and_stuff;
import :guards;
import :device_and_queue;
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
  vee::render_pass m_rp;

  hai::array<vee::image_view> m_civs;
  hai::array<vee::framebuffer> m_fbs;

  vee::command_buffer m_cb;

  unsigned m_idx;

public:
  swapchain_and_stuff(const device_and_queue &dq)
      : swapchain_and_stuff(dq.physical_device(), dq.surface()) {
    m_cb = vee::allocate_primary_command_buffer(dq.command_pool());
  }
  swapchain_and_stuff(vee::physical_device pd, vee::surface::type s) {
    m_dimg = vee::create_depth_image(pd, s);
    m_dmem = vee::create_local_image_memory(pd, *m_dimg);
    vee::bind_image_memory(*m_dimg, *m_dmem);
    m_div = vee::create_depth_image_view(*m_dimg);

    m_swc = vee::create_swapchain(pd, s);
    m_ext = vee::get_surface_capabilities(pd, s).currentExtent;
    m_rp = vee::create_render_pass(pd, s);

    auto swc_imgs = vee::get_swapchain_images(*m_swc);
    m_civs = hai::array<vee::image_view>{swc_imgs.size()};
    m_fbs = hai::array<vee::framebuffer>{swc_imgs.size()};

    for (auto i = 0; i < swc_imgs.size(); i++) {
      m_civs[i] = vee::create_rgba_image_view(swc_imgs[i], pd, s);
      m_fbs[i] = vee::create_framebuffer({
          .physical_device = pd,
          .surface = s,
          .render_pass = *m_rp,
          .image_buffer = *m_civs[i],
          .depth_buffer = *m_div,
      });
    }
  }

  [[nodiscard]] constexpr const auto extent() const noexcept { return m_ext; }
  [[nodiscard]] constexpr const auto framebuffer() const noexcept {
    return *m_fbs[m_idx];
  }
  [[nodiscard]] constexpr const auto render_pass() const noexcept {
    return *m_rp;
  }

  [[nodiscard]] constexpr auto aspect() const noexcept {
    return static_cast<float>(m_ext.width) / static_cast<float>(m_ext.height);
  }

  auto acquire_next_image() noexcept {
    vee::wait_and_reset_fence(*m_f);
    return m_idx = vee::acquire_next_image(*m_swc, *m_img_available_sema);
  }

  auto cmd_render_pass(vee::render_pass_begin rpb) const noexcept {
    rpb.render_pass = render_pass();
    rpb.framebuffer = framebuffer();
    rpb.extent = extent();
    return voo::cmd_render_pass(rpb);
  }
  auto cmd_render_pass(const cmd_buf_one_time_submit &pcb) const noexcept {
    return cmd_render_pass({
        .command_buffer = *pcb,
        .clear_color = {{0.1, 0.2, 0.3, 1.0}},
    });
  }
  auto cmd_buf_render_pass_continue(vee::command_buffer cb) const noexcept {
    return voo::cmd_buf_render_pass_continue(cb, render_pass(), extent());
  }
  void cmd_buf_render_pass_continue(vee::command_buffer cb,
                                    auto &&fn) const noexcept {
    auto cbg = cmd_buf_render_pass_continue(cb);
    fn(cbg);
  }

  void queue_submit(vee::queue q, vee::command_buffer cb) const noexcept {
    vee::queue_submit({
        .queue = q,
        .fence = *m_f,
        .command_buffer = cb,
        .wait_semaphore = *m_img_available_sema,
        .signal_semaphore = *m_rnd_finished_sema,
    });
  }
  void queue_submit(const device_and_queue &dq,
                    vee::command_buffer cb) const noexcept {
    queue_submit(dq.queue(), cb);
  }

  void queue_submit(const device_and_queue &dq) const noexcept {
    queue_submit(dq.queue(), m_cb);
  }

  void queue_present(vee::queue q) const noexcept {
    vee::queue_present({
        .queue = q,
        .swapchain = *m_swc,
        .wait_semaphore = *m_rnd_finished_sema,
        .image_index = m_idx,
    });
  }
  void queue_present(const device_and_queue &dq) const noexcept {
    queue_present(dq.queue());
  }

  void one_time_submit(
      const device_and_queue &dq, vee::command_buffer cb,
      is_fn_taking_const_ref<cmd_buf_one_time_submit> auto fn) const {
    {
      cmd_buf_one_time_submit pcb{cb};
      fn(pcb);
    }
    queue_submit(dq, cb);
  }
  void one_time_submit(
      const device_and_queue &dq,
      is_fn_taking_const_ref<cmd_buf_one_time_submit> auto fn) const {
    one_time_submit(dq, m_cb, fn);
  }
};
} // namespace voo
