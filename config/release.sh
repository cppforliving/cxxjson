source $( dirname "${BASH_SOURCE[0]}")/common.sh
export CXXFLAGS="${CXXFLAGS} -O3 -march=native"
export DEFINES="${DEFINES} NDEBUG"
export LINKFLAGS="${LINKFLAGS} -flto"
