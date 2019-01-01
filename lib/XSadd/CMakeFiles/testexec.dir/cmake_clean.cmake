file(REMOVE_RECURSE
  "CMakeCache.txt"
  "cmake_install.cmake"
  "html"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/testexec.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
