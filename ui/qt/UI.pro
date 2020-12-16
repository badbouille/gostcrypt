#-------------------------------------------------
#
# Project created by QtCreator 2017-07-17T17:26:10
#
#-------------------------------------------------

QT += qml quick widgets

TEMPLATE = app
CONFIG += c++17
CONFIG += console

INCLUDEPATH += inc ../../common/ince ../../crypto/ince ../../volume/ince ../../fuse/ince ../../core/ince ..

DEFINES += FUSE_USE_VERSION=29 \
        _FILE_OFFSET_BITS=64 \
        _LARGEFILE_SOURCE \
        _LARGE_FILESCFLAGS

unix {
    DEFINES += GST_UNIX
    linux {
        DEFINES += GST_LINUX
    }
    macx {
        DEFINES += GST_BSD GST_MACOSX
    }
    freebsd {
        DEFINES += GST_BSD GST_FREEBSD
    }
    solaris {
        DEFINES += GST_SOLARIS
    }
}

DEFINES += QT_DEPRECATED_WARNINGS QT_NO_FOREACH


RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    inc/UserSettings.h \
    inc/DragWindowProvider.h \
    inc/TranslationApp.h \
    inc/GraphicInterface.h \
    inc/SecureTextField.h \
    inc/UserInterface.h

SOURCES += \
    src/main.cpp \
    src/UserSettings.cpp \
    src/TranslationApp.cpp \
    src/GraphicInterface.cpp \
    src/SecureTextField.cpp \
    src/UserInterface.cpp

LIBS += \
        ../../bin/core.a \
        ../../bin/fuse.a \
        ../../bin/volume.a \
        ../../bin/crypto.a \
        ../../bin/common.a \
        -ldl -lfuse -lext2fs -lrt -lpthread -lcom_err

TARGET = gc_qt

DESTDIR = ../../bin
OBJECTS_DIR = ../../obj/qt
MOC_DIR = ../../obj/qt

lupdate_hack{
    SOURCES += qml/*.qml \
        qml/*.js
}
TRANSLATIONS =  translations/gostcrypt_en.ts \
                translations/gostcrypt_fr.ts \
                translations/gostcrypt_es.ts \
                translations/gostcrypt_it.ts \
                translations/gostcrypt_ru.ts \
                translations/gostcrypt_ar.ts

translation {
    SOURCES += UI/* \
               UI/dialogs/* \
               UI/frames/* \
               UI/ressource/* \
               UI/wizard/* \
}

DISTFILES += \
    UI/ressource/separator.png \
    UI/dialogs/GSLanguage.qml \
    UI/SecureTextField.qml \
    UI/wizard/StepComponent.qml \
    UI/SubWindow.qml \
    UI/wizard/NextPreviousButton.qml \
    UI/dialogs/FavoriteVolumeOptions.qml \
    UI/dialogs/OpenVolume.qml \
    UI/dialogs/Performance.qml \
    UI/dialogs/Preferences.qml \
    UI/dialogs/DefaultKeyfile.qml \
    UI/dialogs/Contributors.qml \
    UI/dialogs/ChangePassword.qml \
    UI/dialogs/Benchmark.qml \
    UI/dialogs/License.qml \
    UI/dialogs/KeyfileGenerator.qml \
    UI/dialogs/TestVectors.qml \
    translations/gostcrypt_ar.ts \
    UI/dialogs/RestoreHeaderVolume.qml \
    UI/dialogs/BackupHeaderVolume.qml \
    UI/AbstractTheme.qml
