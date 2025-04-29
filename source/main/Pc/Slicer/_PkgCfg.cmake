# Uncomment that line, in case of 3rd party package
# set(PkgCfg_3rd_party 1)

# Uncomment for link libraries
# set(PkgCfg_ListLinkLibraries MyLib1_lib MyLib2_lib)

set(PkgCfg_ListTargetExeFiles "${CMAKE_CURRENT_LIST_DIR}/main.cpp;")

# Optional set package specific compile options
# e.g. Enable the following line in case you need NO-Optimizations for debugging
# set(PkgCfg_TargetCompileOption -Og)

# 3rd party. Turn off all warnings
if (PkgCfg_3rd_party AND (NOT PCB_ShowWarnings))
  message(STATUS "  3rd party => warnings off")
  if (PkgCfg_TargetCompileOption)
    set(PkgCfg_TargetCompileOption -w)
  else()
    list(APPEND PkgCfg_TargetCompileOption -w)
  endif()
endif()
