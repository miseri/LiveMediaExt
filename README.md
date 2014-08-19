Note: this project has the following dependencies:

boost 1.55: http://www.boost.org
google glog 0.3.3: https://code.google.com/p/google-glog/
live555: http://www.live555.com
cpp-util: https://github.com/miseri/cpp-util

Of these we don't mirror boost. Instead the environmental 
variables BOOST_INCLUDE_DIR and BOOST_LIB_DIR need to be defined.

Environmental variable:
BOOST_INCLUDE_DIR: boost include directory.
BOOST_LIB_DIR: directory where compiled .lib files can be found.

The external can be checked out using the following command:
git submodule add git://github.com/miseri/live555-CMake.git externals/live555-CMake
git submodule add git://github.com/miseri/cpp-util.git externals/cpp-util
git submodule add git://github.com/miseri/glog.git externals/glog
