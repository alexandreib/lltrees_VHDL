library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_textio.all;
use ieee.numeric_std.all;
use ieee.math_real.all;
use std.textio.all;
use std.env.stop;
--library xil_defaultlib;
--use xil_defaultlib.axil_slave_bfm.all;

entity tb_axil_regs is
end entity;

architecture test of tb_axil_regs is

    constant PERIOD : time := 20 ns;
    constant C_DATA_W : natural := 64;
    constant C_ADDR_W : natural := 8;
    signal data           : std_logic_vector(C_DATA_W-1 downto 0);    
    signal addr	          : std_logic_vector(C_ADDR_W-1 downto 0);
    
    signal s_axi_aclk     : std_logic := '0';
    signal s_axi_aresetn  : std_logic := '0';
    signal s_axi_awvalid  : std_logic;
    signal s_axi_awready  : std_logic;
    signal s_axi_awaddr	  : std_logic_vector(C_ADDR_W-1 downto 0);
    signal s_axi_wvalid	  : std_logic;
    signal s_axi_wdata	  : std_logic_vector(C_DATA_W-1 downto 0);
    signal s_axi_wready	  : std_logic;
    signal s_axi_bvalid	  : std_logic;
    signal s_axi_bready	  : std_logic;
    signal s_axi_bresp	  : std_logic_vector(1 downto 0);
    
    signal s_axi_arvalid  : std_logic;
    signal s_axi_arready  : std_logic;
    signal s_axi_araddr	  : std_logic_vector(C_ADDR_W-1 downto 0);
    signal s_axi_rvalid	  : std_logic;
    signal s_axi_rready	  : std_logic;
    signal s_axi_rdata	  : std_logic_vector(C_DATA_W-1 downto 0);
    signal s_axi_rresp	  : std_logic_vector(1 downto 0);
  
    component axil_regs is
        generic (
            C_DATA_W	   : integer	:= 64;
            C_ADDR_W	   : integer	:= 8
        );
        port (
            s_axi_aclk    : in  std_logic; -- Clock signal. All inputs/outputs of this bus interface are rising edge aligned with this clock.
            s_axi_aresetn : in  std_logic; -- Active-Low synchronous reset signal
            
            s_axi_awvalid : in  std_logic; -- Write address valid. This signal indicates that the channel is signaling valid write address.
            s_axi_awready : out std_logic; -- Write address ready. This signal indicates that the slave is ready to accept an address.
            s_axi_awaddr  : in  std_logic_vector(C_ADDR_W - 1 downto 0); -- Write address. The write address gives the address of the transaction.
            s_axi_wvalid  : in  std_logic; -- Write valid. This signal indicates that valid write data are available.
            s_axi_wdata   : in  std_logic_vector(C_DATA_W - 1 downto 0); -- Write data.
            s_axi_wready  : out std_logic; -- Write ready. This signal indicates that the slave can accept the write data.
            s_axi_bvalid  : out std_logic; -- Write response valid. This signal indicates that the channel is signaling a valid write response.
            s_axi_bready  : in  std_logic; -- Write response ready. This signal indicates that the master can accept a write response.
            s_axi_bresp   : out std_logic_vector(1 downto 0);-- Write response. This signal indicate the status of the write transaction.
            
            s_axi_arvalid : in  std_logic; -- Read address valid. This signal indicates that the channel is signaling valid read address.
            s_axi_arready : out std_logic; -- Read address ready. This signal indicates that the slave is ready to accept an address.
            s_axi_araddr  : in  std_logic_vector(C_ADDR_W - 1 downto 0); -- Read address. The read address gives the address of the transaction.
            s_axi_rvalid  : out std_logic; -- Read valid. This signal indicates that the channel is signaling the required read data.
            s_axi_rready  : in  std_logic; --Read ready. This signal indicates that the master can accept the read data and response information.
            s_axi_rdata   : out std_logic_vector(C_DATA_W - 1 downto 0); -- Read data.
            s_axi_rresp   : out std_logic_vector(1 downto 0) -- Read response. This signal indicate the status of the read transfer.
        );
    end component;

  
    procedure readAxil (
        constant addr           : in  std_logic_vector(C_ADDR_W-1 downto 0);
   
        signal s_axi_aclk       : in  std_logic;
        signal s_axi_arready    : in  std_logic;
        signal s_axi_rvalid     : in  std_logic;
        signal s_axi_araddr     : out std_logic_vector(C_ADDR_W-1 downto 0);
        signal s_axi_arvalid    : out std_logic;
        signal s_axi_rready     : out std_logic
        ) is
    begin
        s_axi_araddr  <= addr;
        s_axi_arvalid <= '1';
        wait until (rising_edge(s_axi_aclk) and s_axi_arready = '1');
        s_axi_arvalid <= '0';
        wait until (rising_edge(s_axi_aclk) and s_axi_rvalid = '1');
        s_axi_rready <= '1';
        wait until (rising_edge(s_axi_aclk));
        s_axi_rready <= '0';
    end procedure;
       
    procedure writeAxil (    
        constant addr : in  std_logic_vector(C_ADDR_W-1 downto 0);
        constant data : in  std_logic_vector(C_DATA_W-1 downto 0);
        
        signal s_axi_aclk    : in  std_logic;
        signal s_axi_bvalid : in  std_logic;
        signal s_axi_awaddr  : out std_logic_vector(C_ADDR_W-1 downto 0);
        signal s_axi_wdata : out std_logic_vector(C_DATA_W-1 downto 0);
        signal s_axi_wvalid  : out std_logic;
        signal s_axi_awvalid : out std_logic;        
        signal s_axi_bready : out std_logic
    ) is
    begin
        s_axi_awaddr  <= addr;
        s_axi_wdata   <= data;
        s_axi_awvalid <= '1';
        s_axi_wvalid <= '1';
        s_axi_bready  <= '0';
        wait until (s_axi_bvalid = '1');
        s_axi_awvalid <= '0';
        s_axi_wvalid  <= '0';
        wait until rising_edge(s_axi_aclk);
        s_axi_bready  <= '1';
        wait until (s_axi_bvalid = '0');
        s_axi_bready  <= '0';
        wait until rising_edge(s_axi_aclk);
        end procedure;
  
