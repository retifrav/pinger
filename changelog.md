# Changelog

<!-- MarkdownTOC -->

- [1.2.3](#123)
- [1.2.2](#122)
- [1.2.1](#121)
- [1.2.0](#120)

<!-- /MarkdownTOC -->

## 1.2.3

Released on `2021-11-22`.

- proper CLI arguments parsing
- fixed ICMP/HTTP label clipping
- better font sizes handling on different screens

## 1.2.2

Released on `2021-11-11`.

- improvements in ICMP/PING label positioning
- debug mode is disabled by default, can be enabled with `--debug`
- host field is highlighted and labeled based on current pinging method
- host label and input field are centered vertically
- menu items for the source code and license URLs
- kill ping process on exit only if using ping utility

## 1.2.1

Released on `2021-10-24`.

- returned back to Qt 5.15.2, because Qt 6.2.0 still isn't ready

## 1.2.0

Released on `2021-10-24`.

- added ping / HTTP HEAD switch to settings
- adjusted analytics for HTTP HEAD option (*3 times more latency-tolerable*)
- skipping first successful pre-flight request from statistics (*it's always the slowest*)
- additional check for validity of HTTP HEAD request elapsed timer
- help tooltips change the cursor/pointer shape
- dialogs can be closed also with Space key
- the STOP button also cancels/deletes pending request/reply
- after STOP button is clicked, focus is moved to report button
- settings switches values are set based on actual settings before showing the dialog
- the report is always shown in debug mode, even if there were less than minimum required number of packets sent
