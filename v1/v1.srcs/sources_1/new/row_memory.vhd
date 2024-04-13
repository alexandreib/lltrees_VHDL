library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_textio.all;
use ieee.numeric_std.all;
use ieee.math_real.all;
use ieee.std_logic_unsigned.all;

entity row_memory is

generic (
    memory_word_size: integer; -- number of bits per word
    memory_address_size: integer); -- 16 address
    
port (
    clk: in std_logic; -- active high write enable
    we: in std_logic; -- active high write enable
    
    write_addr: in std_logic_vector (memory_address_size-1 downto 0); -- RAM address
    data_in: in std_logic_vector (memory_word_size-1 downto 0); -- write data
    
    read_addr: in std_logic_vector (memory_address_size-1 downto 0); -- RAM address
    data_out: out std_logic_vector (memory_word_size-1 downto 0)); -- read data
    
end entity row_memory;

architecture RAMBEHAVIOR of row_memory is

    subtype WORD is std_logic_vector ( memory_word_size-1 downto 0); -- define size of WORD
    type MEMORY is array (0 to memory_address_size -1 ) of WORD; -- define size of MEMORY
    signal RAM : MEMORY;
begin

    process (clk)
        variable RAM_ADDR_IN: natural range 0 to 7; -- translate address to integer
    begin
        if (clk'event and clk = '1') then
            if (we = '1') then
                RAM(conv_integer(write_addr)) <= data_in;
            end if;
        end if;
    
    end process;

data_out <= RAM(conv_integer(read_addr));
    

end architecture RAMBEHAVIOR;
-- From Xilinx "Synthesis and Simulation Design Guide"