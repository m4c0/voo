export module voo:guards;
import :frame_sync_stuff;
import :queue;
import :swapchain;
import hai;
import vee;

namespace voo {
struct cb_deleter {
  void operator()(vee::command_buffer cb) { vee::end_cmd_buf(cb); }
};
struct rp_deleter {
  void operator()(vee::command_buffer cb) { vee::cmd_end_render_pass(cb); }
};

export class cmd_buf_one_time_submit {
  hai::value_holder<vee::command_buffer, cb_deleter> m_cb;

public:
  explicit cmd_buf_one_time_submit(vee::command_buffer cb) : m_cb{cb} {
    vee::begin_cmd_buf_one_time_submit(cb);
  }

  [[nodiscard]] constexpr auto operator*() const { return *m_cb; }

  static void build(vee::command_buffer cb, auto &&fn) {
    cmd_buf_one_time_submit pcb{cb};
    fn(pcb);
  }
};

export class cmd_buf_sim_use {
  hai::value_holder<vee::command_buffer, cb_deleter> m_cb;

public:
  explicit cmd_buf_sim_use(vee::command_buffer cb) : m_cb{cb} {
    vee::begin_cmd_buf_sim_use(cb);
  }

  [[nodiscard]] constexpr auto operator*() const { return *m_cb; }

  static void build(vee::command_buffer cb, auto &&fn) {
    cmd_buf_sim_use pcb{cb};
    fn(pcb);
  }
};

export class cmd_render_pass {
  hai::value_holder<vee::command_buffer, rp_deleter> m_cb;

public:
  explicit cmd_render_pass(const vee::render_pass_begin &rpb)
      : m_cb{rpb.command_buffer} {
    vee::cmd_begin_render_pass(rpb);
  }

  [[nodiscard]] constexpr auto operator*() const { return *m_cb; }

  static void build(const vee::render_pass_begin &rbp, auto &&fn) {
    cmd_render_pass pcb{rbp};
    fn(pcb);
  }
};

export class cmd_buf_render_pass_continue {
  hai::value_holder<vee::command_buffer, cb_deleter> m_cb;

public:
  explicit cmd_buf_render_pass_continue(vee::command_buffer cb,
                                        vee::render_pass::type rp)
      : m_cb{cb} {
    vee::begin_cmd_buf_render_pass_continue(cb, rp);
  }

  [[nodiscard]] constexpr auto operator*() const { return *m_cb; }

  static void build(vee::command_buffer cb, vee::render_pass::type rp,
                    auto &&fn) {
    cmd_buf_render_pass_continue pcb{cb, rp};
    fn(pcb);
  }
};

  export class present_guard {
    queue * m_q;
    swapchain * m_swc;
    frame_sync_stuff * m_sync;
  public:
    present_guard(queue * q, swapchain * sc, frame_sync_stuff * sync)
      : m_q { q }, m_swc { sc }, m_sync { sync }
    {
      sync->wait_and_reset_fence();
      sc->acquire_next_image(sync->img_available_sema());
    }
    ~present_guard() {
      m_swc->queue_present(m_q, m_sync->rnd_finished_sema());
    }
  };
} // namespace voo
