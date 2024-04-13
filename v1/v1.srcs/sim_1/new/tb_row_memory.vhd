library ieee;
use ieee.std_logic_1164.all;
use std.env.finish;


entity tb_row_memory is
end tb_row_memory;


architecture Behavioral of tb_row_memory is
    constant PERIOD : time := 20 ns;
    constant memory_word_size : natural := 64;
    constant memory_address_size : natural := 3;
    
    component row_memory is
    generic (
        memory_word_size : integer; -- number of bits per word
        memory_address_size: integer); -- number of address bits; N = 2^A
    port (
        clk: in std_logic; -- active high write enable
        we: in std_logic; -- active high write enable
        addr: in std_logic_vector (memory_address_size-1 downto 0); -- RAM address
        data_in: in std_logic_vector (memory_word_size - 1 downto 0); -- write data
        data_out: out std_logic_vector (memory_word_size - 1 downto 0)); -- read data
    end component;
    
    signal clk	: std_logic := '0';
    signal we	: std_logic := '0' ;
    signal addr	    : std_logic_vector(memory_address_size-1 downto 0);
    signal data_in	: std_logic_vector(memory_word_size -1 downto 0);
    signal data_out	: std_logic_vector(memory_word_size -1 downto 0);

begin
    clk  <= not clk after PERIOD/2;

    process
    begin
        wait until (rising_edge(clk));
        wait until (rising_edge(clk));
        data_in <= x"0000_0000_0000_0000";
        addr <= "000";
        we <= '1';
        
        wait until (rising_edge(clk));
        data_in <= x"0000_0000_0000_0001";
        addr <= "001";
        
        wait until (rising_edge(clk));
        data_in <= x"0000_0000_0000_0002";
        addr <= "010";
        
        wait until (rising_edge(clk));
        data_in <= x"0000_0000_0000_0000";
        addr <= "000";
        we <= '0';
        
        wait until (rising_edge(clk));
        addr <= "001";
        
        wait until (rising_edge(clk));
        addr <= "010";
        
        wait until (rising_edge(clk));
        wait until (rising_edge(clk));
        
        finish;
    end process;

    row_memory_i : row_memory
    generic map (
        memory_word_size =>  memory_word_size,
        memory_address_size => memory_address_size
    )
    port map ( 
      clk=>clk,
      we=>we,
      addr=>addr,
      data_in=>data_in,
      data_out=>data_out
    ); 


end Behavioral;
