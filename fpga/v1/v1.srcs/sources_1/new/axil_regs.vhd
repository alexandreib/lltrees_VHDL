library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


    
entity axil_regs is
  generic (
    C_DATA_W : integer := 64;
    C_ADDR_W : integer := 8 ); -- 
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
end axil_regs;

architecture arch_imp of axil_regs is
    
    -- AXI4LITE signals
    signal axi_arready  : std_logic;
    signal axi_rvalid   : std_logic;
    signal axi_awready  : std_logic;
    signal axi_bvalid   : std_logic;
    
    -- used to store write / read addresses
    signal axi_awaddr   : std_logic_vector(C_ADDR_W - 1 downto 0);
    signal axi_araddr   : std_logic_vector(C_ADDR_W - 1 downto 0);
    
    -- State machine for read / write
    type rd_sm is (idle, start_rd, rd_data);
    signal rd_st : rd_sm;
    type wr_sm is (idle, wr_data, wr_resp);
    signal wr_st : wr_sm;
  
    constant VER_ADDR : std_logic_vector(C_ADDR_W - 1 downto 0) := "00000000";
    constant CMD_ADDR : std_logic_vector(C_ADDR_W - 1 downto 0) := "00000001";
    constant MEM_ADDR : std_logic_vector(C_ADDR_W - 1 downto 0) := "00000010";
    
    signal reg_version : std_logic_vector(C_DATA_W - 1 downto 0) := x"0000_0000_0000_FFFF";
    signal reg_command : std_logic_vector(C_DATA_W - 1 downto 0) := x"0000_0000_FFFF_0000";
       
--    ----------------------------------------------------------------
--    -- Memory
--    ----------------------------------------------------------------
--    constant memory_word_size : natural := 64;
--    constant memory_address_size : natural := 4;
    
--    component row_memory is
--    generic (
--        memory_word_size : integer; -- number of bits per word
--        memory_address_size: integer); -- number of address bits; N = 2^A
--    port (
--        clk: in std_logic;
--        we: in std_logic;
--        addr: in std_logic_vector (memory_address_size-1 downto 0); -- RAM address
--        data_in: in std_logic_vector (memory_word_size - 1 downto 0); -- write data
--        data_out: out std_logic_vector (memory_word_size - 1 downto 0)); -- read data
--    end component;
    
--    signal memory_clk	   : std_logic;
--    signal memory_we	   : std_logic;
--    signal memory_addr	   : std_logic_vector(memory_address_size-1 downto 0);
--    signal memory_data_in  : std_logic_vector(memory_word_size-1 downto 0);
--    signal memory_data_out : std_logic_vector(memory_word_size-1 downto 0);

begin  

--    -- memory connections assignments
--    row_memory_i : row_memory
--    generic map (
--        memory_word_size => memory_word_size,
--        memory_address_size=> memory_address_size)
--    port map ( 
--        clk=>memory_clk,
--        we=>memory_we,
--        addr=>memory_addr,
--        data_in=>memory_data_in,
--        data_out=>memory_data_out
--    ); 

    --------------------------------------------- READ --------------------------------------------- 
    s_axi_arready   <= axi_arready;
    s_axi_rvalid    <= axi_rvalid;
    process (s_axi_aclk)
    begin
    if rising_edge(s_axi_aclk) then
        if s_axi_aresetn = '0' then
            axi_arready <= '1';
            axi_rvalid <= '0';
            rd_st <= idle;
        else
            case rd_st is
            when idle =>
                if (s_axi_arvalid = '1' and axi_arready = '1') then
                    axi_araddr  <= s_axi_araddr; -- store read address
                    axi_arready <= '0';  -- address received, stop receiving additional addresses
                    axi_rvalid  <= '0';
                    rd_st       <= start_rd;
                end if;
            when start_rd =>
                axi_rvalid      <= '1';
                rd_st           <= rd_data;
            when rd_data =>
                if (s_axi_rready = '1' and axi_rvalid = '1' ) then
                    axi_rvalid  <= '0';
                    axi_arready <= '1'; -- data received (or timeout), address bus is ready for new addr
                    rd_st <= idle;
                end if;
            end case;
        end if;
    end if;
    end process;


    axi_rdata_proc : process(s_axi_aclk) is
        variable loc_addr : std_logic_vector(C_ADDR_W-1 downto 0);
    begin
    if (rising_edge (s_axi_aclk)) then
        if (s_axi_aresetn = '0') then
            s_axi_rdata <= (others => '0');
            s_axi_rresp <= "00";
        else        
            if rd_st = rd_data then
                loc_addr := axi_araddr(C_ADDR_W-1 downto 0);
                case loc_addr is
                    when VER_ADDR =>
                        s_axi_rdata <= reg_version;
                        s_axi_rresp <= "00";
                    when CMD_ADDR =>
                        s_axi_rdata <= reg_command;
                        s_axi_rresp <= "00";
                    when others =>
                        s_axi_rdata <= (others => '0'); -- slave decode error, read register does not exist
                        s_axi_rresp <= "10";
                end case;
            end if;
        end if;
    end if;
    end process axi_rdata_proc;
    
  --------------------------------------------- WRITE --------------------------------------------- 
    s_axi_awready   <= axi_awready;
    s_axi_bvalid    <= axi_bvalid;
    process (s_axi_aclk)
    begin
    if rising_edge(s_axi_aclk) then
        if s_axi_aresetn = '0' then
            axi_awready <= '1';
            axi_bvalid <= '0';
            wr_st <= idle;
        else
            case wr_st is
            when idle =>
            if (s_axi_awvalid  = '1' and axi_awready = '1') then
                axi_awaddr  <= s_axi_araddr; -- store write address
                axi_awready <= '0'; -- address received, stop receiving additional addresses
                axi_bvalid  <= '0';
                
                -- Check if waddr and wdata were sent on same clock
                if (s_axi_wvalid = '1') then
                    axi_bvalid <= '1';
                    wr_st <= wr_resp;
                else
                    wr_st <= wr_data;
                end if;
            end if;
            
            when wr_data =>
                if (s_axi_wvalid = '1') then
                    axi_bvalid <= '1';
                    wr_st <= wr_resp;
                end if;
            
            when wr_resp =>
                    axi_bvalid <= '1';
                if (s_axi_bready = '1' and axi_bvalid = '1') then
                    axi_bvalid <= '0';
                    axi_awready <= '1'; -- data received, address bus is ready for new addr
                    wr_st <= idle;
                end if;
            end case;
        end if;
    end if;            
    end process;
    axi_wdata_proc : process (s_axi_aclk)
        variable loc_addr : std_logic_vector(C_ADDR_W - 1 downto 0);
    begin
    if rising_edge(s_axi_aclk) then
        if s_axi_aresetn = '0' then
            s_axi_bresp <= "00";
        else
            if wr_st = wr_resp then
                loc_addr := axi_awaddr(C_ADDR_W-1 downto 0);
                case loc_addr is
                when VER_ADDR =>
                    s_axi_bresp <= "00";
                    reg_version <= s_axi_wdata;
                when CMD_ADDR =>
                    s_axi_bresp <= "00";
                    reg_command <= s_axi_wdata;
                when others =>
                    s_axi_bresp <= "10"; -- slave decoder error, register is read/only or does not exist
                end case;
            end if;
        end if;
    end if;
    end process;  
    
    
    
end arch_imp;
