#
# $Id$
#
# Author Kris Thielemans
# Copyright 2004- $Date$ Hammersmith Imanet Ltd
# This file is part of STIR, and is distributed under the 
# terms of the GNU Lesser General Public Licence (LGPL) Version 2.1.
#
# This file will/can be included by a makefile skeleton in a subdirectory
# Requirements:
# the skeleton should set the following variables
#    dir must be set to the subdirectory name (relative to the location of Makefile)
#    $(dir)_SOURCES must be set to a list of .cxx and/or .c files (without path)
# Result:
# targets $(dir) clean_exes_$(dir) install_$(dir)
# variables $(dir)_SOURCES (with $dir)/ prepended)
#           $(dir)_EXES, $(dir)_EXE_FILENAMES
#
# Example for dir=SUBDIR
#
# make SUBDIR will compile and link all executables
# make clean_exes_SUBDIR will remove all generated files
# make install_SUBDIR will install all executables

#$(warning including exe.mk from $(dir))

$(dir)_SOURCES:= $(addprefix $(dir)/, $($(dir)_SOURCES))
$(dir)_EXES:= \
	$(patsubst %.cxx, $(DEST)%, $(filter %.cxx, $($(dir)_SOURCES))) \
	$(patsubst %.c, $(DEST)%, $(filter %.c, $($(dir)_SOURCES)))

.PHONY: $(dir) clean_exes_$(dir) install_$(dir)

# make sure make keeps the .o file
# otherwise it will be deleted
.PRECIOUS: $(patsubst %.cxx, $(DEST)%.o, $(filter %.cxx, $($(dir)_SOURCES))) 

$(dir):  $($(dir)_EXES)

ifeq ($(SYSTEM),CYGWIN)
$(dir)_EXE_FILENAMES := $(addsuffix .exe, $($(dir)_EXES))
else
$(dir)_EXE_FILENAMES := $($(dir)_EXES)
endif


# note: see lib.mk for explanation for the $(@:...) trick
# it really is just a way to get $(dir) at build-time
clean_exes_$(dir):
	rm -f $($(@:clean_exes_%=%)_EXE_FILENAMES)
	rm -f $(DEST)$(@:clean_exes_%=%)/*.[oP]


install_$(dir): $(dir)
	cp $($(@:install_%=%)_EXE_FILENAMES) $(INSTALL_DIR)

ifneq ($(MAKECMDGOALS:clean%=clean),clean)
  -include \
	$(patsubst %.cxx, $(DEST)%.P, $(filter %.cxx, $($(dir)_SOURCES)))) \
	$(patsubst %.c, $(DEST)%.P, $(filter %.c, $($(dir)_SOURCES)))
endif

# set to some garbage such that we get an error when  the next skeleton forgets to set dir
dir := dir_not_set_in_exe.mk
