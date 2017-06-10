(If your operating system is Window, you are kindly suggested to open all 
the files by using Microsoft Wordpad instead of Microsoft Notepad)

1. Files
---------
1.1 run.x - executable file in Linux
    run.exe - executable file in Windows
1.2 IP_init.txt - IP information input file example
    Signal_init.txt - Signal information input file example
1.3 top_linux.vhd - the top-level entity output example for Linux
    top_windows.vhd - the top-level entity output example for Windows

2. How to use
—--------------
2.1 In Linux

2.1.1 Compile
      gcc -o run.x Top_Entity_Code_Generator.c 

2.1.2 Execute
      run.x [IP_info_file] [Signal_info_file] [Output_file]
      for instance:
      ./run.x IP_init.txt Signal_init.txt top_linux.vhd

2.2 In Windows

2.2.1 Compile
      gcc -o run.exe Top_Entity_Code_Generator.c 

2.2.2 Execute
      run.exe [IP_info_file] [Signal_info_file] [Output_file]
      for instance:
      run.exe IP_init.txt Signal_init.txt top_windows.vhd
