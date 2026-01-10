public interface ICommunicationService
{
    Task<byte[]?> SendMessageAsync(string message, CancellationToken cancellationToken = default);
}