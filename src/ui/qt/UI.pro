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
    DEFINES += PLATFORM_LINUX
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

win32|win64 {
    DEFINES += PLATFORM_WINDOWS
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
    inc/UserInterface.h \
    inc/QThreads.h

SOURCES += \
    src/main.cpp \
    src/UserSettings.cpp \
    src/TranslationApp.cpp \
    src/GraphicInterface.cpp \
    src/SecureTextField.cpp \
    src/UserInterface.cpp \
    src/QThreads.cpp

LIBS += \
        $${STATIC_LIBSD}/core.a \
        $${STATIC_LIBSD}/fuse.a \
        $${STATIC_LIBSD}/volume.a \
        $${STATIC_LIBSD}/crypto.a \
        $${STATIC_LIBSD}/common.a \
        -ldl -lfuse -lpthread -lrt

TARGET = $${BNAME}

DESTDIR = $${DDIR}
OBJECTS_DIR = $${OBJD}
MOC_DIR = $${OBJD}

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
    SOURCES += qml/* \
               qml/dialogs/* \
               qml/frames/* \
               qml/ressource/* \
               qml/wizard/* \
}

DISTFILES += \
    qml/ressource/separator.png \
    qml/dialogs/GSLanguage.qml \
    qml/SecureTextField.qml \
    qml/wizard/StepComponent.qml \
    qml/SubWindow.qml \
    qml/wizard/NextPreviousButton.qml \
    qml/dialogs/FavoriteVolumeOptions.qml \
    qml/dialogs/OpenVolume.qml \
    qml/dialogs/Performance.qml \
    qml/dialogs/Preferences.qml \
    qml/dialogs/DefaultKeyfile.qml \
    qml/dialogs/Contributors.qml \
    qml/dialogs/ChangePassword.qml \
    qml/dialogs/Benchmark.qml \
    qml/dialogs/License.qml \
    qml/dialogs/KeyfileGenerator.qml \
    qml/dialogs/TestVectors.qml \
    translations/gostcrypt_ar.ts \
    qml/dialogs/RestoreHeaderVolume.qml \
    qml/dialogs/BackupHeaderVolume.qml \
    qml/AbstractTheme.qml
