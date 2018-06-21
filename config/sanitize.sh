source $( dirname "${BASH_SOURCE[0]}")/common.sh
export CXXFLAGS="${CXXFLAGS} -fsanitize=address -fsanitize=undefined"
export LINKFLAGS="${LINKFLAGS} -fsanitize=address -fsanitize=undefined"
