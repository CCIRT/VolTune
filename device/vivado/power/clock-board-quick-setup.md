# Clock board quick setup guide

## About this document

This document is written for quick setup of [Skyworks Si5391 clock generators](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-sheets/si5391-datasheet.pdf) and Skyworks ClockBuilder Pro.  
For more information, please refer to [Si5391 Reference Manual](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/reference-manuals/si5391-reference-manual.pdf) or official documents.

## Required environments

- [Skyworks Si5391 clock generators](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-sheets/si5391-datasheet.pdf)
- Skyworks ClockBuilder Pro Version 4.7
  - Installed on Windows 10 PC.

Connect them with USB cable.

## Clock setting

1. Execute "ClockBuilder Pro"
2. Click "Open Default Plan" (Fig.1)
3. Click "Output Clocks" (Fig.2)
4. Set OUT0 and OUT0A Frequency to 125 MHz (Fig.3)  
   While testing 7.5 Gbps design, set to 117.188 MHz.
5. Click "Write to EVB"(Fig.4)

### Fig.1

![](../../../docs/design_docs/img/clock-board-open-default-plan.png)

### Fig.2

![](../../../docs/design_docs/img/clock-board-output-clocks.png)

### Fig.3

![](../../../docs/design_docs/img/clock-board-set-freq.png)

### Fig.4

![](../../../docs/design_docs/img/clock-board-write-to-evb.png)

## Trouble shooting

- "Open Default Plan" does not appear.
  - Please check connection of board and PC.
  - If they are not connected, "Open Default Plan" does not appear.
