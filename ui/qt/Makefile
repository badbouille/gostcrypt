# sub Makefile (qt special)

# binary maker
$(BINARY): $(COBJS) $(CXXOBJS)
	@echo "-------- Building Qt binary $@ --------"
	qmake -o qt_makefile && make -f qt_makefile
