include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/kstore
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_BINARY_DIR}/lib
)

set(kmediafactory_output_SRCS
  dvdinfo.cpp
  outputplugin.cpp
  dvddirectoryobject.cpp
  dvdauthorobject.cpp
  k3bobject.cpp
)

kde4_add_ui_files(kmediafactory_output_SRCS
  dvdinfo.ui
)

kde4_automoc(${kmediafactory_output_SRCS})
kde4_add_plugin(kmediafactory_output ${kmediafactory_output_SRCS})
kde4_install_libtool_file(${PLUGIN_INSTALL_DIR} kmediafactory_output)
target_link_libraries(kmediafactory_output ${KDE4_KDECORE_LIBS} kmediafactoryinterfaces kmf kmediafactorykstore)

install(TARGETS kmediafactory_output DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kmediafactory_outputui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory_output)
install(FILES kmediafactory_output.desktop DESTINATION ${SERVICES_INSTALL_DIR})