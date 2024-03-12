library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use std.env.finish;


entity tb_row_memory is
end tb_row_memory;


architecture Behavioral of tb_row_memory is
    constant PERIOD : time := 20 ns;
    constant A : natural := 3;
    constant K : natural := 64;
    
    component row_memory is
    generic (
        K: integer := 64; -- number of bits per word
        A: integer := 3); -- number of address bits; N = 2^A
    port (
        clk: in std_logic; -- active high write enable
        we: in std_logic; -- active high write enable
        addr: in std_logic_vector (A-1 downto 0); -- RAM address
        data_in: in std_logic_vector (K-1 downto 0); -- write data
        data_out: out std_logic_vector (K-1 downto 0)); -- read data
    end component;
    
    signal clk	: std_logic := '0';
    signal we	: std_logic := '0' ;
    signal addr	    : std_logic_vector(A-1 downto 0);
    signal data_in	: std_logic_vector(K-1 downto 0);
    signal data_out	: std_logic_vector(K-1 downto 0);

begin
    clk  <= not clk after PERIOD/2;

    process
    begin
        wait until (rising_edge(clk));
        wait until (rising_edge(clk));
        data_in <= x"0000_0000_0000_1546";
        addr <= "000";
        we <= '1';
        wait until (rising_edge(clk));
        data_in <= x"0000_0000_0000_0000";
        addr <= "000";
        we <= '0';
        wait until (rising_edge(clk));
        wait until (rising_edge(clk));
        
        finish;
    end process;

    row_memory_i : row_memory
    generic map (
        K => K,
        A => A
    )
    port map ( 
      clk=>clk,
      we=>we,
      addr=>addr,
      data_in=>data_in,
      data_out=>data_out
    ); 


end Behavioral;
