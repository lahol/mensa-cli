dist_man_MANS = mensa.1 mensarc.5 mensa-schema.5
mensa.1: manpages-local
mensarc.5: manpages-local
mensa-schema.5: manpages-local

manpages-local: mensa.1.in mensarc.5.in mensa-schema.5.in
	@for f in {mensa.1,mensarc.5,mensa-schema.5}; \
	do \
		$(SED) -e 's|__FULL_DATA_DIR__|$(datadir)|' \
			-e 's|__PACKAGE_STRING__|@PACKAGE_STRING@|' \
			$$f.in > $$f; \
	done;
clean-local:
	@$(RM) -f mensa.1 mensarc.5 mensa-schema.5
