{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }: flake-utils.lib.eachDefaultSystem (
    system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in {
      devShells.default = pkgs.mkShell {
        buildInputs = [
          pkgs.gcc-arm-embedded-13
          pkgs.python3
          pkgs.cmake
          pkgs.picotool
        ];
      };
    }
  );
}
