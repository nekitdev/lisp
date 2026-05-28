{ lib, llvm, cmake, ... }:

llvm.stdenv.mkDerivation {
  pname = "lisp";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [ cmake ];

  meta = {
    homapage = "https://github.com/nekitdev/lisp";
    description = "Lisp interpreter in C++";
    license = lib.licenses.mit;
    platforms = lib.platforms.all;
    maintainers = [ lib.maintainers.nekitdev ];
  };
}
