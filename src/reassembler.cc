#include "reassembler.hh"
#include "debug.hh"



using namespace std;

uint64_t merge_substrings(map<size_t, string>& substrings, uint64_t unassembled_bytes_);

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

      if (!waiting_assembler.empty())
      {
        first_index = waiting_assembler.begin()->first;
        data = waiting_assembler.begin()->second;
        if(first_index + data.length() <= first_unassembler_)
        {
          unassembled_bytes_ -= data.length();
          waiting_assembler.erase(waiting_assembler.begin());
        }
        else if(first_index <= first_unassembler_)
        {
          data = data.substr(first_unassembler_ - first_index);
          output_.writer().push(data);
          first_unassembler_ += data.length();
          unassembled_bytes_ -= waiting_assembler.begin()->second.length();
          waiting_assembler.erase(waiting_assembler.begin());
          
        }
      }
    }
    // save
    // if(first_index > first_unassembler_ && first_index <= first_unassembler_ + output_.writer().available_capacity())
    // {   
    //   uint64_t count = 0;
    //   uint64_t index = first_index;
    //   const uint64_t len = (first_index + data.length() < first_unassembler_ + output_.writer().available_capacity()) ? first_index + data.length() : first_unassembler_ + output_.writer().available_capacity();
    //   for(;index < len;)
    //   {
    //     if(waiting_assembler.count(index) == 0)
    //     {
    //       waiting_assembler.emplace(index, data.substr(count, 1));
    //       unassembled_bytes_++;
    //     }
    //     index++;
    //     count++;
    //   }  
    // }

    

    if(first_index > first_unassembler_ && first_index <= first_unassembler_ + output_.writer().available_capacity())
    {
      if(first_index + data.length() > first_unassembler_ + output_.writer().available_capacity())
      {
        data = data.erase(first_unassembler_ + output_.writer().available_capacity() - first_index);
      }

      unassembled_bytes_ += data.length();
      waiting_assembler.emplace(first_index, data);
      unassembled_bytes_ = merge_substrings(waiting_assembler, unassembled_bytes_);


      // if(waiting_assembler.empty())
      // {
      //   unassembled_bytes_ += data.length();
      //   waiting_assembler.emplace(first_index, data);
      // }
      // else
      // {
      //   if(first_index > waiting_assembler.begin()->first)
      //   {
      //     if(first_index + data.length() > waiting_assembler.begin()->first + waiting_assembler.begin()->second.length())
      //     {
      //       unassembled_bytes_ -= waiting_assembler.begin()->second.length();
      //       waiting_assembler.begin()->second = waiting_assembler.begin()->second.erase(first_index - waiting_assembler.begin()->first).append(data);
      //       unassembled_bytes_ += waiting_assembler.begin()->second.length();
      //     }
      //   }
        
      //   if(first_index < waiting_assembler.begin()->first)
      //   {
      //     waiting_assembler.
      //   }
      // }
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

uint64_t merge_substrings(map<size_t, string>& substrings, uint64_t unassembled_bytes_) {
  auto it = substrings.begin();
  while (it != substrings.end()) {
      auto next_it = it;
      ++next_it;

      // 如果下一个 substring 与当前 substring 相邻或重叠
      if (next_it != substrings.end() && it->first + it->second.length() >= next_it->first) {
          // 计算合并后的 substring 的起始位置和内容
          size_t new_start = it->first;
          // size_t new_end = max(it->first + it->second.length(), next_it->first + next_it->second.length());
          string new_data = it->second;

          // 如果下一个 substring 有额外的数据，则追加到当前 substring
          if (next_it->first + next_it->second.length() > it->first + it->second.length()) {
              size_t overlap = it->first + it->second.length() - next_it->first;
              new_data += next_it->second.substr(overlap);
          }

          // 删除原来的两个 substrings，插入合并后的 substring
          unassembled_bytes_ -= it->second.length();
          substrings.erase(it);
          unassembled_bytes_ -= next_it->second.length();
          substrings.erase(next_it);
          substrings.emplace(new_start, new_data);


          // 重新从合并后的位置开始检查
          it = substrings.find(new_start);
          unassembled_bytes_ += it->second.length();
      } else {
          ++it;
      }
  }

  return unassembled_bytes_;
}
