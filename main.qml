import QtQuick 2.6
import Board 1.0


Rectangle {
    id: root
    property int a: 80
    width: board.getWidth()*a
    height: board.getHeight()*a

    Board {
        id: board
        unitLen: a

        anchors.fill: parent
        anchors.leftMargin: unitLen/2
        anchors.topMargin: unitLen/2

        Repeater {
            model: board.chessList

                Rectangle {
                    property var chess: modelData
                    anchors.horizontalCenter: board.left
                    anchors.horizontalCenterOffset: chess.location.x*a
                    anchors.verticalCenter: board.top
                    anchors.verticalCenterOffset: chess.location.y*a

                    width: a; height: a; radius: a
                    border.color: "black"

                    Text {
                        anchors.centerIn: parent
                        text: chess.name
                    }

                    MouseArea {
                        anchors.fill: parent
                        enabled: !board.predictionMode
                        onClicked: chess.getPrediction()
                    }

                    Repeater {
                        model: chess.prediction
                        Rectangle {
                            anchors.left: parent.left
                            anchors.leftMargin: (modelData.location.x - chess.location.x)*a
                            anchors.top: parent.top
                            anchors.topMargin: (modelData.location.y - chess.location.y)*a

                            width: a; height: a; radius: a
                            border.color: "red"
                            color: "transparent"

                            MouseArea {
                                anchors.fill: parent
                                onClicked: chess.moveTo(modelData.location)
                            }
                        }
                    }

                }

        }

    }
}
