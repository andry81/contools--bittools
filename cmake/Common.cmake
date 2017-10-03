function(exclude_paths_from_path_list include_list_var path_list exclude_path_list verbose_flag)
  if(verbose_flag)
    message(STATUS "(**) exclude_paths_from_path_list: exclude list: ${exclude_path_list}")
  endif()

  set(include_list "")

  foreach(path ${path_list})
    set(_excluded 0)
    foreach(exclude_path ${exclude_path_list})
      if("${path}" MATCHES "(.*)${exclude_path}(.*)")
        if(verbose_flag)
          message(STATUS "(**) exclude_paths_from_path_list: excluded: ${path}")
        endif()
        set(_excluded 1)
        break()
      endif()
    endforeach()
    if(NOT _excluded)
      list(APPEND include_list "${path}")
    endif()
  endforeach()

  if(verbose_flag)
    message(STATUS "(**) exclude_paths_from_path_list: include list: ${include_list}")
  endif()

  set(${include_list_var} ${include_list} PARENT_SCOPE)
endfunction()

function(exclude_file_path_from_path_list include_list_var path_list exclude_file_path_list verbose_flag)
  if(verbose_flag)
    message(STATUS "(**) exclude_file_path_from_path_list: exclude list: ${exclude_file_path_list}")
  endif()

  set(include_list "")

  foreach(path ${path_list})
    set(_excluded 0)
    foreach(exclude_file_path ${exclude_file_path_list})
      if("${path}|" MATCHES "(.*)${exclude_file_path}\\|")
        if(verbose_flag)
          message(STATUS "(**) exclude_file_path_from_path_list: excluded: ${path}")
        endif()
        set(_excluded 1)
        break()
      endif()
    endforeach()
    if(NOT _excluded)
      list(APPEND include_list "${path}")
    endif()
  endforeach()

  if(verbose_flag)
    message(STATUS "(**) exclude_file_path_from_path_list: include list: ${include_list}")
  endif()

  set(${include_list_var} ${include_list} PARENT_SCOPE)
endfunction()