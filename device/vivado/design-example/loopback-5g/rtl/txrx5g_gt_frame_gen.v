`timescale 1ns / 1ps
`define DLY #1
//***********************************Entity Declaration*******************************
(* DowngradeIPIdentifiedWarnings="yes" *)
module TxRx5g_GT_FRAME_GEN 
(
       // User Interface
    output  [63:0]  TX_DATA_OUT,
    output  [7:0]   TXCTRL_OUT,
    
          // System Interface
    input           USER_CLK,
    input           SYSTEM_RESET 
); 
//********************************* Wire Declarations********************************* 
(* ASYNC_REG = "TRUE" *) (* keep = "true" *)    reg     system_reset_r; 
(* ASYNC_REG = "TRUE" *) (* keep = "true" *)    reg     system_reset_r2; 
(* keep = "true" *)reg    [7:0]   read_counter_i;
(* keep = "true" *)reg    [63:0]  tx_data_i;
(* keep = "true" *)reg    [7:0]   tx_ctrl_i;
    //___________ synchronizing the async reset for ease of timing simulation ________
    always@(posedge USER_CLK)
        begin
            system_reset_r <= `DLY SYSTEM_RESET;
            system_reset_r2 <= `DLY system_reset_r;
        end
        
    // Assign TX_DATA_OUT to BRAM output
    always @(posedge USER_CLK) begin
        if(system_reset_r2) begin
            read_counter_i  <= `DLY 8'd0;
            tx_data_i       <= `DLY 64'h0; 
            tx_ctrl_i       <= `DLY 1'h0;   
        end
        else begin            
            read_counter_i  <=  `DLY  read_counter_i+ 8'd1;
            tx_data_i       <= `DLY tx_data_i + 64'h1; 
            
            if (&read_counter_i[3:0]) begin
                tx_ctrl_i       <= `DLY 1'h1;              
            end  
            else begin
                tx_ctrl_i       <= `DLY 1'h0;                              
            end    
        end
    end // end always

    //________________________________ BRAM Inference Logic _____________________________    

    assign TX_DATA_OUT      =   (read_counter_i[3:0]==4'h0) ? 64'hFEDC_BA98_7654_32BC : tx_data_i;
                                
    assign TXCTRL_OUT       =   tx_ctrl_i;

endmodule 

