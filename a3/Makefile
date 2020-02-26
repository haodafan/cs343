OPT:= -O2 -Multi -nodebug
PIMPL:=CFOR
BIMPL:=NOBUSY
TYPE:=int
SENTINEL:=-1

CXX = u++					# compiler
CXXFLAGS = -g ${OPT} -Wall -Wextra -MMD -D"${PIMPL}" -D"${BIMPL}" \
		   -DTYPE="${TYPE}" -DSENTINEL="${SENTINEL}" # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS1 = q2*.o # object files forming 1st executable with prefix "q2"
EXEC1 = polymult				# 1st executable name

OBJECTS2 = q3*.o # object files forming 2nd executable with prefix "q3"
EXEC2 = buffer					# 2nd executable name

OBJECTS = ${OBJECTS1} ${OBJECTS2}		# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}			# all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables

-include PolyImpl

ifeq (${POLYIMPL},${PIMPL})			# same implementation type as last time ?
${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@
else
ifeq (${PIMPL},)				# no implementation type specified ?
# set type to previous type
PIMPL=${POLYIMPL}
${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC1}
${EXEC1} :
	rm -f PolyImpl
	touch q2polymult.h
	sleep 1
	${MAKE} ${EXEC1} PIMPL=${PIMPL}
endif
endif

PolyImpl :
	echo "POLYIMPL=${PIMPL}" > PolyImpl
	sleep 1

-include BufImpl

ifeq (${BUFIMPL},${BIMPL})			# same implementation type as last time ?
${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@
else
ifeq (${BIMPL},)				# no implementation type specified ?
# set type to previous type
BIMPL=${BUFIMPL}
${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC2}
${EXEC2} :
	rm -f BufImpl
	touch q3buffer.h
	sleep 1
	${MAKE} ${EXEC2} BIMPL="${BIMPL}"
endif
endif

BufImpl :
	echo "BUFIMPL=${BIMPL}" > BufImpl
	sleep 1

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} PolyImpl BufImpl
