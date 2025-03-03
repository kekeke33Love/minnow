#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), close_flag_( false ), bytes_pushed_( 0 ), bytes_popped_( 0 ) {}

void Writer::push( string data )
{
  if( has_error() || is_closed() )
  {
    return;
  }

  uint64_t len = min( data.length(), available_capacity() );
  buf_.append( data, 0, len);
  bytes_pushed_ += len;

}

void Writer::close()
{
  close_flag_ = true;
}

bool Writer::is_closed() const
{
  return close_flag_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - ( bytes_pushed_ - bytes_popped_ );
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  return this->buf_;
  // return string_view( buf_ ).substr( 0, 1 );
}

void Reader::pop( uint64_t len )
{
  if(bytes_buffered() == 0)
  {
    return;
  }
  len = min( len, bytes_buffered() );
  buf_.erase( 0, len);
  bytes_popped_ += len;
}

bool Reader::is_finished() const
{
  if(close_flag_ && bytes_buffered() == 0 )
  {
    return true;
  }
  return false;
}

uint64_t Reader::bytes_buffered() const
{
  return bytes_pushed_ - bytes_popped_;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}