begin
    s_axi_aclk  <= not s_axi_aclk after PERIOD/2;
    s_axi_aresetn <= '1' after PERIOD * 5;
    
    process
    begin
    wait until (s_axi_aresetn = '1');
    wait until (rising_edge(s_axi_aclk));
    
    addr <= x"00";
    wait until (rising_edge(s_axi_aclk));
    readAxil (addr, s_axi_aclk, s_axi_arready, s_axi_rvalid, s_axi_araddr, s_axi_arvalid, s_axi_rready);  

    addr <= x"01";
    wait until (rising_edge(s_axi_aclk));
    readAxil (addr, s_axi_aclk, s_axi_arready, s_axi_rvalid, s_axi_araddr, s_axi_arvalid, s_axi_rready);  
        
    wait until (rising_edge(s_axi_aclk));
    wait until (rising_edge(s_axi_aclk));
    wait until (rising_edge(s_axi_aclk));
    
    addr <= x"00";
    data <= x"1234567812345678";
    wait until (rising_edge(s_axi_aclk));
    writeAxil (addr, data, s_axi_aclk, s_axi_bvalid, s_axi_awaddr, s_axi_wdata, s_axi_wvalid, s_axi_awvalid, s_axi_bready);  
    
    addr <= x"01";
    data <= x"1234567812345678";
    wait until (rising_edge(s_axi_aclk));
    writeAxil (addr, data, s_axi_aclk, s_axi_bvalid, s_axi_awaddr, s_axi_wdata, s_axi_wvalid, s_axi_awvalid, s_axi_bready);  
        
    wait until (rising_edge(s_axi_aclk));
    wait until (rising_edge(s_axi_aclk));
    wait until (rising_edge(s_axi_aclk));
        
    addr <= x"00";
    wait until (rising_edge(s_axi_aclk));
    readAxil (addr, s_axi_aclk, s_axi_arready, s_axi_rvalid, s_axi_araddr, s_axi_arvalid, s_axi_rready);  

    addr <= x"01";
    wait until (rising_edge(s_axi_aclk));
    readAxil (addr, s_axi_aclk, s_axi_arready, s_axi_rvalid, s_axi_araddr, s_axi_arvalid, s_axi_rready);  
    
    stop;
end process;


  axil_regs_i : axil_regs
    generic map (
        C_DATA_W => C_DATA_W,
        C_ADDR_W => C_ADDR_W
    )
    port map ( 
      s_axi_aclk	=> s_axi_aclk,
      s_axi_aresetn	=> s_axi_aresetn,
      s_axi_awaddr	=> s_axi_awaddr,
      s_axi_awvalid	=> s_axi_awvalid,
      s_axi_awready	=> s_axi_awready,
      s_axi_wdata	=> s_axi_wdata,
      s_axi_wvalid	=> s_axi_wvalid,
      s_axi_wready	=> s_axi_wready,
      s_axi_bresp	=> s_axi_bresp,
      s_axi_bvalid	=> s_axi_bvalid,
      s_axi_bready	=> s_axi_bready,
      s_axi_araddr	=> s_axi_araddr,
      s_axi_arvalid	=> s_axi_arvalid,
      s_axi_arready	=> s_axi_arready,
      s_axi_rdata	=> s_axi_rdata,
      s_axi_rresp	=> s_axi_rresp,
      s_axi_rvalid	=> s_axi_rvalid,
      s_axi_rready	=> s_axi_rready	        
    );              
end architecture;