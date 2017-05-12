import QtQuick 2.0
import QtQuick.Controls 1.5
import QtQuick.Dialogs 1.2

Item {
    id: prep

    property int rect_width: 280
    property int rect_hight: 175
    property bool ready: false
    property bool hasSeat: false

    Loader {
        id: box1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.verticalCenter
        anchors.topMargin: rect_width/2
        width: rect_width
        height: rect_hight
        sourceComponent: seatInfo
        property int seatNo: 1
    }

    Loader {
        id: box2
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: rect_width/2
        width: rect_hight
        height: rect_width
        sourceComponent: seatInfo
        property int seatNo: 2
    }

    Loader {
        id: box3
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.verticalCenter
        anchors.bottomMargin: rect_width/2
        width: rect_width
        height: rect_hight
        sourceComponent: seatInfo
        property int seatNo: 3
    }

    Loader {
        id: box4
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: rect_width/2
        width: rect_hight
        height: rect_width
        sourceComponent: seatInfo
        property int seatNo: 4
    }

    Component {
        id: seatInfo
        Rectangle {
            property bool occupied: socket.seat[seatNo-1]
            color: occupied?"red":"green"

            Text { text: seatNo }
            Text {
                anchors.centerIn: parent
                text: socket.seat[seatNo-1]
            }
            MouseArea {
                enabled: !prep.ready && !occupied
                anchors.fill: parent
                onClicked: {
                    socket.setSeat(seatNo)
                    prep.hasSeat = true
                }
            }
        }
    }

    Item {
        id: prep_ctrl
        width: 175 ; height: 175
        anchors.right: parent.right
        anchors.rightMargin: 35
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 35

        Column {
            anchors.centerIn: parent
            spacing:  35
            Button {
                enabled: prep.hasSeat
                text: qsTr("unseat")
                onClicked: {
                    socket.setSeat(0)
                    prep.hasSeat = false
                    if (prep.ready) {
                        socket.setReady(false)
                        prep.ready = false
                    }
                }

            }
            Button {
                enabled: prep.hasSeat
                text: prep.ready?qsTr("Cancel"):qsTr("Ready")
                onClicked: {
                    socket.setReady(!prep.ready)
                    prep.ready = ! prep.ready
                }
            }
        }
    }
}
