# Creates the SIP file which includes all
# auto-generated SIP files (by sipify.pl script)


# cpp files + module name


MACRO(GENERATE_SIP_INCLUDE_FILE MODULE_NAME)

MESSAGE(${MODULE_NAME})

STRING(TOUPPER ${MODULE_NAME} _uppercase_module)
STRING(CONCAT _cpp_source_var "QGIS_" ${_uppercase_module} "_SRCS")
get_property(CPP_SOURCE_FILES GLOBAL PROPERTY ${_cpp_source_var})

MESSAGE("${CPP_SOURCE_FILES}")

SET(_file ${CMAKE_SOURCE_DIR}/python/${MODULE_NAME}/${MODULE_NAME}_auto_.sip)

file(WRITE ${_file} "# Include auto-generated SIP files\n" )

foreach(_cpp_file ${CPP_SOURCE_FILES})
  MESSAGE(${_cpp_file})
  STRING(REGEX_REPLACE "\.cpp$" "src\1.sip" _header ${_cpp_file})
  STRING(REGEX_REPLACE "^src\/(.*)\.cpp$" "\1.sip" _sip ${_cpp_file})
    file(STRINGS "${_header}" skip REGEX "SIP_NO_FILE")
    if(!skip)
      file(APPEND ${_file} "kkk" )
    endif(!skip)
endforeach()


ENDMACRO(GENERATE_SIP_INCLUDE_FILE)
