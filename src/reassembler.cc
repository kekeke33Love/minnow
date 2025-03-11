#include "reassembler.hh"
#include "debug.hh"



using namespace std;

uint64_t merge_substrings(map<uint64_t, string>& substrings, uint64_t unassembled_bytes_);

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if(is_last_substring)
  {
    eof_index = first_index + data.length();
  }

  if(first_unassembler_ == eof_index && data.length() == 0)
  {
    output_.writer().close();
  }

  if(first_index + data.length() <= first_unassembler_ || first_index >= first_unassembler_ + output_.writer().available_capacity())
  {
    return;
  }

  if(first_index < first_unassembler_)
  {
    data = data.substr(first_unassembler_ - first_index);
    first_index = first_unassembler_;
  }
  if(first_index + data.length() > first_unassembler_ + output_.writer().available_capacity())
  {
    data = data.erase(first_unassembler_ + output_.writer().available_capacity() - first_index);
  }
  if(waiting_assembler.count(first_index) == 1)
  {
    if(data.length() > waiting_assembler.find(first_index)->second.length())
    {
      auto it = waiting_assembler.find(first_index);
      unassembled_bytes_ -= it->second.length();
      waiting_assembler.erase(it);
    }
    else if(data.length() == waiting_assembler.find(first_index)->second.length())
    {
      return;
    }
  }
  unassembled_bytes_ += data.length();
  waiting_assembler.emplace(first_index, data);
  unassembled_bytes_ = merge_substrings(waiting_assembler, unassembled_bytes_);

  if(!waiting_assembler.empty())
  {
    auto it = waiting_assembler.begin();
    if(it->first == first_unassembler_)
    {
      first_unassembler_ += it->second.length();
      output_.writer().push(it->second);
      unassembled_bytes_ -= it->second.length();
      waiting_assembler.erase(waiting_assembler.begin());
    }
  }

  if(first_unassembler_ == eof_index)
  {
    output_.writer().close();
  }

  // if(first_unassembler_ < first_index + data.length())
  // { 
  //   // 可以直接使用并且还有缓存
  //   if(output_.writer().available_capacity() > 0 && first_unassembler_ >= first_index)
  //   {  
  //     uint64_t index = first_unassembler_ - first_index;
  //     uint64_t len =  min(data.substr(index).length(), output_.writer().available_capacity());
  //     output_.writer().push(data.substr(index, len));
  //     first_unassembler_ += len;

  //     while(!waiting_assembler.empty())
  //     {
  //       first_index = waiting_assembler.begin()->first;
  //       data = waiting_assembler.begin()->second;
  //       if(first_index + data.length() <= first_unassembler_)
  //       {
  //         unassembled_bytes_ -= data.length();
  //         waiting_assembler.erase(waiting_assembler.begin());
  //       }
  //       else if(first_index <= first_unassembler_)
  //       {
  //         data = data.substr(first_unassembler_ - first_index);
  //         output_.writer().push(data);
  //         first_unassembler_ += data.length();
  //         unassembled_bytes_ -= waiting_assembler.begin()->second.length();
  //         waiting_assembler.erase(waiting_assembler.begin());
          
  //       }
  //     }
  //   }
  //   // save
  //   if(first_index > first_unassembler_ && first_index <= first_unassembler_ + output_.writer().available_capacity())
  //   {
  //     if(first_index + data.length() > first_unassembler_ + output_.writer().available_capacity())
  //     {
  //       data = data.erase(first_unassembler_ + output_.writer().available_capacity() - first_index);
  //     }

  //     if(waiting_assembler.count(first_index) == 1)
  //     {
  //       auto it = waiting_assembler.find(first_index);
  //       unassembled_bytes_ -= it->second.length();
  //       waiting_assembler.erase(it);

  //     }
  //     unassembled_bytes_ += data.length();
  //     waiting_assembler.emplace(first_index, data);
      
  //     unassembled_bytes_ = merge_substrings(waiting_assembler, unassembled_bytes_);


      
  //   }
  // }

  // if(first_unassembler_ == eof_index)
  // {
  //   output_.writer().close();
  // }

  debug( "unimplemented insert({}, {}, {}) called", first_index, data, is_last_substring );
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  debug( "unimplemented count_bytes_pending() called" );
  return unassembled_bytes_;
}

uint64_t merge_substrings(map<uint64_t, string>& substrings, uint64_t unassembled_bytes_) {
  auto it = substrings.begin();
  while (it != substrings.end()) {
      auto next_it = it;
      ++next_it;

      // 如果下一个 substring 与当前 substring 相邻或重叠
      if (next_it != substrings.end() && it->first + it->second.length() >= next_it->first) {
          // 计算合并后的 substring 的起始位置和结束位置
          uint64_t new_start = it->first;

          // 构造合并后的 substring
          string new_data = it->second;
          if (next_it->first + next_it->second.length() > it->first + it->second.length()) {
              // 追加下一个 substring 的额外数据
              uint64_t overlap = it->first + it->second.length() - next_it->first;
              new_data += next_it->second.substr(overlap);
          }

          // 删除原来的两个 substrings，插入合并后的 substring
          unassembled_bytes_ -= it->second.length();
          unassembled_bytes_ -= next_it->second.length();

          substrings.erase(it);
          substrings.erase(next_it);
          substrings.emplace(new_start, new_data);
          unassembled_bytes_ += new_data.length();

          // 重新从合并后的位置开始检查
          it = substrings.find(new_start);
      } else {
          ++it;
      }
  }

  return unassembled_bytes_;
}