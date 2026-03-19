`timescale 1ns / 1ps
`define DLY #1
`define BER 64'h0000_0003_A352_9440       // E12 64'h0x0003_A352_9440, E15 64'h0x0E35_FA93_1A00, 
//`define BER 64'd1000000000       // E12 64'h0x0003_A352_9440, E15 64'h0x0E35_FA93_1A00, 

//***********************************Entity Declaration************************
(* DowngradeIPIdentifiedWarnings="yes" *)
module TxRx5g_GT_FRAME_CHECK #
(
    // parameter to set the number of words in the BRAM
    parameter   RX_DATA_WIDTH            =  64,
    parameter   RXCTRL_WIDTH             =  2,
    parameter   START_OF_PACKET_CHAR     =  64'h00000000000000fb
)                            
(
    // User Interface
    input  wire [(RX_DATA_WIDTH-1):0] RX_DATA_IN,
    input  wire [(RXCTRL_WIDTH-1):0] RXCTRL_IN,

    output reg          RXENPCOMMADET_OUT,
    output reg          RXENMCOMMADET_OUT,
    
    // Track Data
    output wire         TRACK_DATA_OUT,

    // System Interface
    input  wire         USER_CLK,
    input  wire         SYSTEM_RESET 
);


//***************************Internal Register Declarations******************** 

(* ASYNC_REG = "TRUE" *) (* keep = "true" *)reg system_reset_r;
(* ASYNC_REG = "TRUE" *) (* keep = "true" *)reg system_reset_r2;

(* keep = "true" *)reg                          begin_r;
(* keep = "true" *)reg                          data_error_detected_r;
(* keep = "true" *)reg     [63:0]               error_count_r;
(* keep = "true" *)reg                          error_detected_r;
(* keep = "true" *)reg     [3:0]                read_counter_i;    

(* keep = "true" *)reg     [(RX_DATA_WIDTH-1):0]rx_data_r;
(* keep = "true" *)reg     [(RX_DATA_WIDTH-1):0]rx_data_r_track;

(* keep = "true" *)reg                          start_of_packet_detected_r;    
(* keep = "true" *)reg                          track_data_r;
(* keep = "true" *)reg                          track_data_r2;
(* keep = "true" *)reg                          track_data_r3;
(* keep = "true" *)reg     [79:0]               rx_data_ram_r;
(* keep = "true" *)reg     [(RX_DATA_WIDTH-1):0]rx_data_r2;
(* keep = "true" *)reg     [(RX_DATA_WIDTH-1):0]rx_data_r3;
(* keep = "true" *)reg     [(RX_DATA_WIDTH-1):0]rx_data_r4;
(* keep = "true" *)reg     [(RX_DATA_WIDTH-1):0]rx_data_r5;
(* keep = "true" *)reg     [(RX_DATA_WIDTH-1):0]rx_data_r6;
(* keep = "true" *)reg     [(RXCTRL_WIDTH-1):0] rxctrl_r;
(* keep = "true" *)reg     [(RXCTRL_WIDTH-1):0] rxctrl_r2;
(* keep = "true" *)reg     [(RXCTRL_WIDTH-1):0] rxctrl_r3;
(* keep = "true" *)reg     [2:0]                sel;

//*********************************Wire Declarations***************************
(* keep = "true" *)wire    [(RX_DATA_WIDTH-1):0]bram_data_r;
(* keep = "true" *)wire                         error_detected_c;
(* keep = "true" *)wire                         next_begin_c;
(* keep = "true" *)wire                         next_data_error_detected_c;
(* keep = "true" *)wire                         next_track_data_c;
(* keep = "true" *)wire                         start_of_packet_detected_c;

(* keep = "true" *)wire    [(RX_DATA_WIDTH-1):0]rx_data_aligned;
(* keep = "true" *)wire                         rx_data_has_start_char_c;

