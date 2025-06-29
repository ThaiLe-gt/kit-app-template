-- Use folder name to build extension name and tag. Version is specified explicitly.
local ext = get_current_extension_info()

project_ext (ext)

-- Link only those files and folders into the extension target directory
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
}

-- Build Carbonite plugin to be loaded by the extension. This plugin implements
-- omni::ext::IExt interface to be automatically started by extension system.
project_ext_plugin(ext, "akila.cpp_extension.plugin")
    local plugin_name = "akila.cpp_extension"
    add_files("iface", "%{root}/include/omni/ext", "IExt.h")
    add_files("impl", "plugins/"..plugin_name)
    includedirs { "plugins/"..plugin_name }

    -- Begin OpenUSD
    extra_usd_libs = {
        "usd",
        "usdGeom",
        "usdUtils"
    }
 
    add_usd(extra_usd_libs)
    libdirs { "%{target_deps}/usd/release/lib" }
    defines { "NOMINMAX", "NDEBUG" }
    runtime "Release"
    rtti "On"
 
    filter { "system:linux" }
        exceptionhandling "On"
        staticruntime "Off"
        cppdialect "C++17"
        buildoptions { "-pthread -lstdc++fs -Wno-error" }
        linkoptions { "-Wl,--disable-new-dtags -Wl,-rpath,%{target_deps}/usd/release/lib:%{target_deps}/python/lib:" }
    filter { "system:windows" }
        buildoptions { "/wd4244 /wd4305" }
    filter {}

-- To write C++ tests, we have to create a shared library with tests to be loaded
-- project_ext_tests(ext, ext.id..".tests")
    -- dependson { ext.id..".plugin" }
    -- add_files("impl", "tests.cpp")
    -- includedirs { "%{target_deps}/doctest/include" }
