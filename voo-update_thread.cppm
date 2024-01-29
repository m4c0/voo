export module voo:update_thread;
import :device_and_queue;
import :fence;
import sith;
import vee;

namespace voo {
// TODO: better naming, since this isn't a thread
export class update_thread {
  voo::device_and_queue *m_dq;

  // We can't share cmd pool resources (i.e. cmd bufs) between threads, so we
  // allocate our own.
  vee::command_pool m_cp = vee::create_command_pool(m_dq->queue_family());
  vee::command_buffer m_cb = vee::allocate_primary_command_buffer(*m_cp);

  voo::fence m_f{voo::fence::signaled{}};

protected:
  explicit update_thread(device_and_queue *dq) : m_dq{dq} {}

  virtual void build_cmd_buf(vee::command_buffer cb) = 0;

  void run_once() {
    m_f.wait_and_reset();

    build_cmd_buf(m_cb);

    m_dq->queue_submit({
        .fence = *m_f,
        .command_buffer = m_cb,
    });
  }

  void run(sith::thread *t) {
    while (!t->interrupted()) {
      run_once();
    }

    // Wait until our submissions are done
    m_dq->device_wait_idle();
  }
};
} // namespace voo
