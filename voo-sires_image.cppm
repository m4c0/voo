export module voo:sires_image;
import :device_and_queue;
import :guards;
import :h2l_image;
import :mapmem;
import :updater;
import :queue;
import jute;
import silog;
import sith;
import stubby;
import traits;
import vee;

namespace voo {
export auto load_sires_image(jute::view file, vee::physical_device pd) {
  return stbi::load_from_resource(file)
      .map([file, pd](auto &&img) {
        unsigned w = img.width;
        unsigned h = img.height;
        auto m_img = h2l_image{pd, w, h};

        {
          mapmem m{m_img.host_memory()};
          auto *c = static_cast<unsigned char *>(*m);
          for (auto i = 0; i < img.width * img.height * 4; i++) {
            c[i] = (*img.data)[i];
          }
        }
        silog::log(silog::info, "Loaded %dx%d image [%.*s]", img.width,
                   img.height, static_cast<int>(file.size()), file.data());
        return traits::move(m_img);
      })
      .take([file](auto msg) {
        silog::log(silog::error, "Failed loading resource image [%.*s]: %s",
                   static_cast<int>(file.size()), file.data(), msg);
      });
}

export class sires_image : public updater<h2l_image> {
  jute::view m_name;
  vee::physical_device m_pd;

  void update_data(h2l_image *img) override {
    *img = load_sires_image(m_name, m_pd);
  }

public:
  sires_image(jute::view name, voo::device_and_queue *dq)
      : sires_image{name, dq->physical_device(), dq->queue()} {}
  sires_image(jute::view name, vee::physical_device pd, voo::queue *q)
      : updater{q, {}}
      , m_name{name}
      , m_pd{pd} {
    run_once();
  }
};
} // namespace voo
