import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

Button {
    id: keyButton

    Layout.preferredWidth: 50
    Layout.preferredHeight: 50

    style: BlackButtonStyle {
        bWidth: keyButton.Layout.preferredWidth
        fontFamily: adventPro.name
        fontSize: 16
    }
}
