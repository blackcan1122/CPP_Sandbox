newoption
{
	trigger = "graphics",
	value = "OPENGL_VERSION",
	description = "version of OpenGL to build raylib against",
	allowed = {
		{ "opengl11", "OpenGL 1.1"},
		{ "opengl21", "OpenGL 2.1"},
		{ "opengl33", "OpenGL 3.3"},
		{ "opengl43", "OpenGL 4.3"},
		{ "openges2", "OpenGL ES2"},
		{ "openges3", "OpenGL ES3"}
	},
	default = "opengl33"
}

function download_progress(total, current)
    local ratio = current / total;
    ratio = math.min(math.max(ratio, 0), 1);
    local percent = math.floor(ratio * 100);
    print("Download progress (" .. percent .. "%/100%)")
end

function check_raylib()
    os.chdir("external")
    if(os.isdir("raylib-master") == false) then
        if(not os.isfile("raylib-master.zip")) then
            print("Raylib not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/raysan5/raylib/archive/refs/heads/master.zip", "raylib-master.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " ..  os.getcwd())
        zip.extract("raylib-master.zip", os.getcwd())
        os.remove("raylib-master.zip")
    end
    os.chdir("../")
end

function build_externals()
     print("calling externals")
     check_raylib()
end

function platform_defines()
    filter {"configurations:Debug or Release"}
        defines{"PLATFORM_DESKTOP"}

    filter {"configurations:Debug_RGFW or Release_RGFW"}
        defines{"PLATFORM_DESKTOP_RGFW"}

    filter {"options:graphics=opengl43"}
        defines{"GRAPHICS_API_OPENGL_43"}

    filter {"options:graphics=opengl33"}
        defines{"GRAPHICS_API_OPENGL_33"}

    filter {"options:graphics=opengl21"}
        defines{"GRAPHICS_API_OPENGL_21"}

    filter {"options:graphics=opengl11"}
        defines{"GRAPHICS_API_OPENGL_11"}

    filter {"options:graphics=openges3"}
        defines{"GRAPHICS_API_OPENGL_ES3"}

    filter {"options:graphics=openges2"}
        defines{"GRAPHICS_API_OPENGL_ES2"}

    filter {"system:macosx"}
        disablewarnings {"deprecated-declarations"}

    filter {"system:linux"}
        defines {"_GLFW_X11"}
        defines {"_GNU_SOURCE"}
-- This is necessary, otherwise compilation will fail since
-- there is no CLOCK_MONOTOMIC. raylib claims to have a workaround
-- to compile under c99 without -D_GNU_SOURCE, but it didn't seem
-- to work. raylib's Makefile also adds this flag, probably why it went
-- unnoticed for so long.
-- It compiles under c11 without -D_GNU_SOURCE, because c11 requires
-- to have CLOCK_MONOTOMIC
-- See: https://github.com/raysan5/raylib/issues/2729

    filter{}
end

-- if you don't want to download raylib, then set this to false, and set the raylib dir to where you want raylib to be pulled from, must be full sources.
downloadRaylib = true
raylib_dir = "external/raylib-master"

workspaceName = 'MyGame'
baseName = path.getbasename(path.getdirectory(os.getcwd()));

--if (baseName ~= 'raylib-quickstart') then
    workspaceName = baseName
--end

if (os.isdir('build_files') == false) then
    os.mkdir('build_files')
end

if (os.isdir('external') == false) then
    os.mkdir('external')
end


workspace (workspaceName)
    location "../"
    configurations { "Debug", "Release", "Debug_RGFW", "Release_RGFW"}
    platforms { "x64", "x86", "ARM64"}

    defaultplatform ("x64")

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter { "platforms:Arm64" }
        architecture "ARM64"

    filter {}

    targetdir "bin/%{cfg.buildcfg}/"

