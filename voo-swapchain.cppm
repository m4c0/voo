export module voo:swapchain;
import :device_and_queue;
import :guards;
import :queue;
import :frame_sync_stuff;
import hai;
import traits;
import vee;

using traits::is_callable;

export namespace voo {
  class swapchain {
    voo::frame_sync_stuff m_sync {};
    vee::swapchain m_swc;
    hai::array<vee::image::type> m_imgs;
    hai::array<vee::image_view> m_civs;
    vee::extent m_ext;
    unsigned m_idx;

  public:
    struct render_pass_pair {
      vee::framebuffer fb;
      vee::render_pass_begin rpb;
    };

    explicit swapchain(const device_and_queue & dq, bool vsync = true)
      : swapchain { dq.physical_device(), dq.surface(), vsync }
    {}

    swapchain(vee::physical_device pd, vee::surface::type s, bool vsync = true)
      : m_swc { vee::create_swapchain(pd, s, vsync) }
      , m_imgs { vee::get_swapchain_images(*m_swc) }
      , m_ext { extent_of(pd, s) }
    {
      m_civs = hai::array<vee::image_view> { m_imgs.size() };

      for (auto i = 0; i < m_imgs.size(); i++) {
        m_civs[i] = vee::create_image_view_for_surface(m_imgs[i], pd, s);
      }
    }

    [[nodiscard]] constexpr auto count() const { return m_civs.size(); }
    [[nodiscard]] constexpr auto extent() const { return m_ext; }
    [[nodiscard]] constexpr auto image(unsigned i) const { return m_imgs[i]; }
    [[nodiscard]] constexpr auto image_view(unsigned i) const { return *m_civs[i]; }
    [[nodiscard]] constexpr auto index() const { return m_idx; }

    [[nodiscard]] constexpr auto aspect() const {
      return static_cast<float>(extent().width) / static_cast<float>(extent().height);
    }

    [[nodiscard]] constexpr auto create_framebuffer(unsigned idx, const vee::render_pass::type rp, auto... ivs) const {
      return vee::create_framebuffer({
        .render_pass = rp,
        .attachments {{ image_view(idx), ivs... }},
        .extent = extent(),
      });
    }
    [[nodiscard]] constexpr auto create_framebuffers(const vee::render_pass::type rp, auto... ivs) const {
      hai::array<vee::framebuffer> res { count() };
      for (auto i = 0; i < res.size(); i++) {
        res[i] = create_framebuffer(i, rp, ivs...);
      }
      return res;
    }

    [[nodiscard]] constexpr auto create_pairs(vee::render_pass_begin rpb) {
      hai::array<render_pass_pair> res { count() };
      for (auto i = 0; i < res.size(); i++) {
        res[i].fb = create_framebuffer(i, rpb.render_pass);

        rpb.framebuffer = *(res[i].fb);
        rpb.extent = m_ext;
        res[i].rpb = rpb;
      }
      return res;
    }

    void acquire_next_image() {
      m_sync.wait_and_reset_fence();
      m_idx = vee::acquire_next_image(*m_swc, m_sync.img_available_sema());
    }

    void queue_submit(vee::command_buffer cb) {
      m_sync.queue_submit(cb);
    }

    void queue_present() {
      queue::present({
        .swapchain = *m_swc,
        .wait_semaphore = m_sync.rnd_finished_sema(),
        .image_index = m_idx,
      });
    }

    void queue_one_time_submit(vee::command_buffer cb, auto && fn) {
      {
        voo::cmd_buf_one_time_submit ots { cb };
        fn();
      }
      queue_submit(cb);
    }
  };
}
