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
            pkg-config
            gdb

            # Vulkan tooling
            vulkan-tools

            # Serialization / WASM
            protobuf
            wasmtime

            # Useful Vulkan/SPIR-V tooling
            shaderc
            spirv-tools
            spirv-headers
          ];

          buildInputs = with pkgs; [
            # Vulkan SDK components
            vulkan-loader
            vulkan-headers
            vulkan-validation-layers

            # OpenGL implementation
            mesa

            # Graphics helpers
            glfw
            glm

            # Text rendering
            freetype

            # Database
            sqlite

            # Networking
            curl

            # Compression
            zlib
          ];

          shellHook = ''
            export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d"

            echo ""
            echo "=== Zig Vulkan Development Shell ==="
            echo "Zig: $(zig version)"
            echo ""

            echo "Libraries:"
            echo "  Vulkan"
            echo "  GLFW"
            echo "  GLM"
            echo "  FreeType"
            echo "  SQLite"
            echo "  cURL"
            echo "  zlib"
            echo "  Protobuf"
            echo "  Wasmtime"
            echo ""

            echo "Zig:"
            echo "  zig build"
            echo "  zig build run"
            echo "  zig build -Doptimize=ReleaseFast"
            echo ""

            echo "C++:"
            echo '  zig c++ src/main.cpp -std=c++23 $(pkg-config --cflags --libs glfw3 freetype2 vulkan sqlite3 libcurl zlib) -o app'
            echo ""

            echo "WebAssembly:"
            echo "  zig build -Dtarget=wasm32-wasi"
            echo "  zig build -Dtarget=wasm32-freestanding"
            echo ""

            echo "Protobuf:"
            echo "  protoc --cpp_out=. schema.proto"
            echo ""

            echo "Wasmtime:"
            echo "  wasmtime module.wasm"
            echo ""

            vulkaninfo --summary >/dev/null 2>&1 \
              && echo "✓ Vulkan runtime detected" \
              || echo "✗ Vulkan runtime not currently available"

            echo ""
          '';
        };
      });
}