import QtQuick 2.15
import QtQuick.Controls 2.15

FormText {
    id: root
    property string tooltipText

    HoverHandler {
        id: hvHnd
        enabled: root.visible
    }

    ToolTip {
        delay: Styles.toolTipDelay
        timeout: Styles.toolTipTimeout
        visible: hvHnd.hovered
        font.pointSize: Styles.consoleFontSize
        text: root.tooltipText
    }
}
