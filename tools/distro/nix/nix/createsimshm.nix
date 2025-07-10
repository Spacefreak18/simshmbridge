{
  version,
  simdef,
  stdenv,
}:
stdenv.mkDerivation {
  name = "createsimshm";
  inherit version;

  buildInputs = [ ];
  nativeBuildInputs = [ ];

  src = ./..;

  buildPhase = ''
    make assets/createsimshm CFLAGS=-D${simdef}
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp -v assets/* $out/bin
  '';
}
