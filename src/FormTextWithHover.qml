import QtQuick
import QtQuick.Controls

FormText {
    //id: root

    property alias tooltipText: tltp.text

    HoverHandler {
        id: hvHnd
        cursorShape: Qt.WhatsThisCursor
        //enabled: parent.visible
        /*onHoveredChanged: {
            console.debug(`${parent.text} hover changed to: ${this.hovered}`);
        }*/
    }

    ToolTip {
        id: tltp
        delay: Styles.toolTipDelay
        timeout: Styles.toolTipTimeout
        visible: hvHnd.hovered
        font.pointSize: Styles.consoleFontSize
    }

    /*onVisibleChanged: {
        console.debug(`Visibility of ${this.tooltipText} changed to ${this.visible}`);
    }*/
}
