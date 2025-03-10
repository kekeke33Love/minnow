#include "reassembler.hh"
#include "debug.hh"



using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if(is_last_substring)
  {
    eof_index = first_index + data.length();
  }

  if(first_unassembler_ < first_index + data.length())
  { 
    // 可以直接使用并且还有缓存
    if(output_.writer().available_capacity() > 0 && first_unassembler_ >= first_index)
    {  
      uint64_t index = first_unassembler_ - first_index;
      uint64_t len =  min(data.substr(index).length(), output_.writer().available_capacity());
      output_.writer().push(data.substr(index, len));
      first_unassembler_ += len;

      while (!waiting_assembler.empty())
      {
        if(waiting_assembler.begin()->first < first_unassembler_)
        {
          waiting_assembler.erase(waiting_assembler.begin());
          unassembled_bytes_--;
        }
        else if(waiting_assembler.begin()->first == first_unassembler_)
        {
          output_.writer().push(waiting_assembler.begin()->second);
          first_unassembler_++;
          waiting_assembler.erase(waiting_assembler.begin());
          unassembled_bytes_--;
        }else
        {
          break;
        }
      }
    }
    // save
    if(first_index > first_unassembler_ && first_index <= first_unassembler_ + output_.writer().available_capacity())
    {   
      uint64_t count = 0;
      uint64_t index = first_index;
      const uint64_t len = (first_index + data.length() < first_unassembler_ + output_.writer().available_capacity()) ? first_index + data.length() : first_unassembler_ + output_.writer().available_capacity();
      for(;index < len;)
      {
        if(waiting_assembler.count(index) == 0)
        {
          waiting_assembler.emplace(index, data.substr(count, 1));
          unassembled_bytes_++;
        }
        index++;
        count++;
      }  
    }
  }

  if(first_unassembler_ == eof_index) 
  {
    output_.writer().close();
  }

  debug( "unimplemented insert({}, {}, {}) called", first_index, data, is_last_substring );
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  debug( "unimplemented count_bytes_pending() called" );
  return unassembled_bytes_;
}
