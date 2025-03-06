#include "reassembler.hh"
#include "debug.hh"



using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if(data.length() == 0 && is_last_substring)
  {
    output_.writer().push(data);
    output_.writer().close();
    // waiting_assembler.
    return; 
  }
  if(first_unassembler_ < first_index + data.length())
  {  
    if(first_unassembler_ >= first_index)
    {
      if(called_flag_)
      {
        waiting_assembler.erase(waiting_assembler.begin());
      }
      output_.writer().push(data.substr(first_unassembler_));
      
      if(is_last_substring)
      {
        output_.writer().close();
        // waiting_assembler.
        return; 
      }
      first_unassembler_ = first_index + data.length();

      if(!waiting_assembler.empty())
      {
        called_flag_ = true;
        Reassembler::insert(waiting_assembler.begin()->first, waiting_assembler.begin()->second, false);
      }
    }
    // save
    else if(!is_last_substring && first_index - first_unassembler_ <= output_.writer().available_capacity()/2)
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
  else
  {
      return;
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
