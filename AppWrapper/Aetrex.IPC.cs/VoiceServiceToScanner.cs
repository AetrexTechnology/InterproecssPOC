using Newtonsoft.Json;
using System;
using System.IO;
using System.IO.Pipes;
using System.Threading;
using System.Threading.Tasks;

namespace Aetrex.IPC.cs
{
    internal class VoiceServiceToScanner
    {
        private string pipeName;
        private bool terminate;
        private string command;
        AutoResetEvent commandReadyToTransmit = new AutoResetEvent(false);

        public VoiceServiceToScanner(string pipeName)
        {
            terminate = false;            
            this.pipeName = pipeName;
            
            Thread server = null;
            server = new Thread(CommunicationThread);
            server.Start();           
        }

        public void Close()
        {
            this.terminate = true;
        }

        public void SetMicrophoneIndex(int micIndex)
        {
            //std::string tmp = "{\"time\": \"" + dateTime + "\", \"instruction\" : \"" + instruction + "\", \"microphoneIndex\" : " + std::to_string(microphoneIndex) + "}";
            var commandObj = new
            {
                time = DateTime.UtcNow.ToString("s", System.Globalization.CultureInfo.InvariantCulture), //Time in ISO 8601
                instruction = "SetAudioCaptureDeviceIndex",
                microphoneIndex = micIndex
            };

            command = JsonConvert.SerializeObject(commandObj);
            commandReadyToTransmit.Set();
        }
        private void CommunicationThread(object data)
        {
            NamedPipeServerStream pipeServer = new NamedPipeServerStream(this.pipeName, PipeDirection.InOut, 1, PipeTransmissionMode.Message, PipeOptions.Asynchronous);

            int threadId = Thread.CurrentThread.ManagedThreadId;
            
            Console.WriteLine($"Server waiting for connection threadId:{Task.CurrentId.ToString()}");

            // Wait for a client to connect
            pipeServer.WaitForConnection();

            Task.Run(async () => {
                Console.WriteLine($"ScannerToVoiceProcess.CommunicationThread() threadId:{Task.CurrentId.ToString()}");
                while (!terminate)
                {
                    try
                    {
                        // commandReadyToTransmit.WaitOne();
                        
                        StreamString ss = new StreamString(pipeServer);

                        Console.WriteLine($"Waiting for a message from voice service");
                        string messageFromVoice = await ss.ReadStringAsync(1000);
                        Console.WriteLine(messageFromVoice);

                        //Console.WriteLine($"About to send a message to voice service");
                        //await ss.WriteStringFixedAsync(command);
                        //Console.WriteLine($"Sent message to voice service");
                    }
                    // Catch the IOException that is raised if the pipe is broken
                    // or disconnected.
                    catch (IOException e)
                    {
                        Console.WriteLine("ERROR: {0}", e.Message);
                    }

                    Thread.Sleep(10000);
                }

                Console.WriteLine("Server closing");
                pipeServer.Close();
                Console.WriteLine("Server closed");

            });
        }
    }
}
