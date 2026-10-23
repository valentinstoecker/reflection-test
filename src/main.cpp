#include "include/info.hpp"
#include "include/json.hpp"
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <variant>

struct Vec3d {
  double x, y, z;
};

INFO(Vec3d) { FIELDS(x, y, z); };

void write_json(const Vec3d &value, rapidjson::Value &out_value,
                rapidjson::MemoryPoolAllocator<> alloc) {
  out_value.SetArray();
  out_value.PushBack(value.x, alloc);
  out_value.PushBack(value.y, alloc);
  out_value.PushBack(value.z, alloc);
}

bool read_json(Vec3d &value, const rapidjson::Value &in_value) {
  value.x = in_value[0].GetDouble();
  value.y = in_value[1].GetDouble();
  value.z = in_value[2].GetDouble();
  return true;
}

struct BoundingBox3d {
  Vec3d lower_bound;
  Vec3d upper_bound;
};

INFO(BoundingBox3d) {
  FIELDS(lower_bound, upper_bound);
  VALIDATOR {
    CHECK_LE(self.lower_bound.x, self.upper_bound.x);
    CHECK_LE(self.lower_bound.y, self.upper_bound.y);
    CHECK_LE(self.lower_bound.z, self.upper_bound.z);
  };
};

struct Sphere {
  double radius;
  Vec3d center;
};

INFO(Sphere) {
  FIELDS(radius, center);
  VALIDATOR { CHECK(self.radius >= 0); };
};

using Shape = std::variant<BoundingBox3d, Sphere>;

struct Element {
  Shape body;
  Vec3d position;
  Vec3d direction;
};

INFO(Element) { FIELDS(body, position, direction); };

int main() {
  rapidjson::Document parsed;
  parsed.Parse("{\"body\":{\"type\":\"BoundingBox3d\",\"lower_bound\":[1.0,2.0,"
               "3.0],\"upper_bound\":[0.0,0.0,0.0]},\"position\":[0.0,0.0,0.0],"
               "\"direction\":[0.0,0.0,0.0]}");
  Element el;
  bool ok = read_json(el, parsed);
  std::cout << ok << std::endl;

  rapidjson::Document written;
  write_json_default(el, written, written.GetAllocator());

  rapidjson::OStreamWrapper osw(std::cout);
  rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
  written.Accept(writer);
  std::cout << std::endl;
  std::cout << ok << std::endl;
  ;
}