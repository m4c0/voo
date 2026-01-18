export module voo:update_thread;
import :fence;
import :guards;
import :queue;
import hai;
import sith;
import vee;

namespace voo {
export class update_thread : public sith::thread {
  voo::queue * m_q {};
  hai::fn<void, vee::command_buffer> m_fn {};

  // We can't share cmd pool resources (i.e. cmd bufs) between threads, so we
  // allocate our own.
  vee::command_pool m_cp;
  vee::command_buffer m_cb;

  voo::fence m_f { true };

protected:
  constexpr update_thread() = default;
  explicit update_thread(queue * q, hai::fn<void, vee::command_buffer> fn)
      : m_q{q}
      , m_fn { fn }
      , m_cp{vee::create_command_pool(q->queue_family())}
      , m_cb{vee::allocate_primary_command_buffer(*m_cp)} {}

  void run_once() {
    m_f.wait_and_reset();

    {
      cmd_buf_one_time_submit g { m_cb };
      m_fn(m_cb);
    }

    m_q->queue_submit({
        .fence = m_f,
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
