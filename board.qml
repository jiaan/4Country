import QtQuick 2.0
import Board 1.0

Board {

    id: grid

    anchors.top: parent.top
    anchors.left: parent.left
    anchors.leftMargin: a
    anchors.topMargin: a

    a: 35
    property bool predictionMode: false
    width: length
    height: length
    Component.onCompleted: connectSocket(socket.socket)

    Repeater {
        model: grid.chessList

        Rectangle {
            property var chess: modelData
            anchors.horizontalCenter: grid.left
            anchors.horizontalCenterOffset: chess.location.x*a
            anchors.verticalCenter: grid.top
            anchors.verticalCenterOffset: chess.location.y*a

            width: a; height: a; radius: a
            border.color: "black"
            color: chess.color

            Text {
                anchors.centerIn: parent
                text: chess.name
            }

            MouseArea {
                anchors.fill: parent
                enabled: socket.turn && !grid.predictionMode && chess.isMine
                onClicked: {
                    grid.predictionMode = true
                    chess.getPrediction()
                }
            }

            Repeater {
                model: chess.prediction
                Rectangle {
                    anchors.left: parent.left
                    anchors.leftMargin: (modelData.location.x - chess.location.x)*a
                    anchors.top: parent.top
                    anchors.topMargin: (modelData.location.y - chess.location.y)*a

                    width: a; height: a; //radius: a
                    border.color: "red"
                    color: "transparent"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            grid.predictionMode = false
                            chess.moveTo(modelData.location)
                        }
                    }
                }
            }

        }

    }

}


