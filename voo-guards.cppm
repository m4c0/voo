export module voo:guards;
import vee;

namespace voo {
export class cmd_buf_one_time_submit {
  vee::command_buffer m_cb;

public:
  explicit cmd_buf_one_time_submit(vee::command_buffer cb) : m_cb{cb} {
    vee::begin_cmd_buf_one_time_submit(cb);
  }
  ~cmd_buf_one_time_submit() { vee::end_cmd_buf(m_cb); }

  cmd_buf_one_time_submit(const cmd_buf_one_time_submit &) = delete;
  cmd_buf_one_time_submit(cmd_buf_one_time_submit &&) = delete;
  cmd_buf_one_time_submit &operator=(const cmd_buf_one_time_submit &) = delete;
  cmd_buf_one_time_submit &operator=(cmd_buf_one_time_submit &&) = delete;

  [[nodiscard]] constexpr auto operator*() const noexcept { return m_cb; }

  static void build(vee::command_buffer cb, auto &&fn) {
    cmd_buf_one_time_submit pcb{cb};
    fn(pcb);
  }
};

export class cmd_buf_sim_use {
  vee::command_buffer m_cb;

public:
  explicit cmd_buf_sim_use(vee::command_buffer cb) : m_cb{cb} {
    vee::begin_cmd_buf_sim_use(cb);
  }
  ~cmd_buf_sim_use() { vee::end_cmd_buf(m_cb); }

  cmd_buf_sim_use(const cmd_buf_sim_use &) = delete;
  cmd_buf_sim_use(cmd_buf_sim_use &&) = delete;
  cmd_buf_sim_use &operator=(const cmd_buf_sim_use &) = delete;
  cmd_buf_sim_use &operator=(cmd_buf_sim_use &&) = delete;

  [[nodiscard]] constexpr auto operator*() const noexcept { return m_cb; }

  static void build(vee::command_buffer cb, auto &&fn) {
    cmd_buf_sim_use pcb{cb};
    fn(pcb);
  }
};

export class cmd_render_pass {
  vee::command_buffer m_cb;

public:
  explicit cmd_render_pass(const vee::render_pass_begin &rpb)
      : m_cb{rpb.command_buffer} {
    vee::cmd_begin_render_pass(rpb);
  }
  ~cmd_render_pass() { vee::cmd_end_render_pass(m_cb); }

  cmd_render_pass(const cmd_render_pass &) = delete;
  cmd_render_pass(cmd_render_pass &&) = delete;
  cmd_render_pass &operator=(const cmd_render_pass &) = delete;
  cmd_render_pass &operator=(cmd_render_pass &&) = delete;

  [[nodiscard]] constexpr auto operator*() const noexcept { return m_cb; }

  static void build(const vee::render_pass_begin &rbp, auto &&fn) {
    cmd_render_pass pcb{rbp};
    fn(pcb);
  }
};

export class cmd_buf_render_pass_continue {
  vee::command_buffer m_cb;

public:
  explicit cmd_buf_render_pass_continue(vee::command_buffer cb,
                                        vee::render_pass::type rp)
      : m_cb{cb} {
    vee::begin_cmd_buf_render_pass_continue(m_cb, rp);
  }
  ~cmd_buf_render_pass_continue() { vee::end_cmd_buf(m_cb); }

  cmd_buf_render_pass_continue(const cmd_buf_render_pass_continue &) = delete;
  cmd_buf_render_pass_continue(cmd_buf_render_pass_continue &&) = delete;
  cmd_buf_render_pass_continue &
  operator=(const cmd_buf_render_pass_continue &) = delete;
  cmd_buf_render_pass_continue &
  operator=(cmd_buf_render_pass_continue &&) = delete;

  [[nodiscard]] constexpr auto operator*() const noexcept { return m_cb; }
};
} // namespace voo
