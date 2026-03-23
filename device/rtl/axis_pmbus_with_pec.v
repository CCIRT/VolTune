// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

module axis_pmbus_with_pec#(
  parameter CLOCK_DIV_WIDTH = 9,
  // clock division factor to generate a 2 MHz
  //   when clock is   2MHz, DIV2MHz is 0.
  //   when clock is 100MHz, DIV2MHz is 49. (100/2 - 1)
  // * DIV2MHz > 2
  parameter DIV2MHz = 49,
  // clock division factor to generate a 800 kHz
  //   when clock is 800kHz, DIV800kHz is 0.
  //   when clock is 100MHz, DIV800kHz is 124. (100/0.8 - 1)
  // * DIV800KhkHz > 2
  parameter DIV800kHz = 124,
  // clock division factor to generate a 200 kHz
  //   when clock is 200kHz, DIV200kHz is 0.
  //   when clock is 100MHz, DIV200kHz is 499. (100/0.2 - 1)
  // * DIV200KhkHz > 2
  parameter DIV200kHz = 499,
  // Reset value of i2c_clock_select input port.
  parameter CLOCK_INIT_SELECT = 2'd0
) (
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clock CLK" *)
  (* X_INTERFACE_PARAMETER = "ASSOCIATED_BUSIF s_axis:m_axis, ASSOCIATED_RESET resetn, FREQ_HZ 100000000" *)
  input wire clock, // main clock

  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0  resetn RST" *)
  (* X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
  input wire resetn,// reset

  // 0: 100kHz, 1: 400kHz, 2: 1MHz
  input wire [1:0] i2c_clock_select,

  // ------- PMBus command AXI Stream interface ----
  // [7:0]  : Command (TKEEP[0])
  // [15:8] : Data1   (TKEEP[1])
  // [23:16]: Data2   (TKEEP[2])
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis TDATA" *)
  input wire [23:0] s_axis_TDATA,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis TKEEP" *)
  input wire [2:0]  s_axis_TKEEP,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis TVALID" *)
  input wire        s_axis_TVALID,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis TREADY" *)
  output wire       s_axis_TREADY,
  // [6:0]: Slave address
  // [7]: read byte?
  // [8]: read word?
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis TUSER" *)
  input wire [8:0] s_axis_TUSER,

  // [7:0] : Data1 (TKEEP[0])
  // [15:8]: Data2 (TKEEP[1])
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TDATA" *)
  output wire [15:0] m_axis_TDATA,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TKEEP" *)
  output wire [1:0]  m_axis_TKEEP,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TVALID" *)
  output wire        m_axis_TVALID,
  // same error
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TUSER" *)
  output wire [1:0]  m_axis_TUSER,

  // error. this signal will be cleared when the next pmbus command sequense start.
  //       0 : No error
  //       1 : address error
  //       2 : write ack error
  //       3 : PEC error
  output wire [1:0]  error,


  // ----------- debug port ------------------
  `ifdef DEBUG_PORT
  output reg ACKS,
  output reg ACKH,
  output reg OUT,
  output reg IN,
  output reg STOP,
  output reg START,
  `endif

  // ----------- PMBus ------------------
  output wire PDATAt,// PMBus DATA (tristate buffer control signal:  io = PDATAt? 1'bz : PDATAo)
  input  wire PDATAi,// PMBus DATA (input)
  output wire PDATAo,// PMBus DATA (output)
  // PMBus clock
  output wire PCLKt, // io = PCLKt? 1'bz : PCLKo
  input wire  PCLKi,
  output wire PCLKo
);
  (* ASYNC_REG = "TRUE" *) reg pclkibuf0, pclkibuf;
  (* ASYNC_REG = "TRUE" *) reg pdataibuf0, pdataibuf;
  always @(posedge clock)
    {pclkibuf0, pclkibuf, pdataibuf0, pdataibuf} <= {PCLKi, pclkibuf0, PDATAi, pdataibuf0};

  // CRC-8
  wire [7:0] crc_current;
  wire [7:0] crc_input;
  wire [7:0] crc_output;
  wire [7:0] crc_index = crc_current ^ crc_input;


  // -----------------------------------------------
  // I2C basic clock
  // -----------------------------------------------

  // ------ clock input signals ---------
  wire clkActive; // run clock counter
  reg  clkHigh;   // output clock is high when this is HIGH.
  // wire clkLow;    // output clock is low when this is HIGH.
  reg recount; // restart count
  // ------------------------------------


  reg i2cClkOut;
  // clock divide counter
  reg  [CLOCK_DIV_WIDTH - 1:0] divcnt;
  wire [CLOCK_DIV_WIDTH - 1:0] init_divcnt, clkbsy_check, next_divcnt, half_divcnt;
  wire                         divcnt_wrap = divcnt == 'd0;

  reg [1:0] i2c_clk_select;

  always @(posedge clock)
    if (!resetn) i2c_clk_select <= CLOCK_INIT_SELECT;
    else if (!clkActive) i2c_clk_select <= i2c_clock_select;

  assign init_divcnt = i2c_clk_select == 2'd1 ? DIV800kHz:
                       i2c_clk_select == 2'd2 ? DIV2MHz:
                                                DIV200kHz;
  localparam CHECK_DELAY = 4;
  assign clkbsy_check = i2c_clk_select == 2'd1 ? DIV800kHz - CHECK_DELAY:
                        i2c_clk_select == 2'd2 ? DIV2MHz - CHECK_DELAY:
                                                 DIV200kHz - CHECK_DELAY;

  assign half_divcnt = i2c_clk_select == 2'd1 ? DIV800kHz / 2:
                       i2c_clk_select == 2'd2 ? DIV2MHz / 2:
                                                DIV200kHz / 2;

  wire clkbsy = i2cClkOut && !pclkibuf && divcnt < clkbsy_check; // secondary device set CLK to LOW
  assign next_divcnt = (!clkActive || divcnt_wrap || clkbsy || recount) ? init_divcnt : divcnt - 1'b1;

  always @(posedge clock)
    if (!resetn) divcnt <= DIV2MHz;
    else divcnt <= next_divcnt;


  // output clock
  localparam CLK_STOP = 1'b1;
  reg  i2cClkBase;
  wire i2cClkNext = !clkActive ? CLK_STOP: (divcnt_wrap? ~i2cClkBase : i2cClkBase);
  wire i2cClkOutNext = clkHigh? 1'b1: i2cClkNext;
  always @(posedge clock)
    if (!resetn) begin
      i2cClkBase <= CLK_STOP;
      i2cClkOut <= CLK_STOP;
    end else begin
      i2cClkBase <= i2cClkNext;
      i2cClkOut <= i2cClkOutNext;
    end

  // detect clock edge
  reg negedgePCLKEn, posedgePCLKEn;
  wire negedgePCLK = ~pclkibuf && negedgePCLKEn;
  // *note* posedge detection is delayed to avoid chattering.
  wire posedgePCLK = i2cClkOut && divcnt == half_divcnt && posedgePCLKEn;

  always @(posedge clock)
    if (!resetn)                          negedgePCLKEn <= 1'b0;
    else if (negedgePCLK)                 negedgePCLKEn <= 1'b0;
    else if (i2cClkOut && !i2cClkOutNext) negedgePCLKEn <= 1'b1;

  always @(posedge clock)
    if (!resetn)                          posedgePCLKEn <= 1'b0;
    else if (posedgePCLK)                 posedgePCLKEn <= 1'b0;
    else if (!i2cClkOut && i2cClkOutNext) posedgePCLKEn <= 1'b1;
  // ---------- Port --------------
  assign PCLKo = i2cClkOut;
  assign PCLKt = i2cClkOut;
  // ------------------------------

  // -----------------------------------------------
  // I2C
  //   * M : main
  //   * S : secondary
  // -----------------------------------------------

  // ---- i2c input signal ------------------------
  reg        i2cStartReq;
  wire       i2cRecv; //0: M -> S, 1: S -> M
  wire [6:0] i2cAddress; // S address
  wire [7:0] i2cNextWriteData; // output data
  wire       i2cHasNextData;
  wire       i2cRepeatStartReq;
  // ----------------------------------------------
  // ---- i2c output signal -----------------------
  reg       i2cMarkWriteDataInvalid; // i2cNextWriteData must be update.
  reg       i2cRecvDataValid;
  reg       i2cRepeatStart;
  reg       i2cAddressNotFound;
  reg       i2cWriteError;
  wire [7:0] i2cRecvData;
  // ----------------------------------------------


  // State machine
  localparam STATE_I2C_IDLE = 4'd0;
  localparam STATE_I2C_START = 4'd1;
  localparam STATE_I2C_ADDR_RW = 4'd2;
  localparam STATE_I2C_ADDR_RW_ACK = 4'd3;
  localparam STATE_I2C_DATA = 4'd4;
  localparam STATE_I2C_W_ACK = 4'd5;
  localparam STATE_I2C_R_ACK = 4'd6;
  localparam STATE_I2C_STOP0 = 4'd7;
  localparam STATE_I2C_STOP1 = 4'd8;
  localparam STATE_I2C_STOP2 = 4'd9;
  localparam STATE_I2C_RESTART0 = 4'd10;// set DATA -> 1
  localparam STATE_I2C_RESTART1 = 4'd11;// clk -> 1
  localparam STATE_I2C_RESTART2 = 4'd12; // DATA -> 0 (clk = 1)

  reg [3:0] i2cState, i2cStateNext;

  wire i2cStateR = i2cState == STATE_I2C_DATA && i2cRecv;
  wire i2cStateW = i2cState == STATE_I2C_DATA && !i2cRecv;
  wire i2cStateRNext = i2cStateNext == STATE_I2C_DATA && i2cRecv;
  wire i2cStateWNext = i2cStateNext == STATE_I2C_DATA && !i2cRecv;

  wire i2cDataOutState = i2cStateW || i2cState == STATE_I2C_ADDR_RW;
  wire i2cDataOutStateNext = i2cStateWNext || i2cStateNext == STATE_I2C_ADDR_RW;
  wire i2cDataInState = i2cState == STATE_I2C_ADDR_RW_ACK ||
                        i2cState == STATE_I2C_W_ACK ||
                        i2cStateR;
  wire i2cDataInStateNext = i2cStateNext == STATE_I2C_ADDR_RW_ACK ||
                            i2cStateNext == STATE_I2C_W_ACK ||
                            i2cStateRNext;

  always @(posedge clock)
    if (!resetn) begin
      i2cState <= STATE_I2C_IDLE;
    end else begin
      i2cState <= i2cStateNext;
    end

  // basic clock input -----
  reg    clkHighNext;
  assign clkActive = i2cState != STATE_I2C_IDLE;
  always @(posedge clock)
    if (!resetn) clkHigh <= 1'b1;
    else         clkHigh <= clkHighNext;
  // -----------------------

  function [7:0] reverseBit(input [7:0] d);
    reverseBit = {d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]};
  endfunction

  function [0:0] ack(input d);
    ack = !d;
  endfunction

  // output/input data
  reg [7:0] i2cData, i2cDataNext, i2cWriteData;
  wire      i2cSlvAck = ack(i2cData[7]);
  reg       i2cDataSet;
  wire      i2cDataUpdateR = i2cDataInState  && posedgePCLK;
  wire      i2cDataUpdateW = i2cDataOutState && negedgePCLK;
  reg       i2cDataT;

  // output/input counter
  reg [2:0] i2cDataCounter;
  reg       i2cDataCounterReset;

  always @(posedge clock)
    if (!resetn)                  i2cDataCounter <= 3'd7;
    else if (i2cDataCounterReset) i2cDataCounter <= 3'd7;
    else if (negedgePCLK)         i2cDataCounter <= i2cDataCounter - 3'd1;

  always @(posedge clock)
    if (!resetn) i2cData <= 8'd0;
    else         i2cData <= i2cDataNext;

  always @(posedge clock)
    if (!resetn) i2cDataT <= 1'b1;
    else if (i2cStateNext == STATE_I2C_ADDR_RW ||
             (i2cStateNext == STATE_I2C_DATA && !i2cRecv)||
             i2cStateNext == STATE_I2C_START ||
             i2cStateNext == STATE_I2C_R_ACK ||
             i2cStateNext == STATE_I2C_STOP0 ||
             i2cStateNext == STATE_I2C_RESTART0 ||
             i2cStateNext == STATE_I2C_RESTART1 ||
             i2cStateNext == STATE_I2C_RESTART2 ) i2cDataT <= i2cDataNext[0];
    else i2cDataT <= 1'b1;

  always @(*) begin
    i2cDataNext = i2cData;
    if (i2cStateNext == STATE_I2C_START) i2cDataNext = 8'd0;
    else if (i2cStateNext == STATE_I2C_STOP0) i2cDataNext = 8'd0;
    else if (i2cStateNext == STATE_I2C_STOP1) i2cDataNext = 8'd1;
    else if (i2cStateNext == STATE_I2C_STOP2) i2cDataNext = 8'd1;
    else if (i2cStateNext == STATE_I2C_RESTART0)i2cDataNext = 8'd1;
    else if (i2cStateNext == STATE_I2C_RESTART1)i2cDataNext = 8'd1;
    else if (i2cStateNext == STATE_I2C_RESTART2)i2cDataNext = 8'd0;
    else if (i2cStateNext == STATE_I2C_IDLE) i2cDataNext = {7'd0, 1'b1};
    else if (i2cStateNext == STATE_I2C_R_ACK)    i2cDataNext = {7'd0, ack(i2cHasNextData)};
    else if (i2cDataSet)                         i2cDataNext = reverseBit(i2cWriteData);
    else if (i2cDataUpdateR)                     i2cDataNext = {pdataibuf, i2cData[7:1]};
    else if (i2cDataUpdateW)                     i2cDataNext = {1'b0, i2cData[7:1]};
  end
  assign i2cRecvData = reverseBit(i2cData);


  always @(*) begin
    i2cStateNext = i2cState;
    clkHighNext = clkHigh;
    i2cMarkWriteDataInvalid = 1'b0;
    i2cRecvDataValid = 1'b0;
    i2cRepeatStart = 1'b0;
    i2cDataCounterReset = 1'b0;
    i2cDataSet = 1'b0;
    i2cAddressNotFound = 1'b0;
    i2cWriteError = 1'b0;
    i2cWriteData = 'd0;
    recount = 1'b0;

    case(i2cState)
    STATE_I2C_IDLE: begin
      clkHighNext = 1'b1;
      if (i2cStartReq && pclkibuf) begin
        i2cStateNext = STATE_I2C_START;
        clkHighNext = 1'b0;
      end
    end

    STATE_I2C_START: begin
      // goto next state after detect PCLKi is LOW
      if (!pclkibuf) begin
        i2cStateNext = STATE_I2C_ADDR_RW;
        i2cDataCounterReset = 1'b1;
        i2cDataSet = 1'b1;
        i2cWriteData = {i2cAddress, i2cRecv};
      end
    end


    STATE_I2C_ADDR_RW: begin
      if (negedgePCLK && i2cDataCounter == 3'd0)
        i2cStateNext = STATE_I2C_ADDR_RW_ACK;
    end

    STATE_I2C_ADDR_RW_ACK: begin
      if (negedgePCLK) begin
        if (i2cSlvAck) begin
          i2cStateNext = STATE_I2C_DATA;
          i2cDataCounterReset = 1'b1;
          if (!i2cRecv) begin
            i2cDataSet = 1'b1;
            i2cWriteData = i2cNextWriteData;
            i2cMarkWriteDataInvalid = 1'b1;
          end
        end else begin
          // address not found.
          i2cStateNext = STATE_I2C_STOP0;
          i2cAddressNotFound = 1'b1;
        end
      end
    end

    STATE_I2C_DATA: begin
      if (negedgePCLK && i2cDataCounter == 'd0) begin
        if (i2cRecv) begin
          i2cStateNext = STATE_I2C_R_ACK;
          i2cRecvDataValid = 1'b1; // notify
        end else begin
          i2cStateNext = STATE_I2C_W_ACK;
        end
      end
    end

    STATE_I2C_W_ACK: begin
      if (negedgePCLK) begin
        if (i2cSlvAck) begin
          if (i2cHasNextData) begin
            i2cDataCounterReset = 1'b1;
            i2cStateNext = STATE_I2C_DATA;
            i2cDataSet = 1'b1;
            i2cWriteData = i2cNextWriteData;
            i2cMarkWriteDataInvalid = 1'b1;
          end else if (i2cRepeatStartReq) begin
            i2cStateNext = STATE_I2C_RESTART0;
            i2cRepeatStart = 1'b1;
          end else begin
            i2cStateNext = STATE_I2C_STOP0;
          end
        end else begin
          // NAK recieve error
          i2cStateNext = STATE_I2C_STOP0;
          i2cWriteError = 1'b1;
        end
      end
    end

    STATE_I2C_R_ACK: begin
      if (negedgePCLK) begin
        if (i2cHasNextData) begin
          i2cDataCounterReset = 1'b1;
          i2cStateNext = STATE_I2C_DATA;
        end else if (i2cRepeatStartReq) begin
          i2cStateNext = STATE_I2C_RESTART0;
          i2cRepeatStart = 1'b1;
        end else begin
          i2cStateNext = STATE_I2C_STOP0;
        end
      end
    end

    STATE_I2C_STOP0: begin
      if (i2cClkBase)
        clkHighNext = 1'b1;
      if (~i2cClkBase && clkHigh)
        i2cStateNext = STATE_I2C_STOP1;
    end

    STATE_I2C_STOP1: begin
      if (i2cClkBase)
        i2cStateNext = STATE_I2C_STOP2;
    end

    STATE_I2C_STOP2: begin
      if (~i2cClkBase)
        i2cStateNext = STATE_I2C_IDLE;
    end

    STATE_I2C_RESTART0: begin
      if (posedgePCLK) begin
        i2cStateNext = STATE_I2C_RESTART1;
      end
    end

    STATE_I2C_RESTART1: begin
      if (divcnt == 'd1) begin
        recount = 1'b1;
        i2cStateNext = STATE_I2C_RESTART2;
      end
    end

    STATE_I2C_RESTART2: begin
      if (!pclkibuf) begin
        i2cStateNext = STATE_I2C_ADDR_RW;
        i2cDataCounterReset = 1'b1;
        i2cDataSet = 1'b1;
        i2cWriteData = {i2cAddress, i2cRecv};
      end
    end

    default: begin
      i2cStateNext = STATE_I2C_IDLE;
      clkHighNext = 1'b1;
    end

    endcase
  end

  // ---------- Port -----------------
  assign PDATAo = i2cData[0];
  assign PDATAt = i2cDataT;


  // ----------- debug port ------------------
  `ifdef DEBUG_PORT
  always @(posedge clock) begin
    if (!resetn)
      {ACKS, ACKH, OUT, IN, STOP, START} <= 'd0;
    else
      {ACKS, ACKH, OUT, IN, STOP, START} <= {
        i2cStateNext == STATE_I2C_W_ACK || i2cStateNext == STATE_I2C_ADDR_RW_ACK,
        i2cStateNext == STATE_I2C_R_ACK,
        (i2cStateNext == STATE_I2C_DATA && !i2cRecv) || i2cStateNext == STATE_I2C_ADDR_RW,
        i2cStateNext == STATE_I2C_DATA && i2cRecv,
        i2cStateNext == STATE_I2C_STOP2,
        i2cStateNext == STATE_I2C_START};
  end
  `endif


  // ------------- PMBus --------------------
  localparam STATE_PMBUS_IDLE = 2'd0;
  localparam STATE_PMBUS_CMD = 2'd1;
  localparam STATE_PMBUS_READ = 2'd2;

  reg [1:0] pmbusState, pmbusStateNext;
  always @(posedge clock)
    if (!resetn) pmbusState <= STATE_PMBUS_IDLE;
    else         pmbusState <= pmbusStateNext;
  // crc 8
  reg [7:0] crc;
  wire      update_crc = i2cDataSet || i2cRecvDataValid;
  reg       clear_crc;

  assign    crc_current = crc;
  assign    crc_input = i2cDataSet? i2cWriteData : i2cRecvData;

  always @(posedge clock)
    if (!resetn) crc <= 8'd0;
    else if (clear_crc) crc <= 8'd0;
    else if (update_crc) crc <= crc_output;


  // error
  reg [1:0]  err;
  wire       clearErr;
  reg        pecError;

  always @(posedge clock)
    if (!resetn) err <= 2'b00;
    else if (clearErr) err <= 2'b00;
    else if (i2cAddressNotFound) err <= 2'b01;
    else if (i2cWriteError)      err <= 2'b10;
    else if (pecError)           err <= 2'b11;

  // s_axis
  reg [23:0] stdata;
  reg [2:0]  stkeep;
  reg [6:0]  staddr;
  reg [1:0]  stread;// [0]: read next cycle, [1]: read 2nd cycle
  reg        stready;
  wire       stshift;
  reg        hasWriteTask;
  wire s_axis_accept = s_axis_TVALID && stready;
  wire hasReadTask = |stread;
  assign clearErr = s_axis_accept;
  assign stshift = i2cMarkWriteDataInvalid && pmbusState == STATE_PMBUS_CMD;

  always @(posedge clock)
    if(!resetn) stready <= 1'b0;
    else stready <= pmbusStateNext == STATE_PMBUS_IDLE;

  always @(posedge clock)
    if(!resetn) {stdata, stkeep, hasWriteTask} <= 'd0;
    else if (s_axis_accept) {stdata, stkeep, hasWriteTask} <= {s_axis_TDATA, s_axis_TKEEP, 1'b1};
    else if (stshift) begin
      stdata <= {8'd0, stdata[23:8]};
      stkeep <= {1'b0, stkeep[2:1]};
      hasWriteTask <= hasReadTask ? stkeep[1] : stkeep[0];
    end

  always @(posedge clock)
    if(!resetn) {stread, staddr} <= 'd0;
    else if (s_axis_accept) begin
      staddr <= s_axis_TUSER[6:0];
      stread[0] <= s_axis_TUSER[7] || s_axis_TUSER[8];
      stread[1] <= s_axis_TUSER[8];
    end

  // m_axis
  reg [15:0] mtdata, mtdataNext;
  reg [1:0]  mtkeep, mtkeepNext;
  reg        readPEC, readPECNext;
  wire [1:0] mtuser = err;
  reg        mtvalid;
  wire       read2nd = stread[1];

  always @(posedge clock)
    if(!resetn) {mtdata, mtkeep, readPEC} <= 'd0;
    else {mtdata, mtkeep, readPEC} <= {mtdataNext, mtkeepNext, readPECNext};

  always @(*) begin
    {mtdataNext, mtkeepNext, readPECNext} = {mtdata, mtkeep, readPEC};
    pecError = 1'b0;
    if (s_axis_accept) begin
      {mtdataNext, mtkeepNext} = 'd0;
      readPECNext = 1'b1;
    end else if (i2cRecvDataValid) begin
      if (!mtkeep[0]) begin
        mtdataNext = {8'd0, i2cRecvData};
        mtkeepNext = 2'b01;
      end else if (read2nd && !mtkeep[1])begin
        mtdataNext = {i2cRecvData, mtdata[7:0]};
        mtkeepNext = 2'b11;
      end else begin
        readPECNext = 1'b0;
        pecError = i2cRecvData != crc;
      end
    end else if (i2cAddressNotFound || i2cWriteError) begin
      mtdataNext = 'd0;
      mtkeepNext = 2'd00;
    end
  end

  always @(posedge clock)
    if(!resetn) mtvalid <= 'd0;
    else if (s_axis_accept || mtvalid) mtvalid <= 'd0;
    else if (i2cRecvDataValid) begin
      if (!readPECNext) begin
        mtvalid <= 1'b1;
      end
    end else if (i2cAddressNotFound || i2cWriteError) begin
      mtvalid <= 1'b1;
    end else if (!(|err) && pmbusState == STATE_PMBUS_CMD && pmbusStateNext == STATE_PMBUS_IDLE) begin
      mtvalid <= 1'b1;
    end


  // i2c input
  assign i2cRecv = pmbusState == STATE_PMBUS_READ;
  assign i2cAddress = staddr;
  assign i2cNextWriteData = stkeep[0] ? stdata[7:0] : crc;
  assign i2cHasNextData = pmbusState == STATE_PMBUS_CMD ? hasWriteTask:
                          pmbusState == STATE_PMBUS_READ? readPECNext: 1'b0;
  assign i2cRepeatStartReq = hasReadTask && pmbusState == STATE_PMBUS_CMD;

  reg i2cStartReqSet;
  always @(posedge clock)
    if(!resetn) i2cStartReq <= 1'b0;
    else if (i2cState != STATE_I2C_IDLE) i2cStartReq <= 1'b0;
    else if (i2cStartReqSet) i2cStartReq <= 1'b1;

  // state
  always @(*) begin
    pmbusStateNext = pmbusState;
    i2cStartReqSet = 1'b0;
    clear_crc = 1'b0;

    case(pmbusState)
    STATE_PMBUS_IDLE:
      if (s_axis_accept) begin
        i2cStartReqSet = 1'b1;
        pmbusStateNext = STATE_PMBUS_CMD;
      end else begin
        clear_crc = 1'b0;
      end

    STATE_PMBUS_CMD:
      if (i2cRepeatStart)
        pmbusStateNext = STATE_PMBUS_READ;
      else if (i2cState == STATE_I2C_IDLE && !i2cStartReq) begin
        pmbusStateNext = STATE_PMBUS_IDLE;
        clear_crc = 1'b1;
      end

    STATE_PMBUS_READ:
      if (i2cState == STATE_I2C_IDLE) begin
        pmbusStateNext = STATE_PMBUS_IDLE;
        clear_crc = 1'b1;
      end

    default:  pmbusStateNext = STATE_PMBUS_IDLE;
    endcase
  end

  // ------------- PORT ---------------------
  assign s_axis_TREADY = stready;
  assign m_axis_TDATA = mtdata;
  assign m_axis_TKEEP = mtkeep;
  assign m_axis_TUSER = mtuser;
  assign m_axis_TVALID = mtvalid;
  assign error = err;


  // -------------- CRC Table ------------------
  assign crc_output =
      (crc_index == 8'h00)? 8'h00 :
      (crc_index == 8'h01)? 8'h07 :
      (crc_index == 8'h02)? 8'h0E :
      (crc_index == 8'h03)? 8'h09 :
      (crc_index == 8'h04)? 8'h1C :
      (crc_index == 8'h05)? 8'h1B :
      (crc_index == 8'h06)? 8'h12 :
      (crc_index == 8'h07)? 8'h15 :
      (crc_index == 8'h08)? 8'h38 :
      (crc_index == 8'h09)? 8'h3F :
      (crc_index == 8'h0A)? 8'h36 :
      (crc_index == 8'h0B)? 8'h31 :
      (crc_index == 8'h0C)? 8'h24 :
      (crc_index == 8'h0D)? 8'h23 :
      (crc_index == 8'h0E)? 8'h2A :
      (crc_index == 8'h0F)? 8'h2D :
      (crc_index == 8'h10)? 8'h70 :
      (crc_index == 8'h11)? 8'h77 :
      (crc_index == 8'h12)? 8'h7E :
      (crc_index == 8'h13)? 8'h79 :
      (crc_index == 8'h14)? 8'h6C :
      (crc_index == 8'h15)? 8'h6B :
      (crc_index == 8'h16)? 8'h62 :
      (crc_index == 8'h17)? 8'h65 :
      (crc_index == 8'h18)? 8'h48 :
      (crc_index == 8'h19)? 8'h4F :
      (crc_index == 8'h1A)? 8'h46 :
      (crc_index == 8'h1B)? 8'h41 :
      (crc_index == 8'h1C)? 8'h54 :
      (crc_index == 8'h1D)? 8'h53 :
      (crc_index == 8'h1E)? 8'h5A :
      (crc_index == 8'h1F)? 8'h5D :
      (crc_index == 8'h20)? 8'hE0 :
      (crc_index == 8'h21)? 8'hE7 :
      (crc_index == 8'h22)? 8'hEE :
      (crc_index == 8'h23)? 8'hE9 :
      (crc_index == 8'h24)? 8'hFC :
      (crc_index == 8'h25)? 8'hFB :
      (crc_index == 8'h26)? 8'hF2 :
      (crc_index == 8'h27)? 8'hF5 :
      (crc_index == 8'h28)? 8'hD8 :
      (crc_index == 8'h29)? 8'hDF :
      (crc_index == 8'h2A)? 8'hD6 :
      (crc_index == 8'h2B)? 8'hD1 :
      (crc_index == 8'h2C)? 8'hC4 :
      (crc_index == 8'h2D)? 8'hC3 :
      (crc_index == 8'h2E)? 8'hCA :
      (crc_index == 8'h2F)? 8'hCD :
      (crc_index == 8'h30)? 8'h90 :
      (crc_index == 8'h31)? 8'h97 :
      (crc_index == 8'h32)? 8'h9E :
      (crc_index == 8'h33)? 8'h99 :
      (crc_index == 8'h34)? 8'h8C :
      (crc_index == 8'h35)? 8'h8B :
      (crc_index == 8'h36)? 8'h82 :
      (crc_index == 8'h37)? 8'h85 :
      (crc_index == 8'h38)? 8'hA8 :
      (crc_index == 8'h39)? 8'hAF :
      (crc_index == 8'h3A)? 8'hA6 :
      (crc_index == 8'h3B)? 8'hA1 :
      (crc_index == 8'h3C)? 8'hB4 :
      (crc_index == 8'h3D)? 8'hB3 :
      (crc_index == 8'h3E)? 8'hBA :
      (crc_index == 8'h3F)? 8'hBD :
      (crc_index == 8'h40)? 8'hC7 :
      (crc_index == 8'h41)? 8'hC0 :
      (crc_index == 8'h42)? 8'hC9 :
      (crc_index == 8'h43)? 8'hCE :
      (crc_index == 8'h44)? 8'hDB :
      (crc_index == 8'h45)? 8'hDC :
      (crc_index == 8'h46)? 8'hD5 :
      (crc_index == 8'h47)? 8'hD2 :
      (crc_index == 8'h48)? 8'hFF :
      (crc_index == 8'h49)? 8'hF8 :
      (crc_index == 8'h4A)? 8'hF1 :
      (crc_index == 8'h4B)? 8'hF6 :
      (crc_index == 8'h4C)? 8'hE3 :
      (crc_index == 8'h4D)? 8'hE4 :
      (crc_index == 8'h4E)? 8'hED :
      (crc_index == 8'h4F)? 8'hEA :
      (crc_index == 8'h50)? 8'hB7 :
      (crc_index == 8'h51)? 8'hB0 :
      (crc_index == 8'h52)? 8'hB9 :
      (crc_index == 8'h53)? 8'hBE :
      (crc_index == 8'h54)? 8'hAB :
      (crc_index == 8'h55)? 8'hAC :
      (crc_index == 8'h56)? 8'hA5 :
      (crc_index == 8'h57)? 8'hA2 :
      (crc_index == 8'h58)? 8'h8F :
      (crc_index == 8'h59)? 8'h88 :
      (crc_index == 8'h5A)? 8'h81 :
      (crc_index == 8'h5B)? 8'h86 :
      (crc_index == 8'h5C)? 8'h93 :
      (crc_index == 8'h5D)? 8'h94 :
      (crc_index == 8'h5E)? 8'h9D :
      (crc_index == 8'h5F)? 8'h9A :
      (crc_index == 8'h60)? 8'h27 :
      (crc_index == 8'h61)? 8'h20 :
      (crc_index == 8'h62)? 8'h29 :
      (crc_index == 8'h63)? 8'h2E :
      (crc_index == 8'h64)? 8'h3B :
      (crc_index == 8'h65)? 8'h3C :
      (crc_index == 8'h66)? 8'h35 :
      (crc_index == 8'h67)? 8'h32 :
      (crc_index == 8'h68)? 8'h1F :
      (crc_index == 8'h69)? 8'h18 :
      (crc_index == 8'h6A)? 8'h11 :
      (crc_index == 8'h6B)? 8'h16 :
      (crc_index == 8'h6C)? 8'h03 :
      (crc_index == 8'h6D)? 8'h04 :
      (crc_index == 8'h6E)? 8'h0D :
      (crc_index == 8'h6F)? 8'h0A :
      (crc_index == 8'h70)? 8'h57 :
      (crc_index == 8'h71)? 8'h50 :
      (crc_index == 8'h72)? 8'h59 :
      (crc_index == 8'h73)? 8'h5E :
      (crc_index == 8'h74)? 8'h4B :
      (crc_index == 8'h75)? 8'h4C :
      (crc_index == 8'h76)? 8'h45 :
      (crc_index == 8'h77)? 8'h42 :
      (crc_index == 8'h78)? 8'h6F :
      (crc_index == 8'h79)? 8'h68 :
      (crc_index == 8'h7A)? 8'h61 :
      (crc_index == 8'h7B)? 8'h66 :
      (crc_index == 8'h7C)? 8'h73 :
      (crc_index == 8'h7D)? 8'h74 :
      (crc_index == 8'h7E)? 8'h7D :
      (crc_index == 8'h7F)? 8'h7A :
      (crc_index == 8'h80)? 8'h89 :
      (crc_index == 8'h81)? 8'h8E :
      (crc_index == 8'h82)? 8'h87 :
      (crc_index == 8'h83)? 8'h80 :
      (crc_index == 8'h84)? 8'h95 :
      (crc_index == 8'h85)? 8'h92 :
      (crc_index == 8'h86)? 8'h9B :
      (crc_index == 8'h87)? 8'h9C :
      (crc_index == 8'h88)? 8'hB1 :
      (crc_index == 8'h89)? 8'hB6 :
      (crc_index == 8'h8A)? 8'hBF :
      (crc_index == 8'h8B)? 8'hB8 :
      (crc_index == 8'h8C)? 8'hAD :
      (crc_index == 8'h8D)? 8'hAA :
      (crc_index == 8'h8E)? 8'hA3 :
      (crc_index == 8'h8F)? 8'hA4 :
      (crc_index == 8'h90)? 8'hF9 :
      (crc_index == 8'h91)? 8'hFE :
      (crc_index == 8'h92)? 8'hF7 :
      (crc_index == 8'h93)? 8'hF0 :
      (crc_index == 8'h94)? 8'hE5 :
      (crc_index == 8'h95)? 8'hE2 :
      (crc_index == 8'h96)? 8'hEB :
      (crc_index == 8'h97)? 8'hEC :
      (crc_index == 8'h98)? 8'hC1 :
      (crc_index == 8'h99)? 8'hC6 :
      (crc_index == 8'h9A)? 8'hCF :
      (crc_index == 8'h9B)? 8'hC8 :
      (crc_index == 8'h9C)? 8'hDD :
      (crc_index == 8'h9D)? 8'hDA :
      (crc_index == 8'h9E)? 8'hD3 :
      (crc_index == 8'h9F)? 8'hD4 :
      (crc_index == 8'hA0)? 8'h69 :
      (crc_index == 8'hA1)? 8'h6E :
      (crc_index == 8'hA2)? 8'h67 :
      (crc_index == 8'hA3)? 8'h60 :
      (crc_index == 8'hA4)? 8'h75 :
      (crc_index == 8'hA5)? 8'h72 :
      (crc_index == 8'hA6)? 8'h7B :
      (crc_index == 8'hA7)? 8'h7C :
      (crc_index == 8'hA8)? 8'h51 :
      (crc_index == 8'hA9)? 8'h56 :
      (crc_index == 8'hAA)? 8'h5F :
      (crc_index == 8'hAB)? 8'h58 :
      (crc_index == 8'hAC)? 8'h4D :
      (crc_index == 8'hAD)? 8'h4A :
      (crc_index == 8'hAE)? 8'h43 :
      (crc_index == 8'hAF)? 8'h44 :
      (crc_index == 8'hB0)? 8'h19 :
      (crc_index == 8'hB1)? 8'h1E :
      (crc_index == 8'hB2)? 8'h17 :
      (crc_index == 8'hB3)? 8'h10 :
      (crc_index == 8'hB4)? 8'h05 :
      (crc_index == 8'hB5)? 8'h02 :
      (crc_index == 8'hB6)? 8'h0B :
      (crc_index == 8'hB7)? 8'h0C :
      (crc_index == 8'hB8)? 8'h21 :
      (crc_index == 8'hB9)? 8'h26 :
      (crc_index == 8'hBA)? 8'h2F :
      (crc_index == 8'hBB)? 8'h28 :
      (crc_index == 8'hBC)? 8'h3D :
      (crc_index == 8'hBD)? 8'h3A :
      (crc_index == 8'hBE)? 8'h33 :
      (crc_index == 8'hBF)? 8'h34 :
      (crc_index == 8'hC0)? 8'h4E :
      (crc_index == 8'hC1)? 8'h49 :
      (crc_index == 8'hC2)? 8'h40 :
      (crc_index == 8'hC3)? 8'h47 :
      (crc_index == 8'hC4)? 8'h52 :
      (crc_index == 8'hC5)? 8'h55 :
      (crc_index == 8'hC6)? 8'h5C :
      (crc_index == 8'hC7)? 8'h5B :
      (crc_index == 8'hC8)? 8'h76 :
      (crc_index == 8'hC9)? 8'h71 :
      (crc_index == 8'hCA)? 8'h78 :
      (crc_index == 8'hCB)? 8'h7F :
      (crc_index == 8'hCC)? 8'h6A :
      (crc_index == 8'hCD)? 8'h6D :
      (crc_index == 8'hCE)? 8'h64 :
      (crc_index == 8'hCF)? 8'h63 :
      (crc_index == 8'hD0)? 8'h3E :
      (crc_index == 8'hD1)? 8'h39 :
      (crc_index == 8'hD2)? 8'h30 :
      (crc_index == 8'hD3)? 8'h37 :
      (crc_index == 8'hD4)? 8'h22 :
      (crc_index == 8'hD5)? 8'h25 :
      (crc_index == 8'hD6)? 8'h2C :
      (crc_index == 8'hD7)? 8'h2B :
      (crc_index == 8'hD8)? 8'h06 :
      (crc_index == 8'hD9)? 8'h01 :
      (crc_index == 8'hDA)? 8'h08 :
      (crc_index == 8'hDB)? 8'h0F :
      (crc_index == 8'hDC)? 8'h1A :
      (crc_index == 8'hDD)? 8'h1D :
      (crc_index == 8'hDE)? 8'h14 :
      (crc_index == 8'hDF)? 8'h13 :
      (crc_index == 8'hE0)? 8'hAE :
      (crc_index == 8'hE1)? 8'hA9 :
      (crc_index == 8'hE2)? 8'hA0 :
      (crc_index == 8'hE3)? 8'hA7 :
      (crc_index == 8'hE4)? 8'hB2 :
      (crc_index == 8'hE5)? 8'hB5 :
      (crc_index == 8'hE6)? 8'hBC :
      (crc_index == 8'hE7)? 8'hBB :
      (crc_index == 8'hE8)? 8'h96 :
      (crc_index == 8'hE9)? 8'h91 :
      (crc_index == 8'hEA)? 8'h98 :
      (crc_index == 8'hEB)? 8'h9F :
      (crc_index == 8'hEC)? 8'h8A :
      (crc_index == 8'hED)? 8'h8D :
      (crc_index == 8'hEE)? 8'h84 :
      (crc_index == 8'hEF)? 8'h83 :
      (crc_index == 8'hF0)? 8'hDE :
      (crc_index == 8'hF1)? 8'hD9 :
      (crc_index == 8'hF2)? 8'hD0 :
      (crc_index == 8'hF3)? 8'hD7 :
      (crc_index == 8'hF4)? 8'hC2 :
      (crc_index == 8'hF5)? 8'hC5 :
      (crc_index == 8'hF6)? 8'hCC :
      (crc_index == 8'hF7)? 8'hCB :
      (crc_index == 8'hF8)? 8'hE6 :
      (crc_index == 8'hF9)? 8'hE1 :
      (crc_index == 8'hFA)? 8'hE8 :
      (crc_index == 8'hFB)? 8'hEF :
      (crc_index == 8'hFC)? 8'hFA :
      (crc_index == 8'hFD)? 8'hFD :
      (crc_index == 8'hFE)? 8'hF4 :
                            8'hF3;
endmodule
