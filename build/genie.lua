
function SetTarget( _configuration, _platform, _basepath )
	--print(_configuration .. _platform)
	local platformname = _platform
	local archname = _platform
	if _platform == "x32" then
		platformname = "Win32"
		archname = "x86"
	end
	local strtarget = string.format( "%s/bin/%s_%s/", _basepath, _configuration, platformname ) 
	local strobj = string.format( "%s/intermediate/%s_%s", _basepath, _configuration, platformname ) 
	configuration {_configuration, _platform}
		targetdir( strtarget )
		objdir( strobj )
end

function SetLibs( _configuration, _platform, _basepath )
	local platformname = _platform
	local archname = _platform
	if _platform == "x32" then
		platformname = "Win32"
		archname = "x86"
	end

	local strGlew
	if _platform == "native" then
		strGlew = string.format( "%s/3rdparty/glew-2.1.0/lib", _basepath )
	else
		strGlew = string.format( "%s/3rdparty/glew-2.1.0/lib/release/%s", _basepath, platformname )
	end

	local strSdl2 = string.format( "%s/3rdparty/SDL2-2.0.16/lib/%s", _basepath, platformname)
	local strSdl2Options = string.format( "-F %s/3rdparty/SDL2-2.0.16/lib/%s", _basepath, platformname)
	
	-- add framework search path for SDL2
	if _platform == "native" then
		configuration {_configuration, _platform}
	    	linkoptions  { strSdl2Options }
	end

	configuration {_configuration, _platform}
		libdirs { strGlew, strSdl2 }
end


--------------------------------------------------------
solution "fracstar"
	configurations { "Debug", "Release" }

	platforms { "x64", "native" }

	---------------------------------------------------------
    PROJ_DIR = path.getabsolute("../../bigball")
	project "bigball" 
		kind "StaticLib"
		language "C++"

        files { PROJ_DIR .. "/engine/src/**.h", PROJ_DIR .. "/engine/src/**.cpp" }

		includedirs { PROJ_DIR .. "/3rdparty/SDL2-2.0.16/include", 
                    PROJ_DIR .. "/3rdparty/glew-2.1.0/include", 
                    PROJ_DIR .. "/3rdparty/zlib-1.2.11", 
                    PROJ_DIR .. "/3rdparty/jsmn", 
                    PROJ_DIR .. "/3rdparty/imgui", 
                    PROJ_DIR .. "/3rdparty" }

		defines { "_CRT_SECURE_NO_WARNINGS", "JSMN_STATIC" }
		
		configuration "windows"
			SetTarget( "Debug", "x64", PROJ_DIR )
			SetTarget( "Release", "x64", PROJ_DIR )
			
		configuration "macosx"
			SetTarget( "Debug", "native", PROJ_DIR )
			SetTarget( "Release", "native", PROJ_DIR )
			
		configuration "Debug"
			defines { "_DEBUG" }
			flags { "Symbols" }
 
		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "Symbols" } 

		configuration "macosx"
            buildoptions { "-std=c++11" } --, "-stdlib=libc++" }

	---------------------------------------------------------
    PROJ_DIR = path.getabsolute("..")
	project "fracstar"
		kind "ConsoleApp"
		language "C++"
		files { PROJ_DIR .. "/src/**.h", 
                PROJ_DIR .. "/src/**.cpp",
                PROJ_DIR .. "/data/**.*" }

		targetname "fracstar"
	
		defines { "_CRT_SECURE_NO_WARNINGS", "_WINDOWS", "_USRDLL", "JSMN_STATIC" }
		flags { "NoPCH", "NoNativeWChar", "NoEditAndContinue" }

        local bigballpath = path.getabsolute("../../bigball")
		includedirs { bigballpath .. "/engine/src/", 
                    bigballpath .. "/3rdparty", 
                    bigballpath .. "/3rdparty/SDL2-2.0.16/include", 
                    bigballpath .. "/3rdparty/glew-2.1.0/include", 
                    bigballpath .. "/3rdparty/zlib-1.2.11", 
                    bigballpath .. "/3rdparty/jsmn", 
                    bigballpath .. "/3rdparty/imgui", 
                    bigballpath .. "/3rdparty/eigen3", 
                    PROJ_DIR .. "/data/shader" }

		configuration "windows"	
			links { "bigball", "glew32", "sdl2", "sdl2main", "opengl32"}

		configuration "macosx"
			links { "bigball", "glew", "SDL2.framework", "OpenGL.framework" }

		local libpath = PROJ_DIR .. "/.." 
		configuration "windows"
			SetTarget( "Debug", "x64", PROJ_DIR )
			SetTarget( "Release", "x64", PROJ_DIR )
			SetLibs( "Debug", "x64", bigballpath )
			SetLibs( "Release", "x64", bigballpath )
			
		configuration "macosx"
			SetTarget( "Debug", "native", PROJ_DIR )
			SetTarget( "Release", "native", PROJ_DIR )
			SetLibs( "Debug", "native", bigballpath )
			SetLibs( "Release", "native", bigballpath )

		configuration "Debug"
			defines { "_DEBUG" }
			flags { "Symbols" } 
 
		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "Symbols"}

		configuration "macosx"
            linkoptions  { "-std=c++11" } 
            buildoptions { "-std=c++11" }
			

