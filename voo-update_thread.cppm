export module voo:update_thread;
import :fence;
import :queue;
import sith;
import vee;

namespace voo {
export class update_thread : public sith::thread {
  voo::queue *m_q;

  // We can't share cmd pool resources (i.e. cmd bufs) between threads, so we
  // allocate our own.
  vee::command_pool m_cp;
  vee::command_buffer m_cb;

  voo::fence m_f{voo::fence::signaled{}};

protected:
  explicit update_thread(queue *q)
      : m_q{q}
      , m_cp{vee::create_command_pool(q->queue_family())}
      , m_cb{vee::allocate_primary_command_buffer(*m_cp)} {}

  virtual void build_cmd_buf(vee::command_buffer cb) = 0;

  void run_once() {
    m_f.wait_and_reset();

    build_cmd_buf(m_cb);

    m_q->queue_submit({
        .fence = *m_f,
        .command_buffer = m_cb,
    });
  }

  void run(sith::thread *t) {
    while (!t->interrupted()) {
      run_once();
    }

    // Wait until our submissions are done
    m_q->device_wait_idle();
  }

  void run() override { run(this); }
};
} // namespace voo
