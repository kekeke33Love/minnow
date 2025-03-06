#include "reassembler.hh"
#include "debug.hh"



using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if(first_unassembler_ < first_index + data.length())
  {  
    if(first_unassembler_ >= first_index) // 可以立即使用
    {
      if(called_flag_)
      {
        waiting_assembler.erase(waiting_assembler.begin());
        called_flag_ = false;
      }

      uint64_t len = min(data.substr(first_unassembler_ - first_index).length(), output_.writer().available_capacity());
      output_.writer().push(data.substr(first_unassembler_ - first_index, len)); // push 容量待优化
      
      if(len == output_.writer().available_capacity())
      {
        return;
      }
      
      if(is_last_substring)
      {
        output_.writer().close();
        return; 
      }

      first_unassembler_ += data.length();

      if(!waiting_assembler.empty())
      {
        called_flag_ = true;
        Reassembler::insert(waiting_assembler.begin()->first, waiting_assembler.begin()->second, false);
      }
    }
    // else if(!is_last_substring && first_index - first_unassembler_ <= output_.writer().available_capacity()/2)  // 不能立即使用，保存map
    else if(!is_last_substring)
    {
      if(called_flag_)
      {
        return;
      }
      else
      {
        waiting_assembler.emplace(first_index, data);
      }
    }
  }
  else if(data.length() == 0 && first_index == first_unassembler_) 
  {
    if(is_last_substring)
    {
      output_.writer().close();
      return;
    }
  }

  if(called_flag_)
  {
    waiting_assembler.erase(waiting_assembler.begin());
    called_flag_ = false;
    if(!waiting_assembler.empty())
    {
      called_flag_ = true;
      Reassembler::insert(waiting_assembler.begin()->first, waiting_assembler.begin()->second, false);
    }
  }

  debug( "unimplemented insert({}, {}, {}) called", first_index, data, is_last_substring );
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  debug( "unimplemented count_bytes_pending() called" );
  return {};
}
