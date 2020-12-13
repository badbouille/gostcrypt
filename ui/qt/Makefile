# sub Makefile (qt special)

# binary maker
$(BINARY): $(COBJS) $(CXXOBJS)
	@echo "-------- Building Qt binary $@ --------"
	qmake -o Makefile_qt
	$(MAKE) -f Makefile_qt

# modules that have to be installed
# qt5-default
# qml-module-qtquick2 qml-module-qtquick-controls
#