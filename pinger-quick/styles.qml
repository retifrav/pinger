pragma Singleton
import QtQuick 2.15
//import QtQuick.Window 2.15

Item {
    // fonts

    property int primaryFontSize: 22 /// Screen.pixelDensity * Screen.devicePixelRatio
    property int secondaryFontSize: 20 /// Screen.pixelDensity * Screen.devicePixelRatio

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

    property int dialogBorderWidth: 1
    property int dialogPaddingLeft: 20
    property int dialogPaddingTop: 15
    property int dialogPaddingRight: 15
    property int dialogPaddingBottom: 15

    property int dialogHeaderFontSize: 25
    //property int dialogTextFontSize: 14

    property int dialogHeaderBottomMargin: 15
    property int dialogSectionTopMargin: 10
    property int dialogSubHeaderBottomMargin: 5
    property int dialogRowSpacing: 3

    // tooltips

    property int toolTipDelay: 750
    property int toolTipTimeout: 3000
}
