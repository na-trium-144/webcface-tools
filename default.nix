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
in
pkgs.stdenv.mkDerivation rec {
  pname = "webcface-tools";
  version = "3.0.0";

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
    # pkgs.fmt_11
    # spdlog
  ];

  mesonFlags = [];
}
