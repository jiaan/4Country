import QtQuick 2.7
import QtQuick.Controls 1.5

ApplicationWindow {
    id: root
    title: qsTr("四国象棋 v0.1")

    width: 1000
    height: 700
    visible: true

    Loader {
        id: ui_loader
        anchors.fill: parent
        source: "welcome.qml"
    }
}
