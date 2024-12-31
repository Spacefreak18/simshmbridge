{
  version,
  simdef,
  simapi,
  stdenv,
}:
stdenv.mkDerivation {
  name = "createsimshm";
  inherit version;

  buildInputs = [ ];
  nativeBuildInputs = [ ];

  src = ./..;

  configurePhase = ''
    ln -s ${simapi} simapi
  '';

  buildPhase = ''
    make assets/createsimshm CFLAGS=-D${simdef}
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp -v assets/* $out/bin
  '';
}
