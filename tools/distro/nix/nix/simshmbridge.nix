{ version
, shortname
, pkgsCross
,
}:
pkgsCross.mingwW64.stdenv.mkDerivation {
  name = "simshmbridge";
  inherit version;

  src = ./../../../..;

  buildPhase = ''
    make -f Makefile.${shortname} assets/${shortname}bridge.exe
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp -v assets/* $out/bin
  '';
}
