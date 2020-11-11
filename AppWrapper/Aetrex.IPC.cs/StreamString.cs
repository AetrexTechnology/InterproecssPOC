using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Aetrex.IPC.cs
{
    // Defines the data protocol for reading and writing strings on our stream.
    internal class StreamString
    {
        private Stream ioStream;
        private UnicodeEncoding streamEncoding;

        public StreamString(Stream ioStream)
        {
            this.ioStream = ioStream;
            streamEncoding = new UnicodeEncoding();
        }

        public string ReadString()
        {
            int len;
            len = ioStream.ReadByte() * 256;
            len += ioStream.ReadByte();
            var inBuffer = new byte[len];
            ioStream.Read(inBuffer, 0, len);

            string serverMessage = streamEncoding.GetString(inBuffer);
            Console.WriteLine($"Server message: {serverMessage}");
            return serverMessage;
        }

        public string ReadString(int len)
        {
            var inBuffer = new byte[len];
            ioStream.Read(inBuffer, 0, len);

            string serverMessage = streamEncoding.GetString(inBuffer).Trim('\0');
            //Console.WriteLine($"Server message: {serverMessage}");
            return serverMessage;
        }

        public async Task<string> ReadStringAsync(int len)
        {
            var inBuffer = new byte[len];
            await ioStream.ReadAsync(inBuffer, 0, len);

            string serverMessage = streamEncoding.GetString(inBuffer).Trim('\0');
            //Console.WriteLine($"Server message: {serverMessage}");
            return serverMessage;
        }

        public int WriteString(string outString)
        {
            byte[] outBuffer = streamEncoding.GetBytes(outString);
            int len = outBuffer.Length;
            if (len > UInt16.MaxValue)
            {
                len = (int)UInt16.MaxValue;
            }
            ioStream.WriteByte((byte)(len / 256));
            ioStream.WriteByte((byte)(len & 255));
            ioStream.Write(outBuffer, 0, len);
            ioStream.Flush();

            return outBuffer.Length + 2;
        }

        public int WriteStringFixed(string outString)
        {
            byte[] outBuffer = streamEncoding.GetBytes(outString+'\0');
            int len = Math.Min(outBuffer.Length, UInt16.MaxValue);
            ioStream.Write(outBuffer, 0, len);            
            ioStream.Flush();

            return outBuffer.Length;
        }

        public async Task<int> WriteStringFixedAsync(string outString)
        {
            byte[] outBuffer = streamEncoding.GetBytes(outString + '\0');
            int len = Math.Min(outBuffer.Length, UInt16.MaxValue);
            await ioStream.WriteAsync(outBuffer, 0, len);
            ioStream.Flush();

            return outBuffer.Length;
        }
    }

    // Contains the method executed in the context of the impersonated user
    internal class ReadFileToStream
    {
        private string fn;
        private StreamString ss;

        public ReadFileToStream(StreamString str, string filename)
        {
            fn = filename;
            ss = str;
        }

        public void Start()
        {
            string contents = File.ReadAllText(fn);
            ss.WriteString(contents);
        }
    }

}
