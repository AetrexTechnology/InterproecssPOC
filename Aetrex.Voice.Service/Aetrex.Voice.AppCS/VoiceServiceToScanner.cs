using Newtonsoft.Json;
using System;
using System.IO;
using System.IO.Pipes;
using System.Threading;
using System.Threading.Tasks;

namespace Aetrex.Voice.AppCS
{
    internal class VoiceServiceToScanner: IPCBase
    {
        
        AutoResetEvent commandReadyToTransmit = new AutoResetEvent(false);

        public VoiceServiceToScanner(string pipeName): base(pipeName)
        {                        
            
        }

        protected override void CommunicationThread()
        {
            NamedPipeServerStream pipeServer = null;

            Task.Run(async () => {

                while (!cancellationToken.IsCancellationRequested)
                {
                    try
                    {
                        pipeServer = new NamedPipeServerStream(
                            this.pipeName,
                            PipeDirection.InOut,
                            1,
                            PipeTransmissionMode.Message,
                            PipeOptions.Asynchronous);

                        // Wait for a client to connect
                        Console.WriteLine($"VoiceServiceToScanner.CommunicationThread() waiting for connection");
                        pipeServer.WaitForConnection();
                        Console.WriteLine($"VoiceServiceToScanner.CommunicationThread() Voice service connected ");

                        Console.WriteLine($"Waiting for a message from voice service");
                        StreamString ss = new StreamString(pipeServer);
                        string messageFromVoice = await ss.ReadStringAsync(1000, cancellationToken);
                        if (!String.IsNullOrWhiteSpace(messageFromVoice))
                        {
                            Console.WriteLine(messageFromVoice);
                        }
                        else
                        {
                            Console.WriteLine("VoiceServiceToScanner.CommunicationThread() No message from voice service");
                        }

                        //The voice service closed the current pipe. When it has another command, it will try to open a new pipe.
                        //For now, close the current connection and create a new one
                        pipeServer.Close();
                        pipeServer = null;
                    }
                    // Catch the IOException that is raised if the pipe is broken
                    // or disconnected.
                    catch (IOException e)
                    {
                        Console.WriteLine("ERROR: {0}", e.Message);
                    }
                    
                }

                if (pipeServer != null)
                {
                    Console.WriteLine("VoiceServiceToScanner closing the pipe");
                    pipeServer.Close();
                    Console.WriteLine("VoiceServiceToScanner pipe is closed");
                }
            }, cancellationToken);
        }
    }
}
