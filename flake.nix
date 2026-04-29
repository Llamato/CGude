{
  description = "";
  
  outputs = { self, nixpkgs }: let
    systems = [
      "x86_64-linux"
      "aarch64-linux"
      "x86_64-darwin"
      "aarch64-darwin"
      "armv7l-linux"
    ];
    
    forAllSystems = nixpkgs.lib.genAttrs systems;
    pkgsFor = system: import nixpkgs { inherit system; };
  in {
    packages = forAllSystems (system:
      let
        padFront = string: padding: targetLength: 
          if (builtins.stringLength string) < targetLength 
          then padFront (padding + string) padding targetLength 
          else string;
        
        lib = nixpkgs.lib;
        pkgs = pkgsFor system;
        
        glfw-configured = pkgs.glfw.overrideAttrs (old: {
          cmakeFlags = (old.cmakeFlags or []) ++ [
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
        
        # Build Utils library
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
        
        # Function to build individual assignments
        cgude-build = number: name: let
          paddedNum = padFront (builtins.toString number) "0" 2;
          path = "${paddedNum}_${name}";
        in pkgs.stdenv.mkDerivation {
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
            find ${path}/build -maxdepth 1 -type f -exec cp {} $out \;
            if [ -d ${path}/Datasets ]; then
              find ${path}/Datasets -maxdepth 1 -type f -exec cp {} $out \;
            fi
            runHook postInstall
          '';
        };
        
      in {
        inherit cgude-utils;

        cg-obj = cgude-build 1 "OBJ";
        cg-intersect = cgude-build 2 "Intersect";
        cg-splines = cgude-build 3 "Splines";
        cg-color = cgude-build 4 "Color";
        cg-diffuse = cgude-build 5 "Diffuse";
        cg-Phong = cgude-build 6 "Phong";
        cg-raycast = cgude-build 7 "Raycasting";
        cg-texturing = cgude-build 9 "Texturing";
        cg-hellogl = cgude-build 10 "HelloGL";
        cg-triforce = cgude-build 11 "Triforce";
        cg-dendritegrowth = cgude-build 12 "DendriteGrowth";
      }
    );
    
    devShells = forAllSystems (system:
      let 
        pkgs = pkgsFor system;
        
        glfw-configured = pkgs.glfw.overrideAttrs (old: {
          cmakeFlags = (old.cmakeFlags or []) ++ [
            (pkgs.lib.cmakeBool "GLFW_BUILD_WAYLAND" false)
          ];
        });
      in {
        default = pkgs.mkShell {
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
  };
}