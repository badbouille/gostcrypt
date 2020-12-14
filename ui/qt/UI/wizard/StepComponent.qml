import QtQuick 2.7
import "../" as UI

Rectangle {
    id: top
    property bool checked: false

    color: (checked === true) ? custompalette.roundFilled : custompalette.round
    width: 12
    height: 12
    radius: 5
    border.width: 1
    border.color: custompalette.darkInput
}
