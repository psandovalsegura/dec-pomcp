bin_PROGRAMS = pomcp

pomcp_SOURCES = \
decrocksample.cpp \
dectiger.cpp \
battleship.cpp \
beliefstate.cpp \
coord.cpp \
experiment.cpp \
main.cpp \
mcts.cpp \
network.cpp \
node.cpp \
pocman.cpp \
rocksample.cpp \
simulator.cpp \
tag.cpp \
testsimulator.cpp \
utils.cpp

noinst_HEADERS = \
decrocksample.h \
dectiger.h \
battleship.h \
beliefstate.h \
coord.h \
experiment.h \
grid.h \
history.h \
mcts.h \
memorypool.h \
network.h \
node.h \
pocman.h \
rocksample.h \
simulator.h \
statistic.h \
tag.h \
testsimulator.h \
utils.h

pomcp_LDFLAGS = $(BOOST_LDFLAGS)

pomcp_LDADD = \
$(BOOST_PROGRAM_OPTIONS_LIB) \
@LIBS@

pomcp_CPPFLAGS = \
$(BOOST_CPPFLAGS) \
@CPPFLAGS@

DISTCLEANFILES = *~
