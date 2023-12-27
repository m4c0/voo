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
};

export class cmd_render_pass {
  vee::command_buffer m_cb;

public:
  explicit cmd_render_pass(const vee::render_pass_begin &rpb)
      : m_cb{rpb.command_buffer} {
    vee::cmd_begin_render_pass(rpb);
    vee::cmd_set_scissor(m_cb, rpb.extent);
    vee::cmd_set_viewport(m_cb, rpb.extent);
  }
  ~cmd_render_pass() { vee::cmd_end_render_pass(m_cb); }

  cmd_render_pass(const cmd_render_pass &) = delete;
  cmd_render_pass(cmd_render_pass &&) = delete;
  cmd_render_pass &operator=(const cmd_render_pass &) = delete;
  cmd_render_pass &operator=(cmd_render_pass &&) = delete;

  [[nodiscard]] constexpr auto operator*() const noexcept { return m_cb; }
};
} // namespace voo
