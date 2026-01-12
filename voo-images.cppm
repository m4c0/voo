export module voo:images;
import vee;
import wagen;

namespace voo {
  export struct bound_image {
    vee::image img {};
    vee::device_memory mem {};
    vee::image_view iv {};

    static bound_image create_depth(vee::extent ext, auto ... usages) {
      bound_image res {};
      res.img = vee::create_depth_image(ext, usages...);
      res.mem = vee::create_local_image_memory(wagen::physical_device(), *res.img);
      vee::bind_image_memory(*res.img, *res.mem);
      res.iv = vee::create_depth_image_view(*res.img);
      return res;
    }

    static bound_image create(vee::extent ext, vee::format fmt, auto... usages) {
      bound_image res {};
      res.img = vee::create_image(ext, fmt, usages...);
      res.mem = vee::create_local_image_memory(wagen::physical_device(), *res.img);
      vee::bind_image_memory(*res.img, *res.mem);
      res.iv = vee::create_image_view(*res.img, fmt);
      return res;
    }
  };
}
