{
  description = "Description for the project";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    self.submodules = true;
  };

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      inherit (pkgs) lib;

      version = "0.1.0";

      supportedGames = {
        assettocorsa = {
          shortname = "ac";
          createsimshm = true;
        };
        projectcars2 = {
          shortname = "pcars2";
          createsimshm = true;
        };
        rfactor2 = {
          shortname = "rf2";
          createsimshm = false;
        };
      };
      gamePackages = lib.mapAttrs
        (name: simdef:
          let
            simshmbridge = pkgs.callPackage ./nix/simshmbridge.nix {
              inherit version;
              shortname = simdef.shortname;
            };
            createsimshm = pkgs.callPackage ./nix/createsimshm.nix {
              inherit version;
              shortname = simdef.shortname;
            };
          in
          {
            "simshmbridge-${name}" = simshmbridge;
          } // lib.optionalAttrs simdef.createsimshm {
            "createsimshm-${name}" = createsimshm;
          } // {
            "${name}" = pkgs.symlinkJoin {
              name = "simshm-${name}";
              paths = [ simshmbridge ] ++ lib.optionals simdef.createsimshm [ createsimshm ];
            };
          }
        )
        supportedGames;
      mergedGames = lib.foldlAttrs (acc: _: value: acc // value) { } gamePackages;
      allGames = pkgs.symlinkJoin {
        name = "simshm-all";
        paths = lib.attrValues (lib.getAttrs (builtins.attrNames supportedGames) mergedGames);
      };
    in
    {
      packages.${system} = mergedGames // { all = allGames; };
    };
}
