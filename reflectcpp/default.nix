{
  stdenv,
  cmake,
  fetchFromGitHub,
  tomlplusplus,
  ctre,
  yyjson,
  lib,
  enableToml ? false,
}:
stdenv.mkDerivation rec {
  name = "reflectcpp";
  version = "0.25.0";
  src = fetchFromGitHub {
    owner = "getml";
    repo = "reflect-cpp";
    tag = "v${version}";
    hash = "sha256-ohj8Oue2Q/02r9hJmGw2bl6qW2xUsbjFJTquHG8JvcI=";
  };
  nativeBuildInputs = [
    cmake
  ];
  buildInputs = [
    ctre
    yyjson
  ]
  ++ lib.optional enableToml tomlplusplus;
  propagatedBuildInputs = [
    ctre
    yyjson
  ]
  ++ lib.optional enableToml tomlplusplus;
  cmakeFlags = [
    "-DREFLECTCPP_USE_VCPKG=OFF"
    "-DREFLECTCPP_USE_BUNDLED_DEPENDENCIES=OFF"
    (lib.cmakeBool "REFLECTCPP_TOML" enableToml)
  ];
}
