
function SetTarget( _configuration, _platform, _basepath )
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
	local strGlew = string.format( "%s/3rdparty/glew-1.13.0/lib/%s/%s", _basepath, _configuration, platformname )
	local strSdl2 = string.format( "%s/3rdparty/SDL2-2.0.3/lib/%s/%s", _basepath, platformname, _configuration )
	local strImgui = string.format( "%s/3rdparty/imgui/bin/%s-%s-Dll", _basepath, platformname, _configuration )

	configuration {_configuration, _platform}
		--libdirs { "../../3rdparty\SDL2-2.0.3/lib/x64\$(Configuration);..\..\3rdparty\glew-1.10.0\lib\release\x64;..\..\3rdparty\tinyxml2\tinyxml2\bin\$(Platform)-$(Configuration)-Dll;..\..\3rdparty\imgui\bin\$(Platform)-$(Configuration)-Dll;$(LibraryPath)
		libdirs { strGlew, strSdl2, strImgui }
end


--------------------------------------------------------
workspace "fracstar"
	configurations { "Debug", "Release" }

	configuration "macosx"
		platforms { "native" }
	configuration "windows"
		platforms { "x32", "x64" }
	--location "src"

	---------------------------------------------------------
	project "bigball" 
		kind "StaticLib"
		language "C++"
		files { "../../engine/src/**.h", "../../engine/src/**.cpp" }

		includedirs { "../../3rdparty/SDL2-2.0.3/include", "../../3rdparty/glew-1.13.0/include", "../../3rdparty/jsmn", "../../3rdparty/imgui", "../../3rdparty" }

		defines { "_CRT_SECURE_NO_WARNINGS" }
		
		local targetpath = "../../engine"
		configuration "windows"
			SetTarget( "Debug", "x32", targetpath )
			SetTarget( "Debug", "x64", targetpath )
			SetTarget( "Release", "x32", targetpath )
			SetTarget( "Release", "x64", targetpath )
		configuration "macosx"
			SetTarget( "Debug", "native", targetpath )
			SetTarget( "Release", "native", targetpath )

	---------------------------------------------------------
	project "fracstar"
		kind "ConsoleApp"
		language "C++"
		files { "../src/**.h", "../src/**.cpp" }

		--removefiles { "3rdparty/**", "mars**" }
		targetname "fracstar"
	
		defines { "_CRT_SECURE_NO_WARNINGS", "_WINDOWS", "_USRDLL" }
		flags { "NoPCH", "NoNativeWChar", "NoEditAndContinue" }

		includedirs { "../../engine/src/", "../../3rdparty", "../../3rdparty/SDL2-2.0.3/include", "../../3rdparty/glew-1.13.0/include", "../../3rdparty/jsmn", "../../3rdparty/imgui", "../../3rdparty/eigen3" }
		links { "bigball" } --"user32", "gdi32" }

		local targetpath = ".."
		local libpath = "../.."
		configuration "windows"
			SetTarget( "Debug", "x32", targetpath )
			SetTarget( "Debug", "x64", targetpath )
			SetTarget( "Release", "x32", targetpath )
			SetTarget( "Release", "x64", targetpath )
			SetLibs( "Debug", "x32", libpath )
			SetLibs( "Debug", "x64", libpath )
			SetLibs( "Release", "x32", libpath )
			SetLibs( "Release", "x64", libpath )
		configuration "macosx"
			SetTarget( "Debug", "native", targetpath )
			SetTarget( "Release", "native", targetpath )
			SetLibs( "Debug", "native", libpath )
			SetLibs( "Release", "native", libpath )

		configuration "Debug"
			defines { "_DEBUG" }
			flags { "Symbols", "MultiProcessorCompile" }
 
		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "Symbols", "MultiProcessorCompile" } 
			optimize "On"

		configuration "macosx"
            linkoptions  { "-std=c++11" } --, "-stdlib=libc++" }
            buildoptions { "-std=c++11" } --, "-stdlib=libc++" }
			

