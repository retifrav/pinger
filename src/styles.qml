pragma Singleton

import QtQuick
import QtQuick.Window

Item {
    //property real basePixelDensity: 5
    function calculatedFontSize(fontSize, adjustment, whatFor = "unknown")
    {
        //return fontSize * Screen.pixelDensity / basePixelDensity;

        const clc = Math.floor((fontSize - adjustment * fontSizeRatio) * fontSizeRatio);
        //console.debug(`${whatFor} font size: ${clc}`);
        return clc;
    }

    // there might be a more complex condition required here
    property int baseFontSize: fontSizeRatio > 0.7
        ? 28
        : 24;
    property int sectionHeaderFontSize: calculatedFontSize(baseFontSize, 0, "section")
    property int headerFontSize: calculatedFontSize(baseFontSize, 1.0, "header")
    property int normalFontSize: calculatedFontSize(baseFontSize, 3.0, "normal")
    property int dialogFontSize: calculatedFontSize(baseFontSize, 6.0, "dialog")
    property int consoleFontSize: calculatedFontSize(baseFontSize, 8.0, "console")

    property int layoutSpacing: 20

    // colors

    property string mainBackground: "#263238"
    property string regionBackground: "#37474F"

    property string labelsColor: "#607D8B"
    property string labelsColorDarker: "#6A777F"
    property string buttonsTextColor: "#D8D8D8"

    property string colorReceived: "#81C784" // 0
    property string colorLost: "#E57373" // 1
    property string colorError: "#CFE573" // 2

    // dialogs

    property int dialogWindowWidth: 500
    property int dialogWindowHeight: 350

    property int dialogBorderWidth: 1
    property int dialogPaddingLeft: 20
    property int dialogPaddingTop: 20
    property int dialogPaddingRight: 15
    property int dialogPaddingBottom: 15

    property int dialogHeaderBottomMargin: 15
    property int dialogSectionTopMargin: 12
    property int dialogSubHeaderBottomMargin: 5
    property int dialogRowSpacing: 3

    // tooltips

    property int toolTipDelay: 500
    property int toolTipTimeout: 3000

    /*Component.onCompleted: {
        console.debug("Font size ratio:", fontSizeRatio)
        console.debug("Device pixel ratio:", Screen.devicePixelRatio);
        console.debug("Pixel density:", Screen.pixelDensity);
    }*/
}
