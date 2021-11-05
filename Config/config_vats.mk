# License -> cgiturl.git/LICENSE

config: VATSMakefile
	@subdirs='$(SUBDIRS)'; for subdir in $$subdirs; do \
	  ( cd $$subdir && $(MAKE) $(MAKEDEFS) $@ ) || exit 1; \
	done

CONFIG_INCS = \
$(dir_top)/Config/definitions.mk $(dir_top)/Config/version.mk \
$(dir_top)/Config/clean.mk $(dir_top)/Config/config.mk

VATSMakefile: VATSMakefile.in $(dir_top)/config.status $(CONFIG_INCS)
	cd $(dir_top) && $(SHELL) ./config.status `echo $(subdir)/$@ | sed 's%^./%%'`

$(dir_top)/Config/definitions.mk: $(top_sdir)/Config/definitions.mk.in $(dir_top)/config.status
	cd $(dir_top) && $(SHELL) ./config.status Config/definitions.mk
