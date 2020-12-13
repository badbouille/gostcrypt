# sub Makefile (qt special)

# binary maker
.PHONY: $(BINARY)
$(BINARY):
	@echo "-------- Building Qt binary $@ --------"
	qmake -o Makefile_qt
	$(MAKE) -f Makefile_qt

# modules that have to be installed
# qt5-default
# qml-module-qtquick2 qml-module-qtquick-controls qml-module-qtquick-layouts qml-module-qtquick-dialogs
# qml-module-qt-labs-folderlistmodel qml-module-qt-labs-settings