using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;

namespace Aetrex.IPC.cs
{
    class Program
    {
        static void Main(string[] args)
        {
            //Create a Windows job object, register this process to the job so on job termination, this process and its child processes will terminate by Windows OS
            JobManagement.Job job = new JobManagement.Job();
            job.AddProcess(Process.GetCurrentProcess().Id);

            const string IPCServer = @"D:\Aetrex\Interprocess\AppWrapper\x64\Debug\AppWrapper.exe";
            Process IPCServerProcess = IPCServerProcess = new Process();
            IPCServerProcess.StartInfo.FileName = IPCServer;
            IPCServerProcess.StartInfo.WindowStyle = ProcessWindowStyle.Normal;
            IPCServerProcess.Start();

            Console.WriteLine($"Program.Main() threadId:{Task.CurrentId.ToString()}");

            //VAClient to scanner communicates over a pipe where C# is the pipe server and VAClient is the pipe client
            const string pipeNameVoiceToScanner = "AetrexScannerOS2VoiceActivation";
            VoiceServiceToScanner voiceToScanner = new VoiceServiceToScanner(pipeNameVoiceToScanner);

            //Scanner to VAClient communicates over a pipe where C# is the pipe client and VAClient is the pipe service
            ScannerToVoiceService scannerToVoice = null;
            const string pipeNameScannerToVoice = "eNovaClient";
            scannerToVoice = new ScannerToVoiceService(pipeNameScannerToVoice);            

            Console.WriteLine("Press 1 to send a message to the voice service");
            Console.WriteLine("Press Enter to exit");
            var keyPressed = Console.ReadKey();
            do
            {                
                if (keyPressed.Key == ConsoleKey.D1)
                {
                    if (scannerToVoice != null)
                    {
                        scannerToVoice.SetMicrophoneIndex(0);
                    }
                }
                if (keyPressed.Key != ConsoleKey.Enter)
                {
                    keyPressed = Console.ReadKey();
                }
            }
            while (keyPressed.Key != ConsoleKey.Enter);
            
            if (voiceToScanner != null)
            {
                Console.WriteLine("Closing the connection from voice process to scanner");
                voiceToScanner.Close();
            }

            if (scannerToVoice != null)
            {
                Console.WriteLine("Closing the connection from the scanner to the voice process");
                scannerToVoice.Close();
            }            
        }

    }
}
