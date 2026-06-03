let
  pkgs = import <nixpkgs> { };
in
pkgs.stdenv.mkDerivation {
  name = "Packify";
  src = ./.;

  nativeBuildInputs = with pkgs; [ gcc gnumake ];

  installPhase = ''
    mkdir -p $out/bin
    cp Packify $out/bin/Packify
  '';
}
