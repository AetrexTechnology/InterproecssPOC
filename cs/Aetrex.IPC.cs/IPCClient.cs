using System;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Security.Principal;
using System.Text;
using System.Threading;

namespace Aetrex.IPC.cs
{
    internal class IPCClient
    {
        private static int numClients = 4;
        private Process IPCServerProcess = null;

        public IPCClient(bool spawnclient)
        {
            //Create a Windows job object, register this process to the job so on job termination, this process and its child processes will terminate by Windows OS
            JobManagement.Job job = new JobManagement.Job();
            job.AddProcess(Process.GetCurrentProcess().Id);

            const string IPCServer = "Aetrex.IPCCPP.Server.exe";
            IPCServerProcess = new Process();
            IPCServerProcess.StartInfo.FileName = IPCServer;
            IPCServerProcess.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            IPCServerProcess.Start();

            if (spawnclient)
            {
                var pipeClient =
                    new NamedPipeClientStream(".", "testpipe",
                        PipeDirection.InOut, PipeOptions.None,
                        TokenImpersonationLevel.Impersonation);

                Console.WriteLine("Connecting to server...\n");
                pipeClient.Connect();

                StreamString ss = new StreamString(pipeClient);
                // Validate the server's signature string.
                if (ss.ReadString(64) == "I am the one true server!")
                {
                    // The client security token is sent with the first write.
                    // Send the name of the file whose contents are returned
                    // by the server.
                    ss.WriteStringFixed("Message from client");

                    // Print the file to the screen.
                    Console.WriteLine(ss.ReadString(64));
                }
                else
                {
                    Console.WriteLine("Server could not be verified.");
                }
                pipeClient.Close();
                // Give the client process some time to display results before exiting.
                Thread.Sleep(4000);
                /*
                Console.WriteLine("Pipe Server closing");
                IPCServerProcess.Close();
                Console.WriteLine("Pipe Server closed");

                Console.WriteLine("Pipe Server process disposing");
                IPCServerProcess.Dispose();
                Console.WriteLine("Pipe Server process disposed");

                IPCServerProcess = null;
                */
            }            
        }        
    }    
}