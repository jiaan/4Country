import QtQuick 2.0
import QtQuick.Controls 1.5



Item {
    anchors.fill: parent

    Text {
        id: my_name
        text: socket.name
        anchors.top: parent.top
        anchors.left: parent.left
    }

    Rectangle {
        id: records
        color: "yellow"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: container.right
        height: 150

        ListView {
            anchors.fill: parent
            anchors.margins: 15

            spacing: 5
            model: socket.players //["one", "two", "three" , "four"] //
            delegate: namecard
        }


        Component {
            id: namecard
            Rectangle {
                //color: "yellow"
                height: 25
                anchors.left: parent.left
                anchors.right: parent.right
                Text {
                    id: player_ready
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.baseline: player_score.baseline
                    text: modelData.ready?"r":"n"
                }

                Text {
                    id: player_name
                    anchors.left: player_ready.left
                    anchors.leftMargin: 10
                    anchors.baseline: player_score.baseline
                    text: modelData.name+":" }

                Text {
                    id: player_seat
                    anchors.right: player_score.left
                    anchors.rightMargin: 5
                    anchors.baseline: player_score.baseline
                    text: modelData.seat}


                Text {
                    id: player_score
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    text: modelData.score
                }

            }
        }
    }


    TextArea {
        id: chatLog        
        anchors.top: records.bottom
        anchors.bottom: chatBox.top
        anchors.left: container.right
        anchors.right:  parent.right
        text: socket.chatlog
    }

    TextArea {
        id: chatBox
        height: 100
        anchors.left: container.right
        anchors.right:  chatButton.left
        anchors.bottom: parent.bottom


    }
    Button {
        id: chatButton
        anchors.right:  parent.right

        anchors.bottom: parent.bottom

        width: 80
        height: chatBox.height
        text: "Send"
        onClicked: {
            socket.chat(chatBox.text)
            chatBox.cursorPosition = 0
            chatBox.text = ""
        }
    }

    Loader {
        id: container
        width: 700
        height: 700
        anchors.top:parent.top
        anchors.left:parent.left
        source: socket.start?"board.qml":"prepare.qml"
    }

//    Rectangle {
//        id: game_ctrl
//        property bool toggler: true
//        width: 50 ; height: 50; radius: 50
//        border.color: "black"
//        anchors.right: container.right
//        anchors.rightMargin: 72
//        anchors.bottom: container.bottom
//        anchors.bottomMargin: 100
//        MouseArea {
//            anchors.fill: parent
//            onClicked: game_ctrl.toggler = !game_ctrl.toggler
//        }
//    }
}
