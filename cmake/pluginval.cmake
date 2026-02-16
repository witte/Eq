set(PLUGINVAL_DIR "${CMAKE_BINARY_DIR}/pluginval")

if (APPLE)
    set(PLUGINVAL_URL "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_macOS.zip")
    set(PLUGINVAL_EXE "${PLUGINVAL_DIR}/pluginval.app/Contents/MacOS/pluginval")
elseif (WIN32)
    set(PLUGINVAL_URL "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_Windows.zip")
    set(PLUGINVAL_EXE "${PLUGINVAL_DIR}/pluginval.exe")
else ()
    set(PLUGINVAL_URL "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_Linux.zip")
    set(PLUGINVAL_EXE "${PLUGINVAL_DIR}/pluginval")
endif ()

if (NOT WIN32)
    set(PLUGINVAL_CHMOD_CMD chmod +x "${PLUGINVAL_EXE}")
else ()
    set(PLUGINVAL_CHMOD_CMD ${CMAKE_COMMAND} -E true) # no op on windows
endif ()

add_custom_command(
    OUTPUT "${PLUGINVAL_EXE}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGINVAL_DIR}"
    COMMAND curl -L -o "${PLUGINVAL_DIR}/pluginval.zip" "${PLUGINVAL_URL}"
    COMMAND ${CMAKE_COMMAND} -E tar xf "${PLUGINVAL_DIR}/pluginval.zip" --format=zip
    COMMAND ${CMAKE_COMMAND} -E remove "${PLUGINVAL_DIR}/pluginval.zip"
    COMMAND ${PLUGINVAL_CHMOD_CMD}
    WORKING_DIRECTORY "${PLUGINVAL_DIR}"
    COMMENT "Downloading pluginval"
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("WE ARE LINUX")
    set(PLUGINVAL_WRAPPER xvfb-run -a)
else ()
    set(PLUGINVAL_WRAPPER "")
endif ()

add_custom_target(pluginval
    COMMAND ${PLUGINVAL_WRAPPER} "${PLUGINVAL_EXE}"
        --validate-in-process
        --strictness-level 5
        "$<GENEX_EVAL:$<TARGET_PROPERTY:${PLUGIN_NAME}_VST3,JUCE_PLUGIN_ARTEFACT_FILE>>"
    DEPENDS "${PLUGINVAL_EXE}" ${PLUGIN_NAME}_VST3
    COMMENT "Running pluginval on ${PLUGIN_NAME} VST3"
)
