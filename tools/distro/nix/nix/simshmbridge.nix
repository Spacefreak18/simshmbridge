{
  version,
  simdef,
  pkgsCross,
}:
pkgsCross.mingw32.stdenv.mkDerivation {
  name = "simshmbridge";
  inherit version;

  src = ./..;

  buildPhase = ''
    make assets/simshmbridge.exe CFLAGS=-D${simdef}
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp -v assets/* $out/bin
  '';
}
