using System;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Security.Principal;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Aetrex.IPC.cs
{
    internal class IPCClient
    {
        private Process IPCServerProcess = null;
        private CancellationTokenSource tokenSource;
        private NamedPipeClientStream pipeClient;

        public IPCClient()
        {
            tokenSource = new CancellationTokenSource();

            //Create a Windows job object, register this process to the job so on job termination, this process and its child processes will terminate by Windows OS
            JobManagement.Job job = new JobManagement.Job();
            job.AddProcess(Process.GetCurrentProcess().Id);

            const string IPCServer = "Aetrex.IPCCPP.Server.exe";
            IPCServerProcess = new Process();
            IPCServerProcess.StartInfo.FileName = IPCServer;
            IPCServerProcess.StartInfo.WindowStyle = ProcessWindowStyle.Normal;
            IPCServerProcess.Start();

            pipeClient =
                new NamedPipeClientStream(".", "testpipe",
                    PipeDirection.InOut, PipeOptions.None,
                    TokenImpersonationLevel.Impersonation);

        }

        public void CommunicateWithService()
        {
            var token = tokenSource.Token;

            Console.WriteLine("Connecting to server...\n");
            pipeClient.Connect();

            StreamString ss = new StreamString(pipeClient);

            Task.Run(() =>
            {
                // polling boolean property
                while (!token.IsCancellationRequested)
                {
                    //token.ThrowIfCancellationRequested(); // throw OperationCancelledException if requested

                    //Wait for a message from the service
                    Console.WriteLine(ss.ReadString(64));

                    //Responds to the service
                    ss.WriteStringFixed("Message from client");                    
                }
                // release resources and exit
                pipeClient.Close();
                // Give the client process some time to display results before exiting.
                Thread.Sleep(4000);

            }, token);
        }

        public void Close()
        {
            tokenSource.Cancel();
        }
    }    
}