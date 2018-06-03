pragma Singleton
import QtQuick 2.10
//import QtQuick.Window 2.10

Item {
    property int primaryFontSize: 22 /// Screen.pixelDensity * Screen.devicePixelRatio
    property int secondaryFontSize: 20 /// Screen.pixelDensity * Screen.devicePixelRatio

    property string mainBackground: "#263238"
    property string regionBackground: "#37474F"

    property string labelsColor: "#607D8B"
    property string labelsColorDarker: "#6A777F"
    property string buttonsTextColor: "#D8D8D8"

    property string colorReceived: "#81C784" // 0
    property string colorLost: "#E57373" // 1
    property string colorError: "#CFE573" // 2
}