//*********************************Main Body of Code***************************

    //___________ synchronizing the async reset for ease of timing simulation ________
    always@(posedge USER_CLK)
      begin
       system_reset_r <= `DLY SYSTEM_RESET;    
       system_reset_r2 <= `DLY system_reset_r; 
     end   

    //______________________ Register RXDATA once to ease timing ______________   

    always @(posedge USER_CLK)
    begin
        rx_data_r  <= `DLY    RX_DATA_IN;
        rx_data_r2 <= `DLY    rx_data_r;
    end 

    always @(posedge USER_CLK)
    begin
        rxctrl_r  <= `DLY    RXCTRL_IN;
    end

    //________________________________ State machine __________________________    
    
    // State registers
    always @(posedge USER_CLK)
        if(system_reset_r2)
            {begin_r,track_data_r,data_error_detected_r}  <=  `DLY    3'b100;
        else
        begin
            begin_r                <=  `DLY    next_begin_c;
            track_data_r           <=  `DLY    next_track_data_c;
            data_error_detected_r  <=  `DLY    next_data_error_detected_c;
        end

    // Next state logic
//    assign  next_begin_c                =   (begin_r && !start_of_packet_detected_r)|| data_error_detected_r ;
    assign  next_begin_c                =   (begin_r && !start_of_packet_detected_r);

//    assign  next_track_data_c           =   (begin_r && start_of_packet_detected_r) || (track_data_r && !error_detected_r);
    assign  next_track_data_c           =   (begin_r && start_of_packet_detected_r) || (track_data_r);
                                      
    assign  next_data_error_detected_c  =   (track_data_r && error_detected_r);                               
        
    assign  start_of_packet_detected_c  =   rx_data_has_start_char_c;

    always @(posedge USER_CLK) 
        start_of_packet_detected_r    <=   `DLY  start_of_packet_detected_c;  

    // Registering for timing
    always @(posedge USER_CLK) 
        track_data_r2    <=   `DLY  track_data_r;  

    always @(posedge USER_CLK) 
        track_data_r3    <=   `DLY  track_data_r2; 

    //______________________________ Capture incoming data ____________________    

    always @(posedge USER_CLK)
    begin
        if(system_reset_r2)    rx_data_r3 <= 'h0;
        else 
        begin
            if(sel == 3'b001)
            begin
                rx_data_r3   <=  `DLY    {rx_data_r[(RX_DATA_WIDTH/8-1):0],rx_data_r2[(RX_DATA_WIDTH - 1):RX_DATA_WIDTH/8]};  
            end
            else if(sel == 3'b010)
            begin
                rx_data_r3   <=  `DLY    {rx_data_r[(2*RX_DATA_WIDTH/8-1):0],rx_data_r2[(RX_DATA_WIDTH - 1):2*RX_DATA_WIDTH/8]};  
            end
            else if(sel == 3'b011)
            begin
                rx_data_r3   <=  `DLY    {rx_data_r[(3*RX_DATA_WIDTH/8-1):0],rx_data_r2[(RX_DATA_WIDTH - 1):3*RX_DATA_WIDTH/8]};  
            end
            else if(sel == 3'b100)
            begin
                rx_data_r3   <=  `DLY    {rx_data_r[(4*RX_DATA_WIDTH/8-1):0],rx_data_r2[(RX_DATA_WIDTH - 1):4*RX_DATA_WIDTH/8]};  
            end
            else if(sel == 3'b101)
            begin
                rx_data_r3   <=  `DLY    {rx_data_r[(5*RX_DATA_WIDTH/8-1):0],rx_data_r2[(RX_DATA_WIDTH - 1):5*RX_DATA_WIDTH/8]};  
            end
            else if(sel == 3'b110)
            begin
                rx_data_r3   <=  `DLY    {rx_data_r[(6*RX_DATA_WIDTH/8 - 1):0],rx_data_r2[(RX_DATA_WIDTH-1):6*RX_DATA_WIDTH/8]};  
            end
            else if(sel == 3'b111)
            begin
                rx_data_r3   <=  `DLY    {rx_data_r[(7*RX_DATA_WIDTH/8 - 1):0],rx_data_r2[(RX_DATA_WIDTH-1):7*RX_DATA_WIDTH/8]};  
            end
            else rx_data_r3  <=  `DLY    rx_data_r2;

        end
    end

    always @(posedge USER_CLK)
    begin
        if(system_reset_r2)  
        begin
            rx_data_r4      <=  `DLY   'h0;
            rx_data_r5      <=  `DLY   'h0;
            rx_data_r6      <=  `DLY   'h0;
            rx_data_r_track <=  `DLY   'h0;
        end
        else
        begin
            rx_data_r4      <=  `DLY    rx_data_r3;
            rx_data_r5      <=  `DLY    rx_data_r4;
            rx_data_r6      <=  `DLY    rx_data_r5;
            rx_data_r_track <=  `DLY    rx_data_r6;
        end
    end

    always @(posedge USER_CLK)
    begin
        if(system_reset_r2)  
        begin
            rxctrl_r2      <=  `DLY   'h0;
            rxctrl_r3      <=  `DLY   'h0;
        end
        else
        begin
            rxctrl_r2      <=  `DLY   rxctrl_r;
            rxctrl_r3      <=  `DLY   rxctrl_r2;
        end
    end
    
    assign rx_data_aligned = rx_data_r3;

    // { BYTE7 | BYTE6 | BYTE5 | BYTE4 | BYTE3 | BYTE2 | BYTE1 | BYTE0 } - Comma can appear on BYTE0/1/2/3/4/5/6/7
    // If Comma appears on BYTE1/2/3/4/5/6/7, RX_DATA is realigned so that Comma appears on BYTE0 in rx_data_r_track
    always @(posedge USER_CLK)
    begin
//        if(reset_on_error_in_r2 || system_reset_r2)    sel <= 3'b000;
        if(system_reset_r2)    sel <= 3'b000;
//        else if (begin_r && !rx_chanbond_seq_r)
        else if (begin_r)
        begin
            // if Comma appears on BYTE7 ..
            if((rx_data_r[(RX_DATA_WIDTH - 1) : 7*RX_DATA_WIDTH/8] == START_OF_PACKET_CHAR[7:0]) && rxctrl_r[7])
            begin
                sel <= 3'b111;        
            end
            // if Comma appears on BYTE6 ..
            else if((rx_data_r[(7*RX_DATA_WIDTH/8 - 1):6*RX_DATA_WIDTH/8] == START_OF_PACKET_CHAR[7:0]) && rxctrl_r[6])
            begin
                sel <= 3'b110;
            end
            // if Comma appears on BYTE5 ..
            else if((rx_data_r[(6*RX_DATA_WIDTH/8 - 1):5*RX_DATA_WIDTH/8] == START_OF_PACKET_CHAR[7:0]) && rxctrl_r[5])
            begin
                sel <= 3'b101;
            end
            // if Comma appears on BYTE4 ..
            else if((rx_data_r[(5*RX_DATA_WIDTH/8 - 1):4*RX_DATA_WIDTH/8] == START_OF_PACKET_CHAR[7:0]) && rxctrl_r[4])
            begin
                sel <= 3'b100;
            end
            // if Comma appears on BYTE3 ..
            else if((rx_data_r[(4*RX_DATA_WIDTH/8 - 1):3*RX_DATA_WIDTH/8] == START_OF_PACKET_CHAR[7:0]) && rxctrl_r[3])
            begin
                sel <= 3'b011;
            end
            // if Comma appears on BYTE2 ..
            else if((rx_data_r[(3*RX_DATA_WIDTH/8 - 1):2*RX_DATA_WIDTH/8] == START_OF_PACKET_CHAR[7:0]) && rxctrl_r[2])
            begin
                sel <= 3'b010;
            end
            // if Comma appears on BYTE1 ..
            else if((rx_data_r[(2*RX_DATA_WIDTH/8 - 1):RX_DATA_WIDTH/8] == START_OF_PACKET_CHAR[7:0]) && rxctrl_r[1])
            begin
                sel <= 3'b001;
            end
            // if Comma appears on BYTE0 ..
            else if((rx_data_r[(RX_DATA_WIDTH/8 - 1):0] == START_OF_PACKET_CHAR[7:0]) && rxctrl_r[0])
            begin
                sel <= 3'b000;
            end
        end      
    end

//    assign rx_data_has_start_char_c = (rx_data_aligned[7:0] == START_OF_PACKET_CHAR[7:0]) && !chanbondseq_in_data && (|rxctrl_r3);
    assign rx_data_has_start_char_c = (rx_data_aligned[7:0] == START_OF_PACKET_CHAR[7:0]) && (|rxctrl_r3);


    //_____________________________ Assign output ports _______________________    
    assign TRACK_DATA_OUT = track_data_r;


    // Drive the enpcommaalign port of the gt for alignment
    always @(posedge USER_CLK)
        if(system_reset_r2)  RXENPCOMMADET_OUT   <=  `DLY    1'b0;
        else              RXENPCOMMADET_OUT   <=  `DLY    1'b1;

    // Drive the enmcommaalign port of the gt for alignment
    always @(posedge USER_CLK)
        if(system_reset_r2)  RXENMCOMMADET_OUT   <=  `DLY    1'b0;
        else              RXENMCOMMADET_OUT   <=  `DLY    1'b1;


    //___________________________ Check incoming data for errors ______________

    //We register the error_detected signal for use with the error counter logic
    always @(posedge USER_CLK)
        if(!track_data_r)  
            error_detected_r    <=  `DLY    1'b0;
        else
            error_detected_r    <=  `DLY    error_detected_c;  

    //We count the total number of errors we detect. By keeping a count we make it less likely that we will miss
    //errors we did not directly observe. 
    always @(posedge USER_CLK)
        if(system_reset_r2)
            error_count_r       <=  `DLY    63'd0;
        else if(error_detected_r)
            error_count_r       <=  `DLY    error_count_r + 1;    
    
    //Here we connect the lower 8 bits of the count (the MSbit is used only to check when the counter reaches
    //max value) to the module output

    //Here we connect the lower 8 bits of the count (the MSbit is used only to check when the counter reaches
    //max value) to the module output
    assign  ERROR_COUNT_OUT =   error_count_r;

 (* keep = "true" *)reg     [63:0] expected_data_i; 
 (* keep = "true" *)reg     [63:0] bit_error;
 (* keep = "true" *)reg     [63:0] timer;
 (* keep = "true" *)wire    [63:0] tmp;
 (* keep = "true" *)wire           complete;
 (* keep = "true" *)wire     [63:0] data_xored; 

    //An error is detected when data read for the BRAM does not match the incoming data
    assign error_detected_c    =  track_data_r3 && ((rx_data_r_track != expected_data_i) && (rx_data_r_track != 64'hFEDC_BA98_7654_32BC)); 
//    assign tmp = (error_detected_c) ? ber_calc(expected_data_i, rx_data_r_track): 0;
    assign complete = (timer>`BER) ? 1'b1 : 1'b0;
    assign data_xored = (error_detected_c)? rx_data_r_track ^ expected_data_i: 0;
    assign tmp = ber_calc(data_xored[15:0]) + ber_calc(data_xored[31:16]) + ber_calc(data_xored[47:32]) + ber_calc(data_xored[63:48]);
                    
    // Assign TX_DATA_OUT to BRAM output
    always @(posedge USER_CLK) begin
        if(system_reset_r2| (!track_data_r2)) begin
            expected_data_i = 64'h0; 
            bit_error <= 64'h0; 
            timer <= 64'h0; 
        end
        else begin            
            if (track_data_r2 & !track_data_r3)    
                expected_data_i       <= rx_data_r5;
            else begin 
                if (track_data_r3) begin               
                    if (rx_data_r_track!= 64'hFEDC_BA98_7654_32BC) begin
                        expected_data_i       <= expected_data_i + 64'h1;
                        timer       <= timer + 64'h1;
                        bit_error = bit_error + tmp;
                    end    
                    else
                        expected_data_i       <= rx_data_r6;                        
                end    
                else
                    expected_data_i       <= 64'h0;
            end    
        end
    end // end always
        
    assign TRACK_DATA_OUT = track_data_r;

    function [15:0] ber_calc;
        input   wire [15:0]  xored;
        integer         i;  
        integer         result;
        begin    
            for (i=0; i<16; i=i+1)
                result = result + xored[i];            
            
            ber_calc = result;
        end
    endfunction    
    
endmodule           
