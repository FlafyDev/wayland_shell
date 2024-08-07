{
  description = "Frontend torrent client";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/f3834de3782b82bfc666abf664f946d0e7d1f116";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    flake-utils,
    nixpkgs,
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {
        inherit system;
        overlays = [
          self.overlays.default
        ];
      };
    in {
      packages = {
        # inherit (pkgs) flarrent;
        # default = pkgs.flarrent;
      };

      devShell = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          pkg-config
          flutter322
          inotify-tools
          lsof
          meson
          ninja
          pkg-config
          scdoc
          wayland-scanner
        ];
        buildInputs = with pkgs; [
          libGL
          gtk-layer-shell
          cava
          pixman
          libpng
          libjpeg
          wayland
          wayland-protocols
          epoxy
          libdrm.out
        ];
      };
    })
    // {
      overlays.default = _final: prev: {
        # flarrent = prev.callPackage ./nix/package.nix {};
      };
    };
}
