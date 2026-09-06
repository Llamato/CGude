{
  description = "A development environment for completing homework tasks based on the cgude opengl framework";

  inputs = {
    nixpkgs.url = "github:NixOs/nixpkgs/nixos-26.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { self, nixpkgs, ... }@inputs:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "riscv64-linux"
        "aarch64-darwin"
      ];
    in
    inputs.flake-utils.lib.eachSystem supportedSystems (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
        lib = pkgs.lib;
        
        glfw-configured = pkgs.glfw.overrideAttrs (old: {
          cmakeFlags = (old.cmakeFlags or []) ++ [
            (pkgs.lib.cmakeBool "GLFW_BUILD_WAYLAND" false)
          ];});

        nativeBuildInputs = with pkgs; [
          gnumake
          gcc
        ];

        buildInputs = with pkgs; [
          glfw-configured
          glew
          libGL
          libGLU
          mesa
          libglvnd
          libX11
          libXrandr
          libXinerama
          libXcursor
          libXi
        ];

        cgude-utils = pkgs.stdenv.mkDerivation {
          pname = "cgude-utils";
          version = "1.0.0";
          src = pkgs.lib.cleanSource ./Utils;
          inherit nativeBuildInputs buildInputs;

          buildPhase = ''
            runHook preBuild
            make CXX=g++ CC=g++
            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall
            mkdir -p $out
            cp build/native/*.o $out/
            cp helvetica* $out/ 2>/dev/null || true
            runHook postInstall
          '';
        };
        cgude-build =
          number: name:
          let
            padFront =
              string: padding: targetLength:
              if (builtins.stringLength string) < targetLength then
                padFront (padding + string) padding targetLength
              else
                string;
            paddedNum = padFront (toString number) "0" 2;
            path = "${paddedNum}_${name}";
          in
          pkgs.stdenv.mkDerivation {
            pname = "cgude-${lib.toLower name}";
            version = "1.0.0";
            src = pkgs.lib.cleanSource ./.;
            inherit nativeBuildInputs buildInputs;

            buildPhase = ''
              runHook preBuild
              mkdir -p Utils/build/native
              cp ${cgude-utils}/*.o Utils/build/native 2>/dev/null || true
              cd ${path}
              make CXX=g++ CC=g++ LDFLAGS="-lstdc++"
              cd ..
              runHook postBuild
            '';

            installPhase = ''
              runHook preInstall
              mkdir -p $out
              find ${path}/build -maxdepth 1  -type f -executable -exec cp {} $out \;
              if [ -d ${path}/Datasets ]; then
                find ${path}/Datasets -maxdepth 1 -type f -exec cp {} $out \;
              fi
              runHook postInstall
            '';
          };
        tasks = {
          cg-obj = cgude-build 1 "OBJ";
          cg-intersect = cgude-build 2 "Intersect";
          cg-splines = cgude-build 3 "Splines";
          cg-color = cgude-build 4 "Color";
          cg-diffuse = cgude-build 5 "Diffuse";
          cg-phong = cgude-build 6 "Phong";
          #cg-raycast = cgude-build 7 "Raycasting";
          #cg-texturing = cgude-build 9 "Texturing";
          #cg-hellogl = cgude-build 10 "HelloGL";
          #cg-triforce = cgude-build 11 "Triforce";
          #cg-dendritegrowth = cgude-build 12 "DendriteGrowth";
        };
      in
      {
        packages = {
          default = pkgs.symlinkJoin {
            name = "cgude-tasks";
            paths = builtins.attrValues tasks;
          };
        }
        // tasks;
        apps = builtins.mapAttrs (name: drv: {
          type = "app";
          program = "${pkgs.writeShellScript "cgude-run-${name}" "exec $(find ${drv}/ -type f -executable | head -1)"}";
        }) tasks;
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            gnumake
            gcc
            gdb
            clang-tools
            bear
            pkg-config
            glfw-configured
            glew
            libGL
            libGLU
            mesa
            libglvnd
            libX11
            libXrandr
            libXinerama
            libXcursor
            libXi
          ];

          shellHook = ''
            export CC=g++
            export CXX=g++
            echo "Development environment ready"
            echo "GCC version: $(g++ --version | head -n1)"
          '';
        };
      }
    );
}
