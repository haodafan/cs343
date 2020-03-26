IMPL:=SEM
OUTPUT:=OUTPUT

CXX = u++					# compiler
CXXFLAGS = -g -multi -O2 -std=c++11 -Wall -Wextra -MMD -D${IMPL} -D${OUTPUT} # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS = q2voter.o q2tallyvotes.o q2.o # list of object files for question 1 prefixed with "q1"
EXEC = vote

DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"

#############################################################

.PHONY : all clean

all : ${EXEC}					# build all executables

-include LockImpl

ifeq (${LOCKIMPL},${IMPL})			# same implementation type as last time ?
${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} printer.o $^ -o $@
else
ifeq (${IMPL},)					# no implementation type specified ?
# set type to previous type
IMPL=${LOCKIMPL}
${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} printer.o $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC}
${EXEC} :
	rm -f LockImpl
	touch q2tallyvotes.h
	sleep 1
	${MAKE} ${EXEC} IMPL="${IMPL}"
endif
endif

LockImpl :
	echo "LOCKIMPL=${IMPL}" > LockImpl
	sleep 1

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d ${OBJECTS} ${EXEC} LockImpl
