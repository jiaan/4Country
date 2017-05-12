import QtQuick 2.0
import QtQuick.Controls 1.5
import QtQuick.Dialogs 1.2

Item {

    Image {
        id: welcome_image
        anchors.centerIn: parent
        source: "welcome.png"
    }

    Loader {
        id: setUp
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.verticalCenter
        sourceComponent: setName
    }

    Component {
        id: setName
        Column {
            spacing: 10

            TextField {
                id: name_box
                width: 120
                maximumLength: 16
                placeholderText: qsTr("Enter name")
            }

            Button {
                width: 120
                text: qsTr("Start")
                onClicked: {
                    if(name_box.text) {
                        socket.setName(name_box.text)
                        setUp.sourceComponent = setConnection
                    }
                }
            }
        }
    }

    Component {
        id: setConnection
        Column {
            spacing: 10

            Button {
                width: 120
                text: qsTr("Connect Game")
                onClicked: setClient.open()
            }

            Button {
                width: 120
                text: qsTr("Host Game")
                onClicked: setServer.open()
            }
        }
    }

    Dialog {
        id: setClient
        title: qsTr("Connect to")
        standardButtons: StandardButton.Cancel | StandardButton.Apply
        onApply: try_connect()

        property bool connected: socket.connected
        onConnectedChanged: {
            setClient.close()
            ui_loader.source = "game.qml"
        }

        Row {
            spacing: 8
            Label {
                text: "Server:"
                anchors.baseline: row2.verticalCenter
            }

            Row {
                id: row2
                spacing: 2
                TextField {
                    id: ipaddr
                    anchors.baseline: row2.verticalCenter
                    width: 120
                    validator: RegExpValidator {
                        regExp:/\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/
                    }
                    placeholderText: qsTr("127.0.0.1")
                }

                Label { text: ":"}

                TextField {
                    id: port
                    anchors.baseline: row2.verticalCenter
                    width: 50
                    placeholderText: qsTr("8976")
                    validator: IntValidator {bottom: 1024; top:65535}
                }
            }
        }

        function try_connect() {
            var s_ip = ipaddr.acceptableInput?ipaddr.text:"127.0.0.1"
            var s_port = port.acceptableInput?parseInt(port.text):8976
            socket.setClient(s_ip,s_port) // error box: TODO
        }
    }




        Dialog {
            id: setServer
            title: qsTr("Set up as server")
            standardButtons: StandardButton.Apply | StandardButton.Cancel
            onApply: try_bind()


            Row{
                anchors.centerIn: parent
                spacing: 8
                Label {
                    text: "Port:"
                    anchors.baseline: custom_port.baseline
                }

                TextField {
                    id: custom_port
                    width: 50
                    placeholderText: qsTr("8976")
                    validator: IntValidator {bottom: 1024; top:65535}
                }
            }

            function try_bind() {

                var s_port = custom_port.acceptableInput?
                            parseInt(custom_port.text):8976
                if(socket.setServer(s_port)) { // error box: TODO
                    setServer.close()
                    ui_loader.source = "game.qml"
                }
            }

        }



}

