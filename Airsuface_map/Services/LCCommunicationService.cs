using System;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Airsuface_map.Services
{
    public class LCCommunicationService : ICommunicationService, IDisposable
    {
        private readonly string _host;
        private readonly int _port;
        private TcpClient? _client;
        private NetworkStream? _stream;
        private CancellationTokenSource? _cts;
        private Task? _loopTask;

        public LCCommunicationService(string host, int port)
        {
            _host = host;
            _port = port;
        }

        private async Task EnsureConnectedAsync(CancellationToken cancellationToken = default)
        {
            if (_client == null || !_client.Connected)
            {
                _client?.Dispose();
                _client = new TcpClient();
                await _client.ConnectAsync(_host, _port, cancellationToken);
                _stream = _client.GetStream();
            }
        }

        public async Task<byte[]?> SendMessageAsync(string message, CancellationToken cancellationToken = default)
        {
            await EnsureConnectedAsync(cancellationToken);

            if (_stream == null)
                throw new InvalidOperationException("Network stream is not available.");

            var data = Encoding.UTF8.GetBytes(message + "\n");
            await _stream.WriteAsync(data, 0, data.Length, cancellationToken);

            var buffer = new byte[4096];
            int bytesRead = await _stream.ReadAsync(buffer, 0, buffer.Length, cancellationToken);
            if (bytesRead > 0)
            {
                var result = new byte[bytesRead];
                Array.Copy(buffer, result, bytesRead);
                return result;
            }
            return null;
        }

        public void StartPeriodicStatusRequest(Action<byte[]?>? onResponse = null)
        {
            _cts = new CancellationTokenSource();
            _loopTask = Task.Run(async () =>
            {
                try
                {
                    await EnsureConnectedAsync(_cts.Token);

                    while (!_cts.IsCancellationRequested)
                    {
                        try
                        {
                            var response = await SendMessageAsync("getStatus", _cts.Token);
                            onResponse?.Invoke(response);
                        }
                        catch (Exception ex)
                        {
                            // 예외 처리 필요시 여기에
                        }
                        await Task.Delay(100, _cts.Token); // 0.1초 간격
                    }
                }
                catch (Exception ex)
                {
                    // 연결 실패 등 예외 처리
                }
            }, _cts.Token);
        }

        public void StopPeriodicStatusRequest()
        {
            _cts?.Cancel();
            _loopTask?.Wait();
        }

        public void Dispose()
        {
            StopPeriodicStatusRequest();
            _stream?.Dispose();
            _client?.Dispose();
            _cts?.Dispose();
        }
    }
}