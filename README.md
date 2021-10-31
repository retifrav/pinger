# Pinger

Network connection quality analyzer.

![Pinger](/misc/screenshot-main-macos.png "Pinger")

The application sends pinging requests to a selected host and tracks the following statistics:

- amount of requests sent, received and lost;
- requests time/latency/delay.

In the end it also does some analytics on the collected data.

## How it works

There are two modes of pinging the host:

- via system `ping` utility and ICMP requests. Most precise and reliable;
- via HTTP HEAD requests. Not as good, but might be the only option on systems without `ping`.

## Supported platforms

Thanks to Qt, the application builds and runs on:

- Mac OS
- Windows
- GNU/Linux

Might also work (*but not tested*) on iOS, Android and other target platforms supported by Qt.
