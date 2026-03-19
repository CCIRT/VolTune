module axis_pmbus_wrapper#(
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
  parameter DIV200kHz = 499
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


  // ----------- PMBus ------------------
  inout wire SCL,
  inout wire SDA,
  output wire SCLdbg,
  output wire SDAdbg
);

  wire PDATAt;
  wire PDATAi;
  wire PDATAo;
  wire PCLKt;
  wire PCLKi;
  wire PCLKo;

  IOBUF u_sclk (
    .IO(SCL),
    .O(PCLKi),
    .I(PCLKo),
    .T(PCLKt)
  );

  IOBUF u_sda (
    .IO(SDA),
    .O(PDATAi),
    .I(PDATAo),
    .T(PDATAt)
  );

  assign SCLdbg = PCLKi;
  assign SDAdbg = PDATAi;

  axis_pmbus_with_pec#(
    .CLOCK_DIV_WIDTH(CLOCK_DIV_WIDTH),
    .DIV2MHz(DIV2MHz),
    .DIV800kHz(DIV800kHz),
    .DIV200kHz(DIV200kHz)
  ) u_inst (
    .clock(clock),
    .resetn(resetn),
    .i2c_clock_select(i2c_clock_select),
    .s_axis_TDATA(s_axis_TDATA),
    .s_axis_TKEEP(s_axis_TKEEP),
    .s_axis_TVALID(s_axis_TVALID),
    .s_axis_TREADY(s_axis_TREADY),
    .s_axis_TUSER(s_axis_TUSER),
    .m_axis_TDATA(m_axis_TDATA),
    .m_axis_TKEEP(m_axis_TKEEP),
    .m_axis_TVALID(m_axis_TVALID),
    .m_axis_TUSER(m_axis_TUSER),
    .error(error),
    .PDATAt(PDATAt),
    .PDATAi(PDATAi),
    .PDATAo(PDATAo),
    .PCLKt(PCLKt),
    .PCLKi(PCLKi),
    .PCLKo(PCLKo));
endmodule
