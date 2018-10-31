# Daemon
Disk monitor daemon for linux

Running:
1) Set directories to watch in config.txt
2) Run command make
3) Run command ./daemon start
4) In log you can see changes in chosen directories

Stopping:
1) Run command ./daemon stop

Testing:
1) Run command ./daemon start
2) Run command ./tester.sh
3) Watch changes in log

Configuration file format:
1) Write one path to watch after on each line of config file
2) Name it config.txt
