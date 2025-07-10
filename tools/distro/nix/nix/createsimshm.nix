{ version
, shortname
, stdenv
,
}:
stdenv.mkDerivation {
  name = "createsimshm";
  inherit version;

  buildInputs = [ ];
  nativeBuildInputs = [ ];

  src = ./../../../..;

  buildPhase = ''
    make -f Makefile.${shortname} assets/${shortname}shm
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp -v assets/* $out/bin
  '';
}
