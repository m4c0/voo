export module voo:updater;
import :guards;
import :queue;
import :update_thread;
import traits;
import vee;

export namespace voo {
template <typename T> class updater_thread : public update_thread {
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
template <typename T> updater_thread(queue *, T) -> updater_thread<T>;
template <typename T>
updater_thread(queue *, T, void (*)(T *)) -> updater_thread<T>;
template <typename T>
updater_thread(queue *, void (*)(T *), auto &&...) -> updater_thread<T>;
} // namespace voo
