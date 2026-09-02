include(FetchContent)

set(baseUrl "https://github.com/witte/JUCE-PreCompiled/releases/download/9.0.1")
FetchContent_Declare(
        JUCE-PreCompiled
        URL  "${baseUrl}/JUCE-PreCompiled-9.0.1-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE}.tar.gz"
#        URL "file:///Users/witte/Work/JUCE-PreCompiled/cmake-build-release/__package/JUCE-PreCompiled-9.0.1-Darwin-arm64-Release.tar.gz"
        # TODO: check for file hash
        # URL_HASH SHA256=...
)
FetchContent_MakeAvailable(JUCE-PreCompiled)