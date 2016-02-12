----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 	Shreya V Deodhar
-- 
-- Create Date:    11:15:39 02/18/2015 
-- Design Name: 
-- Module Name:    spislave - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity spislave is
    Port ( slvSCK  : in  STD_LOGIC;  
           slvMOSI : in  STD_LOGIC;  
              LED  : inout STD_LOGIC_VECTOR (7 downto 0));
end spislave;

architecture Behavioral of spislave is
signal dat_reg : STD_LOGIC_VECTOR (7 downto 0);
begin

    process (slvSCK)
    begin
        if (slvSCK'event and slvSCK = '0') then  
        dat_reg <= dat_reg(6 downto 0) & slvMOSI;
        end if;
    end process;

    LED <= dat_reg;

end Behavioral;