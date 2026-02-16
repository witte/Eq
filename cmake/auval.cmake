function(add_auval target)
    get_target_property(plugin_code ${target} JUCE_PLUGIN_CODE)
    get_target_property(mfr_code ${target} JUCE_PLUGIN_MANUFACTURER_CODE)
    get_target_property(au_type ${target} JUCE_AU_MAIN_TYPE_CODE)
    get_target_property(product_name ${target} JUCE_PRODUCT_NAME)

    # Strip quotes from AU type code ('aufx' -> aufx)
    string(REPLACE "'" "" au_type ${au_type})

    set(au_component "$<GENEX_EVAL:$<TARGET_PROPERTY:${target}_AU,JUCE_PLUGIN_ARTEFACT_FILE>>")
    set(au_install_dir "$ENV{HOME}/Library/Audio/Plug-Ins/Components")

    add_custom_target(auval_${target}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${au_install_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${au_component}" "${au_install_dir}/${product_name}.component"
        COMMAND killall -9 AudioComponentRegistrar || true
        COMMAND auval -v ${au_type} ${plugin_code} ${mfr_code}
        DEPENDS ${target}_AU
        COMMENT "Installing and running auval on ${target} AU"
    )
endfunction()
