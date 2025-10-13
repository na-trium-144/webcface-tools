{
  pkgs ? import <nixpkgs> {},
}:
let
  webcface-src = pkgs.fetchFromGitHub {
    owner = "na-trium-144";
    repo = "webcface";
    rev = "v3.1.1";
    sha256 = "sha256-jZ/w1Qn3kU0+gHBoppXeI0irfGaFNmjXp3tr7zgJYKE=";
  };
  webcface = pkgs.callPackage (webcface-src + "/default.nix") { inherit pkgs; };

  tiny-process-library = pkgs.stdenv.mkDerivation {
    pname = "tiny-process-library";
    version = "v2.0.4-29";
    src = pkgs.fetchFromGitLab {
      owner = "eidheim";
      repo = "tiny-process-library";
      rev = "8bbb5a211c5c9df8ee69301da9d22fb977b27dc1";
      sha256 = "sha256-EdaPXKHbAMR2M2FwPnDP+KeuYbGfGE2j5QXB+CyyjnM=";
    };
    nativeBuildInputs = [ pkgs.cmake ];
  };
in
pkgs.stdenv.mkDerivation rec {
  pname = "webcface-tools";
  version = "3.0.1";

  src = ./.;
  sourceRoot = pname;

  nativeBuildInputs = [
    pkgs.meson
    pkgs.ninja
    pkgs.pkg-config
    pkgs.cmake
  ];
  buildInputs = [
    webcface
    pkgs.ftxui
    pkgs.SDL2
    pkgs.tomlplusplus
    pkgs.cli11
    tiny-process-library
    # from webcface:
      # pkgs.fmt_11
      # spdlog
  ];

  mesonFlags = [];
}
