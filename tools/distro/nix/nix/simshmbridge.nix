{
  version,
  simdef,
  pkgsCross,
}:
pkgsCross.mingwW64.stdenv.mkDerivation {
  name = "simshmbridge";
  inherit version;

  src = ./../../../..;

  buildPhase = ''
    make -f Makefile.${simdef} assets/${simdef}bridge.exe
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp -v assets/* $out/bin
  '';
}
