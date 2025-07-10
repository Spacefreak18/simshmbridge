{ version
, simdef
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
    make -f Makefile.${simdef} assets/${simdef}shm
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp -v assets/* $out/bin
  '';
}
