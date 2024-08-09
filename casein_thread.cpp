module voo;
import casein;
import sith;

voo::casein_thread::casein_thread() {
  casein::handle(casein::CREATE_WINDOW, [this] { m_run = sith::run_guard{this}; });
  casein::handle(casein::RESIZE_WINDOW, [this] { if (!casein::window_live_resize) m_resized = true; });
  casein::handle(casein::QUIT, [this] { m_run = {}; });
}
