# dnl License -> cgiturl.git/LICENSE

mostlyclean: mostlyclean-recursive mostlyclean-here
clean:       clean-recursive       clean-here
distclean:   distclean-recursive   distclean-here
realclean:   realclean-recursive   realclean-here

mostlyclean-here:
clean-here: mostlyclean-here
distclean-here: clean-here
realclean-here: distclean-here

mostlyclean-recursive clean-recursive distclean-recursive realclean-recursive:
	@subdirs='$(SUBDIRS)'; if test -n "$$subdirs"; then \
	  ctarget=`echo $@ | sed 's/-recursive//'`; \
	  for subdir in $$subdirs; do \
	    ( cd $$subdir && $(MAKE) $(MAKEDEFS) $$ctarget ) || exit 1; \
	  done; \
	fi
