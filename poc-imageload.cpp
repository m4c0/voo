#pragma leco tool

import hai;
import jute;
import silog;
import stubby;
import voo;

int main() {
  voo::device_and_queue dq { "poc-imageload" };

  voo::bound_image img {};
  voo::load_image("poc-imageload.png", dq.physical_device(), dq.queue(), &img, [] {
    silog::log(silog::info, "Image loaded");
  });

  silog::log(silog::info, "Waiting for idle device");
  vee::device_wait_idle();
}
