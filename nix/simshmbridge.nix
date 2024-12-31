{
  version,
  simdef,
  simapi,
  pkgsCross,
}:
pkgsCross.mingw32.stdenv.mkDerivation {
  name = "simshmbridge";
  inherit version;

  src = ./..;

  configurePhase = ''
    ln -s ${simapi} simapi
  '';

  buildPhase = ''
    make assets/simshmbridge.exe CFLAGS=-D${simdef}
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp -v assets/* $out/bin
  '';
}
