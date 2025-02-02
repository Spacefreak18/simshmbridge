{
  description = "Description for the project";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { nixpkgs, ... }:
    let
      system = "x86_64-linux";

      pkgs = nixpkgs.legacyPackages.${system};
      inherit (pkgs) lib;

      version = "0.1.0";

      supportedGames = {
        assettocorsa = "ASSETTOCORSA";
      };

      corePackages = scope: {
        simapi = scope.callPackage ./nix/simapi.nix { };
      };

      mapGameAttrs = scope: name: simdef: {
        ${name} = scope.callPackage ./nix/${name}.nix { };

        "simshmbridge-${name}" = scope.callPackage ./nix/simshmbridge.nix {
          inherit version simdef;
        };

        "createsimshm-${name}" = scope.callPackage ./nix/createsimshm.nix {
          inherit version simdef;
        };
      };

      flakeScope = pkgs.lib.makeScope pkgs.newScope (
        scope:
        (corePackages scope)
        // (lib.mergeAttrsList (lib.mapAttrsToList (mapGameAttrs scope) supportedGames))
      );
    in
    {
      packages.${system} = flakeScope.packages flakeScope;
    };
}
