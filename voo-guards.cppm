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

  [[nodiscard]] constexpr auto operator*() const noexcept { return m_cb; }
};
} // namespace voo
