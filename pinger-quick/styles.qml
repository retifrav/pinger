pragma Singleton
import QtQuick 2.15
import QtQuick.Window 2.15

Item {
    property real basePixelDensity: 5
    function calculatedFontSize(fontSize) { return fontSize * Screen.pixelDensity / basePixelDensity; }

    property int primaryFontSize: calculatedFontSize(24)
    property int secondaryFontSize: calculatedFontSize(20)
    property int normalFontSize: calculatedFontSize(18)
    property int dialogFontSize: calculatedFontSize(16)
    property int consoleFontSize: calculatedFontSize(14)

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
    property int dialogPaddingTop: 15
    property int dialogPaddingRight: 15
    property int dialogPaddingBottom: 15

    property int dialogHeaderBottomMargin: 15
    property int dialogSectionTopMargin: 10
    property int dialogSubHeaderBottomMargin: 5
    property int dialogRowSpacing: 3

    // tooltips

    property int toolTipDelay: 750
    property int toolTipTimeout: 3000
}
