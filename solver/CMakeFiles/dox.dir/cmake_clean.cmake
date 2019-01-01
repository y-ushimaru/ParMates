file(REMOVE_RECURSE
  "CMakeCache.txt"
  "cmake_install.cmake"
  "../lib/libmates-util.a"
  "CMakeFiles/dox"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/dox.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
