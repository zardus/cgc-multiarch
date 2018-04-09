TOPTARGETS := all clean

SUBDIRS := $(wildcard cgc-challenge-corpus/?????_?????)

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	sed -i -e "s|^include .*|include ../../single-mk|" $@/Makefile
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
