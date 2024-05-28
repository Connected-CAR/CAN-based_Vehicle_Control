TEMPLATE = app
TARGET = automotive
QT += quick quickcontrols2 \
    sql
SOURCES += \
    DatabaseManager.cpp \
    automotive.cpp

RESOURCES += \
    icons/icons.qrc \
    imagine-assets/imagine-assets.qrc \
    qml/qml.qrc \
    qtquickcontrols2.conf

target.path = $$[QT_INSTALL_EXAMPLES]/quickcontrols/imagine/automotive
INSTALLS += target

HEADERS += \
    DatabaseManager.h
