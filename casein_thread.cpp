module voo;
import casein;
import sith;

static voo::casein_thread *t{};

voo::casein_thread::casein_thread() {
  t = this;
  casein::handle(casein::CREATE_WINDOW, [] { t->m_run = sith::run_guard{t}; });
  casein::handle(casein::RESIZE_WINDOW, [] { t->m_resized = true; });
  casein::handle(casein::QUIT, [] { t->m_run = {}; });
}
