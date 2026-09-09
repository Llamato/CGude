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
        "armv7l-linux"
        "aarch64-darwin"
      ];
    in
    inputs.flake-utils.lib.eachSystem supportedSystems (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
        lib = pkgs.lib;

        glfw-configured = pkgs.glfw.overrideAttrs (old: {
          cmakeFlags = (old.cmakeFlags or [ ]) ++ [
            (pkgs.lib.cmakeBool "GLFW_BUILD_WAYLAND" false)
          ];
        });

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
          number: pname: options:
          with builtins // lib;
          let
            padFront =
              string: padding: targetLength:
              if (stringLength string) < targetLength then
                padFront (padding + string) padding targetLength
              else
                string;
            paddedNum = padFront (toString number) "0" 2;
            path = "${paddedNum}_${pname}";
            flags = map (option: "-D" + option) options;
          in
          pkgs.stdenv.mkDerivation {
            inherit pname nativeBuildInputs buildInputs;
            version = "1.0.0";
            src = cleanSource ./.;

            patchPhase = optionalString (length options > 0) ''
              substituteInPlace ${path}/makefile --replace 'CFLAGS=-c' 'CFLAGS=${concatStringsSep " " flags} -c'
            '';

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
          "OBJ" = 1;
          "Intersect" = 2;
          "Splines" = 3;
          "Color" = 4;
          "Diffuse" = 5;
          "Phong" = 6;
        };

        options = [
          "EXTRA"
          "SAUCE"
        ];

        allOptions =
          with builtins // lib;
          foldl (acc: elem: acc // elem) { } (
            flatten (
              map
                (
                  currentConfig:
                  mapAttrs' (
                    task: index:
                    let
                      selectedOptions = attrNames (filterAttrs (_: v: v) currentConfig);
                    in
                    {
                      name = toLower (concatStringsSep "-" ([ task ] ++ selectedOptions));
                      value = {
                        inherit index task;
                        options = selectedOptions;
                      };
                    }
                  ) tasks
                )
                (
                  cartesianProduct (
                    listToAttrs (
                      map (option: {
                        name = option;
                        value = [
                          true
                          false
                        ];
                      }) options
                    )
                  )
                )
            )
          );

        allTasks = lib.mapAttrs (
          drvname: drvattrs: cgude-build drvattrs.index drvattrs.task drvattrs.options
        ) allOptions;

        boringTasks = lib.mapAttrs (
          drvname: drvattrs: cgude-build drvattrs.index drvattrs.task drvattrs.options
        ) (lib.filterAttrs (_: drvattrs: builtins.length drvattrs.options == 0) allOptions);

      in
      {
        packages = {
          default = pkgs.symlinkJoin {
            name = "cgude-tasks";
            paths = builtins.attrValues boringTasks;
          };
        }
        // allTasks;

        apps = builtins.mapAttrs (
          name: drv:
          let
            runScript = ''
              cd ${drv}
              exec $(find . -type f -executable | head -21)
            '';
          in
          {
            type = "app";
            program = "${pkgs.writeShellScript "cgude-run-${name}" runScript}";
          }
        ) allTasks;

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
