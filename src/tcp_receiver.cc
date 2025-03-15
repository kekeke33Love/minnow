#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if(message.RST)
  {
    reassembler_.reader().set_error();
    return;
  }
  if(message.SYN)
  {
    isn_received = true;
    ISN_Message_.seqno = message.seqno;
  }
  if(isn_received)
  {
    uint64_t stream_index = message.seqno.unwrap(ISN_Message_.seqno, reassembler_.get_first_unassembler()) - (message.SYN ? 0 : 1);
    reassembler_.insert(stream_index, message.payload, message.FIN);
  }
  
  debug( "unimplemented receive() called" );
}

TCPReceiverMessage TCPReceiver::send() const
{ 
  TCPReceiverMessage message;
  if(reassembler_.writer().has_error())
  {
  message.RST = true;
  }

  if(isn_received)
  {
    uint64_t ackno = reassembler_.get_first_unassembler() + 1;
    if(reassembler_.writer().is_closed())
    {
    message.ackno = Wrap32::wrap(ackno + 1, ISN_Message_.seqno);
    }
    else
    {
      message.ackno = Wrap32::wrap(ackno, ISN_Message_.seqno);
    }
  }

  message.window_size = min(reassembler_.writer().available_capacity(), static_cast<uint64_t>UINT16_MAX);
  
  debug( "unimplemented send() called" );
  return message;
}
