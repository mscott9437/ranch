{
  description = "Zig + C++ + Vulkan Development Environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            # Toolchain
            zig
            clang
            llvm
            lldb
            pkg-config
            gdb

            # Vulkan tooling
            vulkan-tools

            # Serialization / WASM
            protobuf
            wasmtime

            # Shader tooling
            shaderc
            spirv-tools
            spirv-headers
          ];

          buildInputs = with pkgs; [
            # Vulkan
            vulkan-loader
            vulkan-headers
            vulkan-validation-layers

            # Graphics
            mesa
            glfw
            glm

            # Text
            freetype

            # Database
            sqlite

            # Networking
            curl

            # Compression
            zlib
          ];

          shellHook = ''
            export CC=clang
            export CXX=clang++
            export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d"

            echo ""
            echo "=== Zig + Clang Vulkan Development Shell ==="
            echo "Zig: $(zig version)"
            echo "Clang: $(clang --version | head -n1)"
            echo ""

            echo "C++ (clang):"
            echo '  clang++ src/main.cpp src/glad.cpp -std=c++23 -Iinclude $(pkg-config --cflags --libs glfw3 freetype2 vulkan sqlite3 libcurl zlib) -o ranch'
            echo ""

            echo "C++ (zig):"
            echo '  zig c++ src/main.cpp src/glad.cpp -std=c++23 -Iinclude $(pkg-config --cflags --libs glfw3 freetype2 vulkan sqlite3 libcurl zlib) -o ranch'
            echo ""
          '';
        };
      });
}