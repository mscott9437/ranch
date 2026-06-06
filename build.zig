const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // Graphics
    exe.linkSystemLibrary("vulkan");
    exe.linkSystemLibrary("glfw");

    // Text rendering
    exe.linkSystemLibrary("freetype");

    // Database
    exe.linkSystemLibrary("sqlite3");

    // Networking
    exe.linkSystemLibrary("curl");

    // Compression
    exe.linkSystemLibrary("z");

    // Optional if you start using C imports:
    exe.linkLibC();

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the application");
    run_step.dependOn(&run_cmd.step);
}
