library IEEE;
use IEEE.STD_LOGIC.ALL;

ENTITY System IS
    PORT (
          OP_A : in std_logic_vector(7 downto 0);
          OP_B : in std_logic_vector(7 downto 0);
          OUTPUT : out std_logic_vector(7 downto 0)
          );
END System;

ARCHITECTURE struct OF System IS
    component Reg_8
    PORT (
          data_in : in std_logic_vector(7 downto 0);
          data_out : out std_logic_vector(7 downto 0)
         );
    end component;

    component RCA
    PORT (
          A : in std_logic_vector(7 downto 0);
          B : in std_logic_vector(7 downto 0);
          OUT : out std_logic_vector(7 downto 0)
         );
    end component;

signal s_0 : std_logic_vector(7 downto 0);
signal s_1 : std_logic_vector(7 downto 0);
signal s_2 : std_logic_vector(7 downto 0);
signal s_3 : std_logic_vector(7 downto 0);
signal s_4 : std_logic_vector(7 downto 0);
signal s_6 : std_logic_vector(7 downto 0);

 begin

s_0 <= OP_A;
s_1 <= OP_B;
s_6 <= OUTPUT;

Reg_8_1 : Reg_8 port map(data_in => s_0, data_out => s_2);
Reg_8_2 : Reg_8 port map(data_in => s_1, data_out => s_3);
Reg_8_4 : Reg_8 port map(data_in => s_4, data_out => s_6);
RCA_3 : RCA port map(A => s_2, B => s_3, OUT => s_4);

END struct;
