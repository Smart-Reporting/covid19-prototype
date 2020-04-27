cmake_minimum_required(VERSION 3.12)
project(DICOMAnonymizer)

set(WA_BUILD ON)
string(FIND "${CMAKE_TOOLCHAIN_FILE}" "Emscripten" EMPOS)
if (${EMPOS} LESS 0)
  message("C++ native compilation")
  set(WA_BUILD OFF)
else()
  message("WebAssembly compilation")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register")

if (WA_BUILD)
  set(WASM_FLAGS "-s WASM=1")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WASM_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WASM_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString']  --js-library ${CMAKE_SOURCE_DIR}/library.js")
  set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/config_wa)
else()
  set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/config_cpp)
endif()

set(DCMTK_SOURCES_DIR "dcmtk-3.6.0")

add_definitions(
  -DHAVE_CONFIG_H=1
  -DLOG4CPLUS_DISABLE_FATAL=1         # Disable the log4cplus support
  )

include_directories(
  ${CONFIG_DIR}                       # To include the "dcmtk" folder
  ${DCMTK_SOURCES_DIR}/oflog/include
  ${DCMTK_SOURCES_DIR}/ofstd/include
  ${DCMTK_SOURCES_DIR}/dcmdata/include
  )

set(DCMTK_SOURCES_OFSTD
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofcmdln.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofconapp.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofcond.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofconfig.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofconsol.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofcrc32.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofdate.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofdatime.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/offname.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/oflist.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofstd.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofstring.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/ofthread.cc
  ${DCMTK_SOURCES_DIR}/ofstd/libsrc/oftime.cc
  )

set(DCMTK_SOURCES_DCMDATA
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/cmdlnarg.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcbytstr.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcchrstr.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dccodec.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcdatset.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcddirif.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcdicdir.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcdicent.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcdictbi.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcdict.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcdictzz.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcdirrec.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcelem.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcerror.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcfilefo.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dchashdi.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcistrma.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcistrmb.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcistrmf.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcistrmz.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcitem.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dclist.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcmetinf.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcobject.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcostrma.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcostrmb.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcostrmf.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcostrmz.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcpath.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcpcache.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcpixel.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcpixseq.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcpxitem.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcrleccd.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcrlecce.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcrlecp.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcrledrg.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcrleerg.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcrlerp.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcsequen.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcstack.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcswap.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dctag.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dctagkey.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dctypes.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcuid.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvm.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrae.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvras.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrat.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvr.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrcs.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrda.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrds.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrdt.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrfd.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrfl.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvris.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrlo.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrlt.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrobow.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrof.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrpn.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrpobw.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrsh.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrsl.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrss.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrst.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrtm.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrui.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrul.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrulup.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrus.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcvrut.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcwcache.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/dcxfer.cc
#  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/mkdeftag.cc
#  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/mkdictbi.cc
  ${DCMTK_SOURCES_DIR}/dcmdata/libsrc/vrscan.cc
  ${CONFIG_DIR}/dcmtk/vrscanl.c
  )

if(WIN32 AND NOT CYGWIN)
  set(OFLOG_PLATFORM_LIBRARIES
      ${DCMTK_SOURCES_DIR}/oflog/libsrc/winsock.cc
      ${DCMTK_SOURCES_DIR}/oflog/libsrc/windebap.cc
      ${DCMTK_SOURCES_DIR}/oflog/libsrc/winconap.cc
      )
else(WIN32 AND NOT CYGWIN)
  set(OFLOG_PLATFORM_LIBRARIES
      ${DCMTK_SOURCES_DIR}/oflog/libsrc/unixsock
      )
endif(WIN32 AND NOT CYGWIN)

set(DCMTK_SOURCES_OFFLOG
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/apndimpl.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/appender.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/config.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/consap.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/factory.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/fileap.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/filter.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/globinit.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/hierarchy.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/hierlock.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/layout.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/lloguser.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/logevent.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/logger.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/logimpl.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/loglevel.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/loglog.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/ndc.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/ntelogap.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/nullap.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/objreg.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/oflog.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/patlay.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/pointer.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/property.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/rootlog.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/sleep.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/sockbuff.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/socketap.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/socket.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/strhelp.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/syncprims.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/syslogap.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/threads.cc
    ${DCMTK_SOURCES_DIR}/oflog/libsrc/timehelp.cc
    )

if (WA_BUILD)
  add_library(dcmtk
      ${DCMTK_SOURCES_OFSTD}
      ${DCMTK_SOURCES_DCMDATA}
      )
else()
  add_library(dcmtk
      ${DCMTK_SOURCES_OFSTD}
      ${DCMTK_SOURCES_DCMDATA}
      ${DCMTK_SOURCES_OFFLOG}
      ${OFLOG_PLATFORM_LIBRARIES}
    )
endif()

set(SOURCE 
    anonymize.cpp
    stripDataset.cpp
    stripDataset.h
    utils.h
    utils.cpp
    )

if (WA_BUILD)
  # WebAssembly target build
  # Regenerate a dummy "library.c" file each time the "library.js" file
  # is modified, so as to force a new execution of the linking
  add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/library.c"
      COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_BINARY_DIR}/library.c" ""
      DEPENDS "${CMAKE_SOURCE_DIR}/library.js")

  add_executable(${PROJECT_NAME} ${SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/library.c)

  target_link_libraries(${PROJECT_NAME} dcmtk)

  install(TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_SOURCE_DIR}/dist
    )

  install(FILES
    ${CMAKE_SOURCE_DIR}/index.html
    ${CMAKE_SOURCE_DIR}/style.css
    ${CMAKE_SOURCE_DIR}/wrap.js
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wasm
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js
    DESTINATION ${CMAKE_SOURCE_DIR}/dist
    )
else()
  # c++ native target build
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/bin )

  add_subdirectory(tests)
  add_library(cpplib ${SOURCE})
  add_executable(${PROJECT_NAME} main.cpp)
  target_link_libraries(${PROJECT_NAME} cpplib dcmtk)
endif()

