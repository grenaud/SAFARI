#include <gbwtgraph/minimizer.h>

#include <sstream>
#define RYMER

namespace gbwtgraph
{

//------------------------------------------------------------------------------

// MinimizerHeader: Numerical class constants.

constexpr std::uint32_t MinimizerHeader::TAG;
constexpr std::uint32_t MinimizerHeader::VERSION;
constexpr std::uint64_t MinimizerHeader::FLAG_MASK;
constexpr std::uint64_t MinimizerHeader::FLAG_KEY_MASK;
constexpr size_t MinimizerHeader::FLAG_KEY_OFFSET;
constexpr std::uint64_t MinimizerHeader::FLAG_SYNCMERS;

//------------------------------------------------------------------------------

// Position: Numerical class constants.

constexpr size_t Position::OFFSET_BITS;
constexpr size_t Position::ID_OFFSET;
constexpr code_type Position::REV_MASK;
constexpr code_type Position::OFF_MASK;

//------------------------------------------------------------------------------

// Key64: Numerical class constants.

constexpr std::size_t Key64::KEY_BITS;
constexpr std::size_t Key64::KMER_LENGTH;
constexpr std::size_t Key64::WINDOW_LENGTH;
constexpr std::size_t Key64::SMER_LENGTH;
constexpr std::size_t Key64::KMER_MAX_LENGTH;

constexpr Key64::key_type Key64::EMPTY_KEY;
constexpr Key64::key_type Key64::NO_KEY;
constexpr Key64::key_type Key64::KEY_MASK;
constexpr Key64::key_type Key64::IS_POINTER;

constexpr size_t Key64::PACK_WIDTH;
constexpr Key64::key_type Key64::PACK_MASK;
constexpr size_t Key64::PACK_WIDTH_RYMER;
constexpr Key64::key_type Key64::PACK_MASK_RYMER;

// Key64: Other class variables.

const std::vector<unsigned char> Key64::CHAR_TO_PACK =
{
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,

  4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  3, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  3, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,

  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,

  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4
};


#ifdef RYMER
const std::vector<unsigned char> Key64::CHAR_TO_PACK_RYMER =
{
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 0 - 15
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 16 - 31
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 32 - 47 (includes space, punctuation)
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 48 - 63 (includes digits)
  4, 0, 4, 1,  4, 4, 4, 0,  4, 4, 4, 4,  4, 4, 4, 4,  // 64 - 79 (includes A -> 0, C -> 1, G -> 0)
  4, 4, 4, 4,  1, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 80 - 95 (includes T -> 1)
  4, 0, 4, 1,  4, 4, 4, 0,  4, 4, 4, 4,  4, 4, 4, 4,  // 96 - 111 (includes a -> 0, c -> 1, g -> 0)
  4, 4, 4, 4,  1, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 112 - 127 (includes t -> 1)
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 128 - 143
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 144 - 159
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 160 - 175
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 176 - 191
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 192 - 207
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 208 - 223
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  // 224 - 239
  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4   // 240 - 255
};
#endif

const std::vector<char> Key64::PACK_TO_CHAR = { 'A', 'C', 'G', 'T' };

#ifdef RYMER
const std::vector<char> Key64::PACK_TO_CHAR_RYMER = {'A', 'C'};
#endif

const std::vector<Key64::key_type> Key64::KMER_MASK =
{
  0x0000000000000000ull,
  0x0000000000000003ull,
  0x000000000000000Full,
  0x000000000000003Full,
  0x00000000000000FFull,
  0x00000000000003FFull,
  0x0000000000000FFFull,
  0x0000000000003FFFull,
  0x000000000000FFFFull,
  0x000000000003FFFFull,
  0x00000000000FFFFFull,
  0x00000000003FFFFFull,
  0x0000000000FFFFFFull,
  0x0000000003FFFFFFull,
  0x000000000FFFFFFFull,
  0x000000003FFFFFFFull,
  0x00000000FFFFFFFFull,
  0x00000003FFFFFFFFull,
  0x0000000FFFFFFFFFull,
  0x0000003FFFFFFFFFull,
  0x000000FFFFFFFFFFull,
  0x000003FFFFFFFFFFull,
  0x00000FFFFFFFFFFFull,
  0x00003FFFFFFFFFFFull,
  0x0000FFFFFFFFFFFFull,
  0x0003FFFFFFFFFFFFull,
  0x000FFFFFFFFFFFFFull,
  0x003FFFFFFFFFFFFFull,
  0x00FFFFFFFFFFFFFFull,
  0x03FFFFFFFFFFFFFFull,
  0x0FFFFFFFFFFFFFFFull,
  0x3FFFFFFFFFFFFFFFull
};

const std::vector<Key64::key_type> Key64::RYMER_MASK =
{
  0x0000000000000000ull,
  0x0000000000000001ull,
  0x0000000000000003ull,
  0x0000000000000007ull,
  0x000000000000000Full,
  0x000000000000001Full,
  0x000000000000003Full,
  0x000000000000007Full,
  0x00000000000000FFull,
  0x00000000000001FFull,
  0x00000000000003FFull,
  0x00000000000007FFull,
  0x0000000000000FFFull,
  0x0000000000001FFFull,
  0x0000000000003FFFull,
  0x0000000000007FFFull,
  0x000000000000FFFFull,
  0x000000000001FFFFull,
  0x000000000003FFFFull,
  0x000000000007FFFFull,
  0x00000000000FFFFFull,
  0x00000000001FFFFFull,
  0x00000000003FFFFFull,
  0x00000000007FFFFFull,
  0x0000000000FFFFFFull,
  0x0000000001FFFFFFull,
  0x0000000003FFFFFFull,
  0x0000000007FFFFFFull,
  0x000000000FFFFFFFull,
  0x000000001FFFFFFFull,
  0x000000003FFFFFFFull,
  0x000000007FFFFFFFull
};

//------------------------------------------------------------------------------

// Key128: Numerical class constants.

constexpr std::size_t Key128::FIELD_BITS;

constexpr std::size_t Key128::KEY_BITS;
constexpr std::size_t Key128::KMER_LENGTH;
constexpr std::size_t Key128::WINDOW_LENGTH;
constexpr std::size_t Key128::SMER_LENGTH;
constexpr std::size_t Key128::KMER_MAX_LENGTH;

constexpr Key128::key_type Key128::EMPTY_KEY;
constexpr Key128::key_type Key128::NO_KEY;
constexpr Key128::key_type Key128::KEY_MASK;
constexpr Key128::key_type Key128::IS_POINTER;

constexpr size_t Key128::PACK_WIDTH;
constexpr size_t Key128::PACK_OVERFLOW;
constexpr Key128::key_type Key128::PACK_MASK;

// Key128: Other class variables.

const std::vector<unsigned char> Key128::CHAR_TO_PACK = Key64::CHAR_TO_PACK;

const std::vector<char> Key128::PACK_TO_CHAR = Key64::PACK_TO_CHAR;

const std::vector<Key128::key_type> Key128::HIGH_MASK =
{
  // k = 0
  0x0000000000000000ull,

  // k = 1 to 32 in the low part of the key.
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,
  0x0000000000000000ull,

  // k = 33 to 63 in the high part of the key
  0x0000000000000003ull,
  0x000000000000000Full,
  0x000000000000003Full,
  0x00000000000000FFull,
  0x00000000000003FFull,
  0x0000000000000FFFull,
  0x0000000000003FFFull,
  0x000000000000FFFFull,
  0x000000000003FFFFull,
  0x00000000000FFFFFull,
  0x00000000003FFFFFull,
  0x0000000000FFFFFFull,
  0x0000000003FFFFFFull,
  0x000000000FFFFFFFull,
  0x000000003FFFFFFFull,
  0x00000000FFFFFFFFull,
  0x00000003FFFFFFFFull,
  0x0000000FFFFFFFFFull,
  0x0000003FFFFFFFFFull,
  0x000000FFFFFFFFFFull,
  0x000003FFFFFFFFFFull,
  0x00000FFFFFFFFFFFull,
  0x00003FFFFFFFFFFFull,
  0x0000FFFFFFFFFFFFull,
  0x0003FFFFFFFFFFFFull,
  0x000FFFFFFFFFFFFFull,
  0x003FFFFFFFFFFFFFull,
  0x00FFFFFFFFFFFFFFull,
  0x03FFFFFFFFFFFFFFull,
  0x0FFFFFFFFFFFFFFFull,
  0x3FFFFFFFFFFFFFFFull
};

const std::vector<Key128::key_type> Key128::LOW_MASK =
{
  // k = 0
  0x0000000000000000ull,

  // k = 1 to 32 in the low part of the key.
  0x0000000000000003ull,
  0x000000000000000Full,
  0x000000000000003Full,
  0x00000000000000FFull,
  0x00000000000003FFull,
  0x0000000000000FFFull,
  0x0000000000003FFFull,
  0x000000000000FFFFull,
  0x000000000003FFFFull,
  0x00000000000FFFFFull,
  0x00000000003FFFFFull,
  0x0000000000FFFFFFull,
  0x0000000003FFFFFFull,
  0x000000000FFFFFFFull,
  0x000000003FFFFFFFull,
  0x00000000FFFFFFFFull,
  0x00000003FFFFFFFFull,
  0x0000000FFFFFFFFFull,
  0x0000003FFFFFFFFFull,
  0x000000FFFFFFFFFFull,
  0x000003FFFFFFFFFFull,
  0x00000FFFFFFFFFFFull,
  0x00003FFFFFFFFFFFull,
  0x0000FFFFFFFFFFFFull,
  0x0003FFFFFFFFFFFFull,
  0x000FFFFFFFFFFFFFull,
  0x003FFFFFFFFFFFFFull,
  0x00FFFFFFFFFFFFFFull,
  0x03FFFFFFFFFFFFFFull,
  0x0FFFFFFFFFFFFFFFull,
  0x3FFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,

  // k = 33 to 63 in the high part of the key
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull,
  0xFFFFFFFFFFFFFFFFull
};

//------------------------------------------------------------------------------

MinimizerHeader::MinimizerHeader() :
  tag(TAG), version(VERSION),
  k(0), w(0),
  keys(0), capacity(0), max_keys(0),
  values(0), unique(0),
  flags(0)
{
}

MinimizerHeader::MinimizerHeader(size_t kmer_length, size_t window_length, size_t initial_capacity, double max_load_factor, size_t key_bits) :
  tag(TAG), version(VERSION),
  k(kmer_length), w(window_length),
  keys(0), capacity(initial_capacity), max_keys(initial_capacity * max_load_factor),
  values(0), unique(0),
  flags(0)
{
  this->set_int(FLAG_KEY_MASK, FLAG_KEY_OFFSET, key_bits);
}

void
MinimizerHeader::sanitize(size_t kmer_max_length)
{
  if(this->k > kmer_max_length)
  {
    std::cerr << "MinimizerHeader::sanitize(): Adjusting k from " << this->k << " to " << kmer_max_length << std::endl;
    this->k = kmer_max_length;
  }
  if(this->k == 0)
  {
    std::cerr << "MinimizerHeader::sanitize(): Adjusting k from " << this->k << " to " << 2 << std::endl;
    this->k = 2;
  }

  if(this->get_flag(FLAG_SYNCMERS))
  {
    if(this->w == 0)
    {
      std::cerr << "MinimizerHeader::sanitize(): Adjusting s from " << this->w << " to " << 1 << std::endl;
      this->w = 1;
    }
    if(this->w >= this->k)
    {
      std::cerr << "MinimizerHeader::sanitize(): Adjusting s from " << this->w << " to " << (this->k - 1) << std::endl;
      this->w = this->k - 1;
    }
  }
  else
  {
    if(this->w == 0)
    {
      std::cerr << "MinimizerHeader::sanitize(): Adjusting w from " << this->w << " to " << 1 << std::endl;
      this->w = 1;
    }
  }
}

void
MinimizerHeader::check() const
{
  if(this->tag != TAG)
  {
    throw sdsl::simple_sds::InvalidData("MinimizerHeader: Invalid tag");
  }

  if(this->version != VERSION)
  {
    std::string msg = "MinimizerHeader: Expected v" + std::to_string(VERSION) + ", got v" + std::to_string(this->version);
    throw sdsl::simple_sds::InvalidData(msg);
  }

  std::uint64_t mask = FLAG_MASK;
  if((this->flags & mask) != this->flags)
  {
    throw sdsl::simple_sds::InvalidData("MinimizerHeader: Invalid flags");
  }
}

void
MinimizerHeader::update_version(size_t key_bits)
{
  this->version = VERSION;
  this->set_int(FLAG_KEY_MASK, FLAG_KEY_OFFSET, key_bits);
}

void
MinimizerHeader::set_int(std::uint64_t mask, size_t offset, size_t value)
{
  this->unset(mask);
  this->set((value << offset) & mask);
}

size_t
MinimizerHeader::get_int(std::uint64_t mask, size_t offset) const
{
  return (this->flags & mask) >> offset;
}

size_t
MinimizerHeader::key_bits() const
{
  return this->get_int(FLAG_KEY_MASK, FLAG_KEY_OFFSET);
}

bool
MinimizerHeader::operator==(const MinimizerHeader& another) const
{
  return (this->tag == another.tag && this->version == another.version &&
          this->k == another.k && this->w == another.w &&
          this->keys == another.keys && this->capacity == another.capacity && this->max_keys == another.max_keys &&
          this->values == another.values && this->unique == another.unique &&
          this->flags == another.flags);
}

//------------------------------------------------------------------------------

Key64
Key64::encode(const std::string& sequence)
{
  key_type packed = 0;
  for(auto c : sequence)
  {
    auto packed_char = CHAR_TO_PACK[c];
    if(packed_char > PACK_MASK)
    {
      throw std::runtime_error("[ENCODE] Key64::encode(): Cannot encode character '" + std::to_string(c) + "'");
    }
    packed = (packed << PACK_WIDTH) | packed_char;
  }
  return Key64(packed);
}

std::string
Key64::decode(size_t k) const
{
  std::stringstream result;
  for(size_t i = 0; i < k; i++)
  {
    result << PACK_TO_CHAR[(this->key >> ((k - i - 1) * PACK_WIDTH)) & PACK_MASK];
  }
  return result.str();
}

Key64
Key64::encode_rymer(const std::string& sequence)
{
    key_type packed = 0;  // Initialize the packed key as 0

    for(auto c : sequence)
    {
        unsigned char packed_char;

        // Convert the character to its Rymer equivalent
        if(c == 'A' || c == 'G') {
            packed_char = 0;
        } else if(c == 'C' || c == 'T') {
            packed_char = 1;
        } else {
            throw std::runtime_error("[ENCODE_RYMER] Key64::encode_rymer(): Unexpected character in input sequence: " + std::string(1, c));
        }

        // Left-shift the current packed key and add the new packed_char
        packed = (packed << 1) | packed_char;
    }

    return Key64(packed);
}


std::string
Key64::decode_rymer(size_t k) const
{
    std::stringstream result;
    // Decode using the kmer scheme
    for(size_t i = 0; i < k; i++)
    {
        char decoded_char = PACK_TO_CHAR_RYMER[(this->key >> ((k - i - 1) * PACK_WIDTH_RYMER)) & PACK_MASK_RYMER];
        //std::cerr << "Decoded character using kmer scheme: " << decoded_char << std::endl;

        // Convert the decoded character to its Rymer equivalent
        if(decoded_char == 'G') {
            decoded_char = 'A';
        } else if(decoded_char == 'T') {
            decoded_char = 'C';
        }

        result << decoded_char;
    }

    std::string decodedStr = result.str();
    //std::cerr << "Decoded sequence using Rymer scheme: " << decodedStr << std::endl;

    return decodedStr;
}


Key64 Key64::reverse_complement(size_t k) const
{
  value_type source = ~(this->get_key()); // Complement of the kmer, plus weird high-order bits.
  code_type result = 0;
  for(size_t i = 0; i < k; i++)
  {
    result = (result << PACK_WIDTH) | (source & PACK_MASK);
    source >>= PACK_WIDTH;
  }
  return Key64(result);
}

Key64 Key64::reverse_complement_rymer(size_t k) const
{
    key_type source = this->key; // Assuming 'key' holds the rymer-encoded value
    key_type result = 0;
    for(size_t i = 0; i < k; i++)
    {
        // Extract the current bits and complement them. Since we're dealing with a 2-letter scheme,
        // complementing is effectively toggling between 0 and 1 (A ↔ C in the rymer scheme).
        key_type current_bits = source & PACK_MASK_RYMER;
        key_type complement_bits = current_bits ^ PACK_MASK_RYMER;

        // Shift left and add the complemented bits to the result
        result = (result << PACK_WIDTH_RYMER) | complement_bits;

        // Shift the source right to process the next character in the next iteration
        source >>= PACK_WIDTH_RYMER;
    }

    // Assuming Key64 can be constructed with a key_type argument
    return Key64(result);
}

std::ostream&
operator<<(std::ostream& out, Key64 value)
{
  out << value.key;
  return out;
}

Key128
Key128::encode(const std::string& sequence)
{

  throw std::runtime_error("WE SHOULD NOT BE IN KEY128");
  size_t low_limit = (sequence.size() > FIELD_CHARS ? FIELD_CHARS : sequence.size());
  
  key_type packed_high = 0;
  key_type packed_low = 0;
  
  for(size_t i = 0; i < sequence.size(); i++)
  {
    auto c = sequence[i];
    auto packed_char = CHAR_TO_PACK[c];
    if(packed_char > PACK_MASK)
    {
      throw std::runtime_error("Key128::encode(): Cannot encode character '" + std::to_string(c) + "'");
    }
    
    key_type& pack_to = (i < sequence.size() - low_limit) ? packed_high : packed_low;
    
    pack_to = (pack_to << PACK_WIDTH) | packed_char;
  }
  
  return Key128(packed_high, packed_low);
}

std::string
Key128::decode(size_t k) const
{
  throw std::runtime_error("WE SHOULD NOT BE IN KEY128");
  std::stringstream result;
  size_t low_limit = (k > FIELD_CHARS ? FIELD_CHARS : k);
  for(size_t i = FIELD_CHARS; i < k; i++)
  {
    result << PACK_TO_CHAR[(this->high >> ((k - i - 1) * PACK_WIDTH)) & PACK_MASK];
  }
  for(size_t i = 0; i < low_limit; i++)
  {
    result << PACK_TO_CHAR[(this->low >> ((low_limit - i - 1) * PACK_WIDTH)) & PACK_MASK];
  }
  return result.str();
}

std::ostream&
operator<<(std::ostream& out, Key128 value)
{
  out << "(" << value.high << ", " << value.low << ")";
  return out;
}

//------------------------------------------------------------------------------

void
hits_in_subgraph(size_t hit_count, const hit_type* hits, const std::unordered_set<nid_t>& subgraph,
                 const std::function<void(pos_t, payload_type)>& report_hit)

{
  for(const hit_type* ptr = hits; ptr < hits + hit_count; ++ptr)
  {
    auto iter = subgraph.find(Position::id(ptr->pos));
    if(iter != subgraph.end()) { report_hit(Position::decode(ptr->pos), ptr->payload); }
  }
}

/*
  Exponential search that returns the first offset with get_value(offset) >= target.
  We assume that start < limit and get_value(start) < target.
  Returns limit if get_value(offset) < target for all offset < limit.
*/
size_t
exponential_search(size_t start, size_t limit, nid_t target, const std::function<nid_t(size_t)>& get_value)
{
  // Exponential search: low is too early.
  size_t step = 1;
  size_t low = start, candidate = start + step;
  while(candidate < limit && get_value(candidate) < target)
  {
    step *= 2;
    low = candidate; candidate += step;
  }

  // Binary search: low + 1 is the first candidate while candidate is the last.
  low++;
  size_t count = std::min(limit, candidate + 1) - low;
  while(count > 0)
  {
    step = count / 2;
    candidate = low + step;
    if(get_value(candidate) < target) { low = candidate + 1; count -= step + 1; }
    else { count = step; }
  }
  return low;
}

void
hits_in_subgraph(size_t hit_count, const hit_type* hits, const std::vector<nid_t>& subgraph,
                 const std::function<void(pos_t, payload_type)>& report_hit)
{

  size_t hit_offset = 0, subgraph_offset = 0;
  while(hit_offset < hit_count && subgraph_offset < subgraph.size())
  {
    nid_t node = Position::id(hits[hit_offset].pos);
    if(node < subgraph[subgraph_offset])
    {
      hit_offset = exponential_search(hit_offset, hit_count, subgraph[subgraph_offset], [&](size_t offset) -> nid_t
      {
        return Position::id(hits[offset].pos);
      });
    }
    else if(node > subgraph[subgraph_offset])
    {
      subgraph_offset = exponential_search(subgraph_offset, subgraph.size(), node, [&](size_t offset) -> nid_t
      {
        return subgraph[offset];
      });
    }
    else
    {
      report_hit(Position::decode(hits[hit_offset].pos), hits[hit_offset].payload);
      ++hit_offset;
    }
  }
}

//------------------------------------------------------------------------------

} // namespace gbwtgraph
