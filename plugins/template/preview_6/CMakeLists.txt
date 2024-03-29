set(template
  preview_6.kmft
)

set(template_files
  mimetype
  template.xml
  settings.kcfg
  settings.ui
  icon.png
  preview.jpg
  images/button.svgz
  images/prev.svgz
  images/back.svgz
  images/next.svgz
)

file(GLOB dirs "${CMAKE_CURRENT_SOURCE_DIR}/locale/??")
foreach(dir ${dirs})
  list(APPEND po_files ${dir}/LC_MESSAGES/template.po ${dir}/LC_MESSAGES/ui.po)
endforeach(dir)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${template}
  COMMAND ${CMAKE_COMMAND}
      -Dtemplate="${template}"
      -Dtemplate_files="${template_files}"
      -Dsrc="${CMAKE_CURRENT_SOURCE_DIR}"
      -Ddest="${CMAKE_CURRENT_BINARY_DIR}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/../template.cmake"
  DEPENDS ${template_files} ${po_files}
)

add_custom_target(preview_6_template ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${template}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${template} DESTINATION ${DATA_INSTALL_DIR}/kmediafactory_template)