if (downloadRaylib) then
    build_externals()
	end

    startproject(workspaceName)

    project (workspaceName)
        kind "ConsoleApp"
        location "build_files/Client"
        targetdir "../bin/%{cfg.buildcfg}"

        files {
            "../src/private/**.cpp",  -- All source files go here
            "../src/public/**.h",  -- All header files go here
            "../src/public/**.hpp"
        }

        filter {"system:windows", "configurations:Release", "action:gmake*"}
            kind "WindowedApp"
            buildoptions { "-Wl,--subsystem,windows" }
            files { "../resources/TimeCalc1.rc" }

        filter {"system:windows", "configurations:Release", "action:vs*"}
            kind "WindowedApp"
            entrypoint "mainCRTStartup"

        filter "action:vs*"
            debugdir "$(SolutionDir)"

        filter{}

        vpaths 
        {
            ["Header Files/*"] = { "../include/client/**.h",  "../include/client/**.hpp", "../src/client/**.h", "../src/client/**.hpp"},
            ["Source Files/*"] = {"../src/client/**.c", "src/client/**.cpp"},
            ["Resources/*"] = {"../resources/**.ico"}
        }
        files {
            "../src/client/**.c", 
            "../src/client/**.cpp", 
            "../src/client/**.h", 
            "../src/client/**.hpp", 
            "../src/client/private/**.c", 
            "../src/client/private/**.cpp", 
            "../src/client/public/**.h", 
            "../src/client/public/**.hpp", 
            "../include/**.h", 
            "../include/**.hpp",
            "../resources/**.ico" -- Add the .ico file to the project
        }
    
        includedirs { "../src/client" }
        includedirs { "../src/client/public" }
        includedirs { "../src/client/private" }
        includedirs { "../include" }
        includedirs { "external/openssl/include"}

        links {"raylib", "libssl_static", "libcrypto_static"}

        cdialect "C99"
        cppdialect "C++17"

        includedirs {raylib_dir .. "/src" }
        includedirs {raylib_dir .."/src/external" }
        includedirs {raylib_dir .."/src/external/glfw/include" }

        flags { "ShadowedVariables"}
        platform_defines()

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            dependson {"raylib"}
            links {"raylib.lib"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }

            filter "system:windows"
            defines{"_WIN32"}
            links {"winmm", "gdi32", "opengl32"}
            links {"ws2_32", "crypt32"}
            libdirs {"../bin/%{cfg.buildcfg}", "external/openssl/lib/static/"}
            files { "../resources/TimeCalc1.rc" } -- Include .rc file for all configurations on Windows
        

        filter "system:linux"
            links {"pthread", "m", "dl", "rt", "X11"}

        filter "system:macosx"
            links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

        filter{}
		

    project "raylib"
        kind "StaticLib"
    
        platform_defines()

        location "build_files/"

        language "C"
        targetdir "../bin/%{cfg.buildcfg}"

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }
        filter{}

        includedirs {raylib_dir .. "/src", raylib_dir .. "/src/external/glfw/include" }
        vpaths
        {
            ["Header Files/*"] = { "../include/**.h",  "../include/**.hpp", "../src/**.h", "../src/**.hpp"},
            ["Source Files/*"] = {"../src/**.c", "src/**.cpp"},
            ["Resources/*"] = {"../resources/**.ico"}
        }
        files {raylib_dir .. "/src/*.h", raylib_dir .. "/src/*.c"}

        removefiles {raylib_dir .. "/src/rcore_*.c"}

        filter { "system:macosx", "files:" .. raylib_dir .. "/src/rglfw.c" }
            compileas "Objective-C"

        filter{}




    project "Server"
    kind "ConsoleApp"
    location "build_files/Server"
    targetdir "../bin/%{cfg.buildcfg}/Server"

    files {
        "../src/Server/private/**.cpp",
        "../src/Server/main.cpp",         -- All .cpp files in private
        "../src/Server/public/**.h"     -- All .h files in public
    }

    print("../src/Server/private/**.cpp")

    includedirs {
        "../src/Server/public",  -- Public headers
        "../src/Server/private", -- Private headers (if needed)
        "../include"
    }
    includedirs { "external/openssl/include"}

    -- Organizing the solution view with folders
    vpaths {
        ["Server/Header Files"] = { "../src/Server/public/**.h" },
        ["Server/Source Files"] = { "../src/Server/private/**.cpp" },
        ["Server/Source Files"] = { "../src/Server/**.cpp" },
        ["Server/Resources"] = { "../resources/**.*" }
    }

    includedirs {raylib_dir .. "/src" }
    includedirs {raylib_dir .."/src/external" }
    includedirs {raylib_dir .."/src/external/glfw/include" }

    libdirs {"../bin/%{cfg.buildcfg}", "external/openssl/lib/static/"}
    
    links {"raylib", "libssl_static", "libcrypto_static"}

    cdialect "C99"
    cppdialect "C++17"

    flags { "ShadowedVariables" }
    platform_defines()

    filter "system:windows"
        defines{"_WIN32"}
        links {"ws2_32", "crypt32"}  -- Windows Sockets API (Winsock)

    filter "system:linux"
        links {"pthread", "m", "dl", "rt", "X11"}

    filter "system:macosx"
        links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

    filter{}