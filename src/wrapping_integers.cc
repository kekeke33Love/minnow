#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

const uint64_t POWER_OF_TWO_32 = static_cast<uint64_t>(UINT32_MAX) + 1;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // debug( "unimplemented wrap( {}, {} ) called", n, zero_point.raw_value_ );
  return Wrap32 {static_cast<uint32_t>(n) + zero_point.raw_value_};
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // debug( "unimplemented unwrap( {}, {} ) called", zero_point.raw_value_, checkpoint );
  uint64_t offset = raw_value_ - Wrap32::wrap(checkpoint, zero_point).raw_value_;
  if(offset <= (POWER_OF_TWO_32 >> 1) || checkpoint + offset < POWER_OF_TWO_32)
  {
    return checkpoint + offset;
  }
  return checkpoint + offset - POWER_OF_TWO_32;
}
