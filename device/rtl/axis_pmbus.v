module axis_pmbus#(
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
  // error[0]: address error
  // error[1]: write ack error
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

  // error
  reg [1:0]  err;
  wire       clearErr;

  always @(posedge clock)
    if (!resetn) err <= 2'b00;
    else if (clearErr) err <= 2'b00;
    else if (i2cAddressNotFound) err <= 2'b01;
    else if (i2cWriteError) err <= 2'b10;

  // s_axis
  reg [23:0] stdata;
  reg [2:0]  stkeep;
  reg [6:0]  staddr;
  reg [1:0]  stread;// [0]: read next cycle, [1]: read 2nd cycle
  reg        stready;
  wire       stshift;
  wire s_axis_accept = s_axis_TVALID && stready;
  wire hasReadTask = |stread;
  assign clearErr = s_axis_accept;
  assign stshift = i2cMarkWriteDataInvalid && pmbusState == STATE_PMBUS_CMD;

  always @(posedge clock)
    if(!resetn) stready <= 1'b0;
    else stready <= pmbusStateNext == STATE_PMBUS_IDLE;

  always @(posedge clock)
    if(!resetn) {stdata, stkeep} <= 'd0;
    else if (s_axis_accept) {stdata, stkeep} <= {s_axis_TDATA, s_axis_TKEEP};
    else if (stshift) begin
      stdata <= {8'd0, stdata[23:8]};
      stkeep <= {1'b0, stkeep[2:1]};
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
  wire [1:0] mtuser = err;
  reg        mtvalid;

  always @(posedge clock)
    if(!resetn) {mtdata, mtkeep} <= 'd0;
    else {mtdata, mtkeep} <= {mtdataNext, mtkeepNext};

  always @(*) begin
    {mtdataNext, mtkeepNext} = {mtdata, mtkeep};
    if (s_axis_accept) begin
      {mtdataNext, mtkeepNext} = 'd0;
    end else if (i2cRecvDataValid) begin
      if (!mtkeep[0]) begin
        mtdataNext = {8'd0, i2cRecvData};
        mtkeepNext = 2'b01;
      end else begin
        mtdataNext = {i2cRecvData, mtdata[7:0]};
        mtkeepNext = 2'b11;
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
      if (!mtkeep[0]) begin
        if (!stread[1]) mtvalid <= 1'b1;
      end else begin
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
  assign i2cNextWriteData = stdata[7:0];
  assign i2cHasNextData = pmbusState == STATE_PMBUS_CMD ? stkeep[0]:
                          pmbusState == STATE_PMBUS_READ? (stread[1] ^ mtkeepNext[1]): 1'b0;
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

    case(pmbusState)
    STATE_PMBUS_IDLE:
      if (s_axis_accept) begin
        i2cStartReqSet = 1'b1;
        pmbusStateNext = STATE_PMBUS_CMD;
      end

    STATE_PMBUS_CMD:
      if (i2cRepeatStart)
        pmbusStateNext = STATE_PMBUS_READ;
      else if (i2cState == STATE_I2C_IDLE && !i2cStartReq)
        pmbusStateNext = STATE_PMBUS_IDLE;

    STATE_PMBUS_READ:
      if (i2cState == STATE_I2C_IDLE)
        pmbusStateNext = STATE_PMBUS_IDLE;

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


endmodule
