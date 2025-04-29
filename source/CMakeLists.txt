
# Process a project's packages depending on the list of packages
# e.g. SPI:VAR1|-O1 => PCB_PkgName = SPI; PCB_PkgVar = VAR1; PCB_PkgBuildOptions =-O1
# Can be used by the packages, to use config specific files/options
foreach(lPackage ${PCB_ProjectPackageList})

    # Try to split at '|'
    string(REPLACE "|" ";" lLstPackage ${lPackage})
    list(LENGTH lLstPackage liPackageLength)

    # No '|', so get only the first part to PCB_PkgNameAndCfg, e.g. SPI:VAR1|-O1 => SPI:VAR1
    if (${liPackageLength} STREQUAL 1)
      list (GET lLstPackage 0 PCB_PkgNameAndCfg)
      string(STRIP PCB_PkgNameAndCfg ${PCB_PkgNameAndCfg})
      unset(PCB_PkgBuildOptions)
    endif()

    # if theres is a '|', then 
    #    - get the first  part to PCB_PkgNameAndCfg,   e.g. SPI:VAR1|-O1 => SPI:VAR1
    #    - get the second part to PCB_PkgBuildOptions, e.g. SPI:VAR1|-O1 => -O1
    if (${liPackageLength} STREQUAL 2)
      list (GET lLstPackage 0 PCB_PkgNameAndCfg)
      string(STRIP PCB_PkgNameAndCfg, ${PCB_PkgNameAndCfg})
      list (GET lLstPackage 1 PCB_PkgBuildOptions)
      string(STRIP PCB_PkgBuildOptions ${PCB_PkgBuildOptions})
    endif()

    #message(STATUS "  ***PCB_PkgNameAndCfg: ${PCB_PkgNameAndCfg}")

    # Try to split at ':'
    string(REPLACE ":" ";" lLstPackage ${PCB_PkgNameAndCfg})

    list (GET lLstPackage 0 PCB_PkgName)
    string(STRIP PCB_PkgName ${PCB_PkgName})

    # if theres is a ':', then 
    #    - get the first  part to PCB_PkgName,  e.g. SPI:VAR1 => SPI
    #    - get the second part to PCB_PkgVar,   e.g. SPI:VAR1 => VAR1
    string(FIND ${PCB_PkgNameAndCfg} ":" liSeparatorPos)
    if (NOT (${liSeparatorPos} STREQUAL -1))
      MATH(EXPR liSeparatorPos "${liSeparatorPos} + 1")
      string(SUBSTRING ${PCB_PkgNameAndCfg} ${liSeparatorPos} -1 PCB_PkgVar)
      string(STRIP PCB_PkgVar ${PCB_PkgVar})
    else()
      unset(PCB_PkgVar)
    endif()

    #message(STATUS "  ***PCB_PkgName: ${PCB_PkgName}")
    #message(STATUS "  ***PCB_PkgVar: ${PCB_PkgVar}")

    message(STATUS "Add package: ${lPackage}")
    add_subdirectory(${PCB_PkgName})
endforeach()


#pass through parent scope
set(PCB_ListTargetExeFiles "${PCB_ListTargetExeFiles}" PARENT_SCOPE)
set(PCB_ListTargetIncDirs  "${PCB_ListTargetIncDirs}"  PARENT_SCOPE)
set(PCB_ListTargetLinkLibs "${PCB_ListTargetLinkLibs}" PARENT_SCOPE)
