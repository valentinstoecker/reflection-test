{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "https://channels.nixos.org/nixpkgs-unstable/nixexprs.tar.zst";
  };

  outputs = inputs: {
    overlays.default = final: prev: {
      reflectcpp = final.callPackage ./reflectcpp { enableToml = true; };
      reflect_test = final.callPackage ./build.nix { };
      nlohmann-json = final.callPackage ./nlohmann-json { };
    };
    packages = builtins.mapAttrs (
      system: pkgs:
      let
        newpkgs = pkgs.extend inputs.self.overlays.default;
      in
      {
        inherit (newpkgs) reflectcpp reflect_test nlohmann-json;
        default = newpkgs.reflect_test;
      }
    ) inputs.nixpkgs.legacyPackages;
  };
}
