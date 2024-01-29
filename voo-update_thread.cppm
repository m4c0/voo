export module voo:update_thread;
import :device_and_queue;
import :fence;
import sith;
import vee;

namespace voo {
export class update_thread {
  voo::device_and_queue *m_dq;

protected:
  explicit constexpr update_thread(device_and_queue *dq) : m_dq{dq} {}

  virtual void build_cmd_buf(vee::command_buffer cb) = 0;

  void run(sith::thread *t) {
    // We can't share cmd pool resources (i.e. cmd bufs) between threads, so we
    // allocate our own.
    auto cp = vee::create_command_pool(m_dq->queue_family());
    auto cb = vee::allocate_primary_command_buffer(*cp);

    voo::fence f{voo::fence::signaled{}};
    while (!t->interrupted()) {
      f.wait_and_reset();

      build_cmd_buf(cb);

      m_dq->queue_submit({
          .fence = *f,
          .command_buffer = cb,
      });
    }

    // Wait until our submissions are done
    m_dq->device_wait_idle();
  }
};
} // namespace voo
