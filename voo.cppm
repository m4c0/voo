export module voo;
export import :casein_thread;
export import :command_pool;
export import :device_and_queue;
export import :fence;
export import :guards;
export import :h2l_buffer;
export import :h2l_image;
export import :h2l_yuv_image;
export import :host_buffer;
export import :mapmem;
export import :one_quad;
export import :one_quad_render;
export import :sires_image;
export import :shader;
export import :swapchain_and_stuff;
export import :queue;
export import :update_thread;
export import :updater;
import jute;
import traits;
import vee;

#pragma leco add_impl casein_thread

// TODO: move these back to their parts once clang on Windows accept it
export namespace voo {
template <typename T> class updater : public update_thread {
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
  constexpr updater() = default;
  updater(queue *q, T data) : update_thread{q}, m_data{traits::move(data)} {}
  updater(queue *q, T data, void (*fn)(T *))
      : update_thread{q}
      , m_data{traits::move(data)}
      , m_fn{fn} {}
  updater(queue *q, void (*fn)(T *), auto &&...args)
      : update_thread{q}
      , m_data{args...}
      , m_fn{fn} {}

  [[nodiscard]] constexpr T &data() { return m_data; }
  [[nodiscard]] constexpr const T &data() const { return m_data; }

  using update_thread::run;
  using update_thread::run_once;
};
template <typename T> updater(queue *, T) -> updater<T>;
template <typename T> updater(queue *, T, void (*)(T *)) -> updater<T>;
template <typename T> updater(queue *, void (*)(T *), auto &&...) -> updater<T>;
} // namespace voo
