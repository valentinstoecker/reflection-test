{
  stdenv,
  python314Packages,
  rapidjson,
  cmake,
}:
stdenv.mkDerivation {
  name = "reflect_test";
  src = ./src;
  nativeBuildInputs = [
    cmake
  ];
  buildInputs = [
    rapidjson
    python314Packages.nanobind
  ];
}
