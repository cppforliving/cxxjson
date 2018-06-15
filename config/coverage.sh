source $( dirname "${BASH_SOURCE[0]}")/common.sh
export CXXFLAGS="${CXXFLAGS} -fprofile-arcs -ftest-coverage"
export LINKFLAGS="${LINKFLAGS} -fprofile-arcs -ftest-coverage"
