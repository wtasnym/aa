QT       += core gui sql widgets
QT += printsupport
QT += charts
QT += core gui sql
CONFIG   += c++17

# Désactive les API obsolètes de Qt avant Qt 6.0 (optionnel)
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# Ajoute les fichiers source de ton projet
SOURCES += \
    client.cpp \
    commande.cpp \
    connection.cpp \
    main.cpp \
    mainwindow.cpp

# Ajoute les fichiers d'en-tête de ton projet
HEADERS += \
    client.h \
    commande.h \
    connection.h \
    mainwindow.h

# Formulaires Qt Designer (UI files)
FORMS += \
    mainwindow.ui

# Règles par défaut pour le déploiement
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
