export module voo:update_thread;
import :device_and_queue;
import :fence;
import :guards;
import :queue;
import sith;
import traits;
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

export template <typename T> class updater_thread : public update_thread {
  T m_data;
  void (*m_fn)(T *){};

  void build_cmd_buf(vee::command_buffer cb) override {
    update_data(&m_data);

    cmd_buf_one_time_submit pcb{cb};
    m_data.setup_copy(cb);
  }

protected:
  virtual void update_data(T *data) { m_fn(data); }

public:
  updater_thread(queue *q, T data)
      : update_thread{q}
      , m_data{traits::move(data)} {}
  updater_thread(queue *q, T data, void (*fn)(T *))
      : update_thread{q}
      , m_data{traits::move(data)}
      , m_fn{fn} {}
  updater_thread(queue *q, void (*fn)(T *), auto &&...args)
      : update_thread{q}
      , m_data{args...}
      , m_fn{fn} {}

  [[nodiscard]] constexpr T &data() noexcept { return m_data; }
  [[nodiscard]] constexpr const T &data() const noexcept { return m_data; }

  using update_thread::run;
  using update_thread::run_once;
};
export template <typename T> updater_thread(queue *, T) -> updater_thread<T>;
export template <typename T>
updater_thread(queue *, T, void (*)(T *)) -> updater_thread<T>;
export template <typename T>
updater_thread(queue *, void (*)(T *), auto &&...) -> updater_thread<T>;
} // namespace voo
