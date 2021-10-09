import QtQuick//2.15
import QtQuick.Controls//2.15

FormText {
    //id: root

    property alias tooltipText: tltp.text

    HoverHandler {
        id: hvHnd
        enabled: parent.visible
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
