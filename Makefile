SRC=src
OBJ=obj
BIN=.

OBJS=

CXXFLAGS=-std=c++14 -I${SRC}
LDFLAGS=

# main target variables
MAIN=stype

# release / debug flag changes {{{
ifndef release
CXXFLAGS+=-g
else
CXXFLAGS+=-O3 -Os
endif
# }}}
# wall / nowall flag changes {{{
ifndef nowall
CXXFLAGS+=-Wall -Wextra -pedantic -Wmain -Weffc++ -Wswitch-default -Wswitch-enum
CXXFLAGS+=-Wmissing-include-dirs -Wmissing-declarations -Wunreachable-code
CXXFLAGS+=-Winline -Wfloat-equal -Wundef -Wcast-align -Wredundant-decls
CXXFLAGS+=-Winit-self -Wshadow
endif
# }}}
# werror flag changes {{{
ifdef werror
CXXFLAGS+=-Werror
endif
# }}}
# profile flag changes {{{
ifdef profile
CXXFLAGS+=-pg
LDFLAGS+=-pg
endif
# }}}

all: dirs ${BIN}/${MAIN}
dirs:
	mkdir -p ${OBJ} ${ODB}

# main target
${BIN}/${MAIN}: ${OBJ}/${MAIN}.o ${OBJS}
	${CXX} -o $@ $^ ${LDFLAGS}

# build target for source files
${OBJ}/%.o: ${SRC}/%.cpp
	${CXX} -c -o $@ $< ${CXXFLAGS}


clean:
	rm -rf ${OBJ}/*.o ${BIN}/${MAIN}

