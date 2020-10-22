# InterproecssPOC
Interprocess communication between a master C# client and a child C++ server over a named pipe   
The master application is Aetrex.IPC.cs which is a C# console application.  
It creates a Windows OS job and adds Aetrex.IPC.cs application to the job. A job is a way to group processes in Windows. By default, when the master process terminates, all the child processes that the master created are terminated by Windows as well.  
Aetrex.IPC.cs creates Aetrex.IPCCPP.Server process, which is a pipe server. Aetrex.IPCCPP.Server is launched in a hidden window. The This happens in IPCClient class constructor. After creating the server process, the client connects to the pipe and waits for a note "I am the one true server!" from the server.
Upon receiving the note from the server, Aetrex.IPC.cs sends a message to the server "Message from client", closes the pipe and is ready to be terminated.  
Pressing any key while Aetrex.IPC.cs is in focus, terminates Aetrex.IPC.cs and all its child processing. This can be observed in Windows Task Manager.  
The server code can be found in Aetrex.IPCCPP.Server solution. It is a C++ console application that creates a thread that starts a named pipe and wait for a client to connect.  
When a client connects, Aetrex.IPCCPP.Server sends the client a welcome string.  