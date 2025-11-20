export module voo:frame_sync_stuff;
import :queue;
import vee;

export namespace voo {
  class frame_sync_stuff {
    vee::semaphore m_img_available_sema = vee::create_semaphore();
    vee::semaphore m_rnd_finished_sema = vee::create_semaphore();
    vee::fence m_f = vee::create_fence_signaled();

  public:
    [[nodiscard]] constexpr auto img_available_sema() const { return *m_img_available_sema; }
    [[nodiscard]] constexpr auto rnd_finished_sema() const { return *m_rnd_finished_sema; }

    void queue_submit(vee::command_buffer cb) const {
      queue::submit({
          .fence = *m_f,
          .command_buffer = cb,
          .wait_semaphore = *m_img_available_sema,
          .signal_semaphore = *m_rnd_finished_sema,
      });
    }

    void wait_and_reset_fence() const { vee::wait_and_reset_fence(*m_f); }
  };
}
