file(REMOVE_RECURSE
  "CMakeCache.txt"
  "cmake_install.cmake"
  "../lib/libmates-util.a"
  "CMakeFiles/check-syntax"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/check-syntax.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
