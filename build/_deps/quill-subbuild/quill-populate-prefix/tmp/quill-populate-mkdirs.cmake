# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/huzaifa/loggerLib/third_party/quill")
  file(MAKE_DIRECTORY "/home/huzaifa/loggerLib/third_party/quill")
endif()
file(MAKE_DIRECTORY
  "/home/huzaifa/loggerLib/build/_deps/quill-build"
  "/home/huzaifa/loggerLib/build/_deps/quill-subbuild/quill-populate-prefix"
  "/home/huzaifa/loggerLib/build/_deps/quill-subbuild/quill-populate-prefix/tmp"
  "/home/huzaifa/loggerLib/build/_deps/quill-subbuild/quill-populate-prefix/src/quill-populate-stamp"
  "/home/huzaifa/loggerLib/build/_deps/quill-subbuild/quill-populate-prefix/src"
  "/home/huzaifa/loggerLib/build/_deps/quill-subbuild/quill-populate-prefix/src/quill-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/huzaifa/loggerLib/build/_deps/quill-subbuild/quill-populate-prefix/src/quill-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/huzaifa/loggerLib/build/_deps/quill-subbuild/quill-populate-prefix/src/quill-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
