﻿using System;
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
        private CancellationToken token;

        public IPCClient()
        {
            tokenSource = new CancellationTokenSource();
            token = tokenSource.Token;

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
            Console.WriteLine("Connecting to server...\n");
            pipeClient.Connect();
            Console.WriteLine("Connected to server...\n");

            Task server = FetchMessageFromServer();

            Task client = PostMessageToService();

            //Task.Run(() =>
            //{
            //    Task server = FetchMessageFromServer();

            //    Task client = PostMessageToService();

            //    Task.WaitAll(server, client);

            //    // release resources and exit
            //    pipeClient.Close();
            //});

            /*
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
            */
        }

        private Task FetchMessageFromServer()
        {
            StreamString ss = new StreamString(pipeClient);

            return Task.Run(async () =>
            {
                // polling boolean property
                while (!token.IsCancellationRequested)
                {
                    Console.WriteLine("Fetching a message from server");
                    //Wait for a message from the service
                    Console.WriteLine(await ss.ReadStringAsync(64));
                    Console.WriteLine("Message from server arrived");
                }
                // Give the client process some time to display results before exiting.
                Thread.Sleep(4000);

            }, token);
        }

        private Task PostMessageToService()
        {
            StreamString ss = new StreamString(pipeClient);
            Random random = new Random();

            return Task.Run(async () =>
            {
                int iMessageIndex = 1;
                // polling boolean property
                while (!token.IsCancellationRequested)
                {
                    //Responds to the service                    
                    Console.WriteLine("Posting a message to server");
                    await ss.WriteStringFixedAsync($"Message from client #{iMessageIndex}");
                    iMessageIndex++;
                    Console.WriteLine("Message to server posted");

                    // Give the client process some time to display results before exiting.
                    int randomWait = random.Next(1, 4) * 1000;
                    Thread.Sleep(2000);
                }
            }, token);
        }
        public void Close()
        {
            tokenSource.Cancel();
        }
    }    
}