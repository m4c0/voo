export module voo:sires_image;
import :command_pool;
import :device_and_queue;
import :guards;
import :h2l_image;
import :images;
import :mapmem;
import :updater;
import :queue;
import dotz;
import jute;
import silog;
import sires;
import sith;
import stubby;
import traits;
import vee;
import wagen;

namespace voo {
  export auto load_image_file_as_buffer(jute::view file, vee::physical_device pd) {
    auto img = stbi::load(sires::slurp(file));
    auto res = host_buffer_for_image(pd, img.width, img.height, 4);
    {
      mapmem m { *res.memory };
      auto *c = static_cast<unsigned char *>(*m);
      for (auto i = 0; i < img.width * img.height * 4; i++) {
        c[i] = (*img.data)[i];
      }
    }
    silog::log(silog::info, "Pre-loaded %dx%d image [%.*s]",
        img.width, img.height,
        static_cast<unsigned>(file.size()), file.begin());
    return res;
  }

  export auto load_image_file(jute::view file, vee::physical_device pd) {
    auto img = stbi::load(sires::slurp(file));
    unsigned w = img.width;
    unsigned h = img.height;
    auto m_img = h2l_image { pd, w, h, VK_FORMAT_R8G8B8A8_SRGB };

    {
      mapmem m{m_img.host_memory()};
      auto *c = static_cast<unsigned char *>(*m);
      for (auto i = 0; i < img.width * img.height * 4; i++) {
        c[i] = (*img.data)[i];
      }
    }
    silog::log(silog::info, "Loaded %dx%d image [%.*s]",
        img.width, img.height,
        static_cast<unsigned>(file.size()), file.begin());
    return m_img;
  }
  export auto load_sires_image(jute::view file, vee::physical_device pd) {
    auto img = stbi::load(sires::slurp(file));
    unsigned w = img.width;
    unsigned h = img.height;
    auto m_img = h2l_image { pd, w, h, VK_FORMAT_R8G8B8A8_SRGB };

    {
      mapmem m{m_img.host_memory()};
      auto *c = static_cast<unsigned char *>(*m);
      for (auto i = 0; i < img.width * img.height * 4; i++) {
        c[i] = (*img.data)[i];
      }
    }
    silog::log(silog::info, "Loaded %dx%d image [%.*s]",
        img.width, img.height,
        static_cast<int>(file.size()), file.data());

    return m_img;
  }
  export constexpr auto load_sires_image(jute::view file) {
    return [=](vee::physical_device pd) { return load_sires_image(file, pd); };
  }

  export void copy_buffer_to_image_sync(vee::extent ext, vee::buffer::type buf, vee::image::type img) {
    fence f { false };
    voo::command_pool cpool {};
    auto cb = cpool.allocate_primary_command_buffer();

    {
      cmd_buf_one_time_submit ots { cb };
      vee::cmd_pipeline_barrier(cb, img, vee::from_host_to_transfer);
      vee::cmd_copy_buffer_to_image(cb, ext, buf, img);
      vee::cmd_pipeline_barrier(cb, img, vee::from_transfer_to_fragment);
    }
    queue::universal()->queue_submit({
      .fence = f,
      .command_buffer = cb,
    });

    f.wait();
  }

  export void load_image(jute::view file, vee::physical_device pd, voo::bound_image * bi, hai::fn<void, dotz::ivec2> callback) {
    // TODO: call this on a different thread
    {
      auto img = stbi::load(sires::slurp(file));
      unsigned w = img.width;
      unsigned h = img.height;
      unsigned sz = w * h * 4;
      auto host = bound_buffer::create_from_host(pd, sz);
      {
        memiter<unsigned char> c { *host.memory };
        for (auto i = 0; i < sz; i++) c[i] = (*img.data)[i];
      }
  
      constexpr const auto fmt = VK_FORMAT_R8G8B8A8_SRGB;
      vee::extent ext { w, h };
      bi->img = vee::create_image(ext, fmt, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
      bi->mem = vee::create_local_image_memory(pd, *bi->img);
      vee::bind_image_memory(*bi->img, *bi->mem);
      bi->iv = vee::create_image_view(*bi->img, fmt);
  
      copy_buffer_to_image_sync(ext, *host.buffer, *bi->img);
      callback(dotz::ivec2 { w, h });
    }
  }
  export void load_image(jute::view file, voo::bound_image * bi, hai::fn<void, dotz::ivec2> callback) {
    return load_image(file, wagen::physical_device(), bi, callback);
  }
} // namespace voo
