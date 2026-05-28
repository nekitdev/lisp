{
  description = "Lisp interpreter in C++";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";

    utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      nixpkgs,
      utils,
      ...
    }:
    utils.lib.eachSystem utils.lib.allSystems (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in
      {
        devShells.default = pkgs.mkShell {
          name = "lisp";

          packages = with pkgs; [ llvm cmake ];
        };

        packages.default = pkgs.callPackage ./default.nix { };
      }
    );
}
