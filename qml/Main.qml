import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window

    visible: false
    title: "Clipboard"
    width: 400
    height: 550
    minimumWidth: 400
    maximumWidth: 400
    minimumHeight: 550
    maximumHeight: 550
    
    color: "#171a21"

    readonly property color backgroundColor: "#171a21"
    readonly property color cardColor: "#222630"
    readonly property color cardHoverColor: "#2c3240"
    readonly property color borderColor: "#343b49"
    readonly property color accentColor: "#80bfff"
    readonly property color textColor: "#edf0f5"
    readonly property color secondaryTextColor: "#929baa"

    property bool keyboardNavigation: false
    property point lastMousePos: Qt.point(-1, -1)

    Timer {
        id: closeTimer
        interval: 100
        repeat: false

        onTriggered: {
            if (!window.active) {
                window.close()
            }
        }
    }

    onActiveChanged: {
        if (!active) {
            closeTimer.start()
        } else {
            closeTimer.stop()
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 14

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: borderColor
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "Недавние"
                color: textColor
                font.pixelSize: 13
                font.bold: true
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: clipboardList.count + " элементов"
                color: secondaryTextColor
                font.pixelSize: 11
            }
        }

        ListView {
            id: clipboardList

            focus: true
            keyNavigationEnabled: true
            keyNavigationWraps: true
            
            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true
            spacing: 8
            highlightMoveDuration: 99

            model: clipboard.model

            delegate: Rectangle {
                id: itemCard

                required property int index
                required property var model

                readonly property bool isImage:
                    String(model.MimeType).startsWith("image/")

                width: clipboardList.width
                height: isImage ? 190 : 76

                radius: 10
                color: window.keyboardNavigation
                    ? (ListView.isCurrentItem ? cardHoverColor : cardColor)
                    : (mouseArea.containsMouse ? cardHoverColor : cardColor)



                border.width: 1
                border.color: borderColor

                Behavior on color {
                    ColorAnimation { duration: 120 }
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // Иконка типа данных
                    Rectangle {
                        Layout.preferredWidth: 38
                        Layout.preferredHeight: 38

                        radius: 9
                        color: "#303d50"

                        Text {
                            anchors.centerIn: parent

                            text: itemCard.isImage ? "🖼" : "📄"
                            font.pixelSize: 19
                        }
                    }

                    // Содержимое
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 5

                        // Предпросмотр изображения
                        Image {
                            visible: itemCard.isImage

                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            
                            source: itemCard.isImage ? itemCard.model.Preview : ""
                            fillMode: Image.PreserveAspectFit
                            asynchronous: true
                            cache: true

                            sourceSize.width: 500
                            sourceSize.height: 500
                        }

                        // Предпросмотр текста
                        Label {
                            visible: !itemCard.isImage

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            text: itemCard.model.Preview || "Пустой элемент"
                            color: textColor

                            font.pixelSize: 12
                            font.bold: true

                            elide: Text.ElideRight
                            maximumLineCount: 2
                            wrapMode: Text.Wrap
                            verticalAlignment: Text.AlignVCenter
                        }

                        Label {
                            Layout.fillWidth: true

                            text: itemCard.model.MimeType || "Неизвестный тип"
                            color: secondaryTextColor
                            font.pixelSize: 10

                            elide: Text.ElideRight
                        }
                    }

                    // Кнопки управления
                    ColumnLayout {
                        spacing: 0

                        ToolButton {
                            Layout.preferredWidth: 30
                            Layout.preferredHeight: 27

                            text: itemCard.model.Pinned ? "★" : "☆"
                            font.pixelSize: 17

                            palette.buttonText: itemCard.model.Pinned
                                                ? "#ffc857"
                                                : secondaryTextColor

                            onClicked: {
                                clipboard.SetPinned(
                                    itemCard.model.ItemID,
                                    !itemCard.model.Pinned
                                )
                            }
                        }

                        ToolButton {
                            Layout.preferredWidth: 30
                            Layout.preferredHeight: 27

                            text: "×"
                            font.pixelSize: 19
                            palette.buttonText: secondaryTextColor

                            onClicked: {
                                clipboard.RemoveItem(itemCard.model.ItemID)
                            }
                        }
                    }
                }

                MouseArea {
                    id: mouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    z: -1

                    onClicked: {
                        clipboard.CopyItem(itemCard.model.ItemID)
                        window.close()
                    }

                    onPositionChanged: (mouse) => 
                    {
                        var globalPos = mapToItem(window.contentItem, mouse.x, mouse.y)
                        if (Math.abs(globalPos.x - window.lastMousePos.x) > 0.5 ||
                            Math.abs(globalPos.y - window.lastMousePos.y) > 0.5) 
                        {
                            window.lastMousePos = globalPos
                            window.keyboardNavigation = false
                            clipboardList.currentIndex = itemCard.index
                        }
                    }
                }

                Keys.onPressed: (event) => 
                {
                    if (event.modifiers === (Qt.ControlModifier) && event.key === Qt.Key_X) 
                    {
                        clipboard.RemoveItem(itemCard.model.ItemID)
                    }

                    if (event.modifiers === Qt.AltModifier &&
                        event.nativeScanCode === 53) {
                        clipboard.SetPinned(
                            itemCard.model.ItemID,
                            !itemCard.model.Pinned
                        )
                    }
                }
            }

            Label {
                anchors.centerIn: parent

                visible: clipboardList.count === 0

                text: "Здесь пока пусто\n\nСкопируй что-нибудь,\nчтобы добавить элемент"
                color: secondaryTextColor

                font.pixelSize: 13
                horizontalAlignment: Text.AlignHCenter
                lineHeight: 1.5
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            Keys.onReturnPressed: (event) => 
            {
                if (currentItem !== null) {
                    clipboard.CopyItem(currentItem.model.ItemID)
                    window.close()
                    event.accepted = true    
                }                
            }

            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Up ||
                    event.key === Qt.Key_Down ||
                    event.key === Qt.Key_Left ||
                    event.key === Qt.Key_Right) {
                    keyboardNavigation = true
                }

                if (event.modifiers === (Qt.ControlModifier | Qt.ShiftModifier) && event.key === Qt.Key_X)
                {
                    clipboard.ClearHistory()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: borderColor
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "CliphistGUI"
                color: secondaryTextColor
                font.pixelSize: 10
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: "Очистить историю"
                font.pixelSize: 11

                palette.buttonText: "#ff8585"

                background: Rectangle {
                    radius: 7
                    color: parent.hovered ? "#3b292d" : "transparent"
                }

                onClicked: clipboard.ClearHistory()
            }
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: window.close()
    }
}

