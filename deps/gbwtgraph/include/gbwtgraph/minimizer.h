#ifndef GBWTGRAPH_MINIMIZER_H
#define GBWTGRAPH_MINIMIZER_H

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <gbwt/utils.h>

#include <gbwtgraph/io.h>
#include <gbwtgraph/utils.h>
#define RYMER

/*
  minimizer.h: Minimizer index.
*/

namespace gbwtgraph
{

//------------------------------------------------------------------------------

struct MinimizerHeader
{
  std::uint32_t tag, version;
  std::uint64_t k, w;
  std::uint64_t keys, capacity, max_keys;
  std::uint64_t values;
  std::uint64_t unique;
  std::uint64_t flags;

  constexpr static std::uint32_t TAG = 0x31513151;
  constexpr static std::uint32_t VERSION = Version::MINIMIZER_VERSION;

  constexpr static std::uint64_t FLAG_MASK       = 0x01FF;
  constexpr static std::uint64_t FLAG_KEY_MASK   = 0x00FF;
  constexpr static size_t        FLAG_KEY_OFFSET = 0;
  constexpr static std::uint64_t FLAG_SYNCMERS   = 0x0100;

  MinimizerHeader();
  MinimizerHeader(size_t kmer_length, size_t window_length, size_t initial_capacity, double max_load_factor, size_t key_bits);
  void sanitize(size_t kmer_max_length);

  // Throws `sdsl::simple_sds::InvalidData` if the header is invalid.
  void check() const;

  void update_version(size_t key_bits);

  // Boolean flags.
  void set(std::uint64_t flag) { this->flags |= flag; }
  void unset(std::uint64_t flag) { this->flags &= ~flag; }
  bool get_flag(std::uint64_t flag) const { return (this->flags & flag); }

  // Integer flags.
  void set_int(std::uint64_t mask, size_t offset, size_t value);
  size_t get_int(std::uint64_t mask, size_t offset) const;

  size_t key_bits() const;

  bool operator==(const MinimizerHeader& another) const;
  bool operator!=(const MinimizerHeader& another) const { return !(this->operator==(another)); }
};

//------------------------------------------------------------------------------

/*
  Types used for internal representation of positions in the minimizer index.
  These are exposed through the interface.
*/

typedef std::uint64_t code_type;
//typedef std::pair<std::uint64_t, std::uint64_t> payload_type;

struct payload_type
{

  std::uint64_t first, second;

  // This is not a constructor in order to have default constructors and operators in
  // objects containing payload_type.
  static payload_type create(std::uint64_t value)
  {
    return { value, 0 };
  }

  bool operator==(payload_type another) const
  {
    return (this->first == another.first && this->second == another.second);
  }

  bool operator!=(payload_type another) const
  {
    return !(*this == another);
  }

  bool operator<(payload_type another) const
  {
    return (this->first < another.first) || (this->first == another.first && this->second < another.second);
  }
};

struct hit_type
{
  code_type    pos;
  payload_type payload;

  // Payload is irrelevant for comparisons.
  bool operator==(const hit_type& another) const { return (this->pos == another.pos); }
  bool operator!=(const hit_type& another) const { return (this->pos != another.pos); }
  bool operator<(const hit_type& another) const { return (this->pos < another.pos); }
  bool operator>(const hit_type& another) const { return (this->pos > another.pos); }

};

struct Position
{
  // Constants for the encoding between pos_t and code_type.
  // Note: These must be consistent with the top-level MAX_NODE_LENGTH.
  constexpr static size_t    OFFSET_BITS = 10;
  constexpr static size_t    ID_OFFSET   = OFFSET_BITS + 1;
  constexpr static code_type REV_MASK    = static_cast<code_type>(1) << OFFSET_BITS;
  constexpr static code_type OFF_MASK    = REV_MASK - 1;

  // Is the offset small enough to fit in the low-order bits of the encoding?
  static bool valid_offset(const pos_t& pos) { return (offset(pos) <= OFF_MASK); }

  // Encode pos_t as code_type.
  static code_type encode(const pos_t& pos)
  {
    return (static_cast<code_type>(gbwtgraph::id(pos)) << ID_OFFSET) |
           (static_cast<code_type>(gbwtgraph::is_rev(pos)) << OFFSET_BITS) |
           (static_cast<code_type>(gbwtgraph::offset(pos)) & OFF_MASK);
  }

  // Decode code_type as pos_t.
  static pos_t decode(code_type pos) { return make_pos_t(pos >> ID_OFFSET, pos & REV_MASK, pos & OFF_MASK); }

  // Return the node id in the encoded position.
  static nid_t id(code_type pos) { return (pos >> ID_OFFSET); }
};

//------------------------------------------------------------------------------

/*
  A kmer encoded using 2 bits/character in a 64-bit integer. The encoding is only
  defined if all characters in the kmer are valid. The highest bit indicates
  whether the corresponding value in the hash table is a position or a pointer.
*/

struct Key128;

struct Key64
{

public:
  // Internal representation.
  typedef std::uint64_t key_type;
  typedef key_type value_type;
  key_type key;

  // Empty key.
  constexpr Key64() : key(EMPTY_KEY) {}

  // No key.
  constexpr static Key64 no_key() { return Key64(NO_KEY & KEY_MASK); }

  // Implicit conversion for testing.
  constexpr Key64(key_type value) : key(value) {}

  // Get a representation of the actual key.
  value_type get_key() const { return (this->key & KEY_MASK); }

  // Comparisons.
  bool operator==(Key64 another) const { return (this->get_key() == another.get_key()); }
  bool operator!=(Key64 another) const { return (this->get_key() != another.get_key()); }
  bool operator<(Key64 another) const { return (this->get_key() < another.get_key()); }

  // Is the corresponding value a pointer?
  bool is_pointer() const { return (this->key & IS_POINTER); }
  void clear_pointer() { this->key &= ~IS_POINTER; }
  void set_pointer() { this->key |= IS_POINTER; }

  // Hash of the key.
  size_t hash() const { return wang_hash_64(this->key & KEY_MASK); }

  // Move the kmer forward, with c as the next character. Update the key, assuming that
  // it encodes the kmer in forward orientation.
  void forward(size_t k, unsigned char c, size_t& valid_chars)
  {
    key_type packed = CHAR_TO_PACK[c];
    if(packed > PACK_MASK) { this->key = EMPTY_KEY; valid_chars = 0; }
    else
    {
      this->key = ((this->key << PACK_WIDTH) | packed) & KMER_MASK[k];
      valid_chars++;
    }
  }

  // Move the kmer forward, with c as the next character. Update the key, assuming that
  // it encodes the kmer in reverse orientation.
  void reverse(size_t k, unsigned char c)
  {
    key_type packed = CHAR_TO_PACK[c];
    if(packed > PACK_MASK) { this->key = EMPTY_KEY; }
    else
    {
      packed ^= PACK_MASK; // The complement of the base.
      this->key = (packed << ((k - 1) * PACK_WIDTH)) | (this->key >> PACK_WIDTH);
    }
  }

#ifdef RYMER

// Corrected forward_rymer function
void forward_rymer(size_t k, unsigned char c, size_t& valid_chars)
{
    static key_type KMER_MASK_RYMER[65] = { 0 }; // Initialize all values to 0
    static bool is_initialized = false;
    if (!is_initialized) {
        for(size_t i = 1; i <= 32; i++) { // Assuming a maximum k of 32
            KMER_MASK_RYMER[i] = (1UL << i) - 1;  // Only shifting by i bits
        }
        is_initialized = true;
    }

    key_type packed = CHAR_TO_PACK_RYMER[c];
    if(packed > PACK_MASK_RYMER)
    {
        this->key = EMPTY_KEY;
        valid_chars = 0;
    }
    else
    {
        this->key = ((this->key << 1) | packed) & KMER_MASK_RYMER[k];  // Only shifting by 1 bit
        valid_chars++;
    }
}

// Corrected reverse_rymer function
void reverse_rymer(size_t k, unsigned char c)
{
    key_type packed = CHAR_TO_PACK_RYMER[c];
    if(packed > PACK_MASK_RYMER)
    {
        this->key = EMPTY_KEY;
    }
    else
    {
        packed ^= PACK_MASK_RYMER;  // The complement of the base.
        this->key = (packed << (k - 1)) | (this->key >> 1);  // Only shifting by 1 bit
    }
}

key_type
minimizerToRymer(key_type minimizer_key, size_t k)
{
    key_type rymer_key = 0;  // Initialize the Rymer key as 0

    for(size_t i = 0; i < k; i++)
    {
        // Extract the base from the minimizer key
        unsigned char base = (minimizer_key >> ((k - i - 1) * 2)) & 0x3;

        // Convert the base to its Rymer equivalent
        unsigned char rymer_base;
        if(base == 0 /* A */ || base == 2 /* G */) {
            rymer_base = 0;  // 'A' in Rymer
        } else if(base == 1 /* C */ || base == 3 /* T */) {
            rymer_base = 1;  // 'C' in Rymer
        } else {
            throw std::runtime_error("Unexpected base value in minimizer key.");
        }

        // Append the Rymer base to the Rymer key
        rymer_key = (rymer_key << 1) | rymer_base;
    }

    return rymer_key;
}

#endif

  /// Encode a string of size k to a key.
  static Key64 encode(const std::string& sequence);

#ifdef RYMER
  static Key64 encode_rymer(const std::string& sequence);
#endif

  /// Decode the key back to a string, given the kmer size used.
  std::string decode(size_t k) const;

#ifdef RYMER
  std::string decode_rymer(size_t k) const;

  key_type get_original_kmer_key(const std::string& rymerStr) const;
  std::string rymer_key_to_minimizer_string(key_type rymer_key, size_t k) const;

  Key64 reverse_complement(size_t k) const;
  Key64 reverse_complement_rymer(size_t k) const;

#endif

  // Required numeric constants.
  constexpr static std::size_t KEY_BITS = sizeof(key_type) * gbwt::BYTE_BITS;
  constexpr static std::size_t KMER_LENGTH = 21;
  constexpr static std::size_t WINDOW_LENGTH = 11;
  constexpr static std::size_t SMER_LENGTH = KMER_LENGTH - WINDOW_LENGTH;
  constexpr static std::size_t KMER_MAX_LENGTH = 31;

private:
  // Specific key values. Note that the highest bit is not a part of the key.
  constexpr static key_type EMPTY_KEY = 0;
  constexpr static key_type NO_KEY = std::numeric_limits<key_type>::max();
  constexpr static key_type KEY_MASK = NO_KEY >> 1;
  constexpr static key_type IS_POINTER = NO_KEY ^ KEY_MASK; // High bit.

  // Constants for the encoding between std::string and the key.
  constexpr static size_t   PACK_WIDTH = 2;
  constexpr static key_type PACK_MASK  = 0x3;
  constexpr static size_t  PACK_WIDTH_RYMER = 1;
  constexpr static key_type PACK_MASK_RYMER  = 0x1;

  // Arrays for the encoding between std::string and the key.
  const static std::vector<unsigned char> CHAR_TO_PACK;
  const static std::vector<unsigned char> CHAR_TO_PACK_RYMER;
  const static std::vector<char>          PACK_TO_CHAR;
  const static std::vector<char>          PACK_TO_CHAR_RYMER;
  const static std::vector<key_type>      KMER_MASK;
  const static std::vector<key_type>      RYMER_MASK;

  friend Key128;
};

// Required for printing keys.
std::ostream& operator<<(std::ostream& out, Key64 value);

//------------------------------------------------------------------------------

/*
  A kmer encoded using 2 bits/character in a pair of 64-bit integers. The encoding is
  only defined if all characters in the kmer are valid. The highest bit indicates
  whether the corresponding value in the hash table is a position or a pointer.
*/

struct Key128
{
public:
  // Internal representation.
  typedef std::uint64_t key_type;
  typedef std::pair<key_type, key_type> value_type;
  constexpr static std::size_t FIELD_BITS = sizeof(key_type) * gbwt::BYTE_BITS;
  key_type high, low;

  // Empty key.
  constexpr Key128() : high(EMPTY_KEY), low(EMPTY_KEY) {}

  // No key.
  constexpr static Key128 no_key() { return Key128(NO_KEY & KEY_MASK, NO_KEY); }

  // Implicit conversion for testing.
  constexpr Key128(key_type key) : high(EMPTY_KEY), low(key) {}

  // For testing.
  constexpr Key128(key_type high, key_type low) : high(high), low(low) {}

  // Get a representation of the actual key.
  value_type get_key() const { return value_type(this->high & KEY_MASK, this->low); }

  // Comparisons.
  bool operator==(Key128 another) const { return (this->get_key() == another.get_key()); }
  bool operator!=(Key128 another) const { return (this->get_key() != another.get_key()); }
  bool operator<(Key128 another) const { return (this->get_key() < another.get_key()); }

  // Is the corresponding value a pointer?
  bool is_pointer() const { return (this->high & IS_POINTER); }
  void clear_pointer() { this->high &= ~IS_POINTER; }
  void set_pointer() { this->high |= IS_POINTER; }

  // Hash of the key. Essentially boost::hash_combine.
  size_t hash() const
  {
    size_t result = wang_hash_64(this->high & KEY_MASK);
    result ^= wang_hash_64(this->low) + 0x9e3779b9 + (result << 6) + (result >> 2);
    return result;
  }

  // Move the kmer forward, with c as the next character. Update the key, assuming that
  // it encodes the kmer in forward orientation.
  void forward(size_t k, unsigned char c, size_t& valid_chars)
  {
    key_type packed = CHAR_TO_PACK[c];
    if(packed > PACK_MASK) { this->high = EMPTY_KEY; this->low = EMPTY_KEY; valid_chars = 0; }
    else
    {
      this->high = ((this->high << PACK_WIDTH) | (this->low >> PACK_OVERFLOW)) & HIGH_MASK[k];
      this->low = ((this->low << PACK_WIDTH) | packed) & LOW_MASK[k];
      valid_chars++;
    }
  }

  // Move the kmer forward, with c as the next character. Update the key, assuming that
  // it encodes the kmer in reverse orientation.
  void reverse(size_t k, unsigned char c)
  {
    key_type packed = CHAR_TO_PACK[c];
    if(packed > PACK_MASK) { this->high = EMPTY_KEY; this->low = EMPTY_KEY; }
    else
    {
      packed ^= PACK_MASK; // The complement of the base.
      if(k > FIELD_CHARS)
      {
        this->low = ((this->high & PACK_MASK) << PACK_OVERFLOW) | (this->low >> PACK_WIDTH);
        this->high = (packed << ((k - FIELD_CHARS - 1) * PACK_WIDTH)) | (this->high >> PACK_WIDTH);
      }
      else // The entire kmer is in the lower part of the key.
      {
        this->low = (packed << ((k - 1) * PACK_WIDTH)) | (this->low >> PACK_WIDTH);
      }
    }
  }


  /// Encode a string of size k to a key.
  static Key128 encode(const std::string& sequence);

  /// Decode the key back to a string, given the kmer size used.
  std::string decode(size_t k) const;

  // Required numeric constants.
  constexpr static std::size_t KEY_BITS = 2 * FIELD_BITS;
  constexpr static std::size_t KMER_LENGTH = 39;
  constexpr static std::size_t WINDOW_LENGTH = 15;
  constexpr static std::size_t SMER_LENGTH = KMER_LENGTH - WINDOW_LENGTH;
  constexpr static std::size_t KMER_MAX_LENGTH = 63;

private:
  // Specific key values. Note that the highest bit is not a part of the key.
  constexpr static key_type EMPTY_KEY = 0;
  constexpr static key_type NO_KEY = std::numeric_limits<key_type>::max();
  constexpr static key_type KEY_MASK = NO_KEY >> 1;
  constexpr static key_type IS_POINTER = NO_KEY ^ KEY_MASK; // High bit.

  // Constants for the encoding between std::string and the key.
  constexpr static size_t   PACK_WIDTH    = 2;
  constexpr static size_t   PACK_WIDTH_RYMER    = 1;
  constexpr static size_t   PACK_OVERFLOW = FIELD_BITS - PACK_WIDTH;
  constexpr static size_t   FIELD_CHARS   = FIELD_BITS / PACK_WIDTH;
  constexpr static key_type PACK_MASK     = 0x3;

  // Arrays for the encoding between std::string and the key.
  const static std::vector<unsigned char> CHAR_TO_PACK;
  const static std::vector<char>          PACK_TO_CHAR;
  const static std::vector<key_type>      HIGH_MASK, LOW_MASK;
};

// Required for printing keys.
std::ostream& operator<<(std::ostream& out, Key128 value);

//------------------------------------------------------------------------------

/*
  A class that implements the minimizer index as a hash table mapping kmers to sets of pos_t.
  For each stored position, we also store 64 bits of payload for external purposes.
  The hash table uses quadratic probing with power-of-two size.
  We encode kmers using 2 bits/character and hash the encoding. A minimizer is the kmer with
  the smallest hash in a window of w consecutive kmers and their reverse complements.

  The index can also use closed syncmers:

    Edgar: Syncmers are more sensitive than minimizers for selecting conserved k-mers in
    biological sequences. PeerJ 9:e10805, 2021.

  A closed syncmer is a kmer where one of the occurrences of the smer (or its reverse
  complement) with the smallest hash is the first or the last. Because we want to select
  the same kmers in both orientations, we use the kmer or its reverse complement,
  depending on which has the smaller hash.

  Minimizers and closed syncmers should have roughly the same seed density when w = k - s.

  Index versions (this should be in the wiki):

    1  The initial version.

    2  Minimizer selection is based on hashes instead of lexicographic order. A sequence and
       its reverse complement have the same minimizers, reducing index size by 50%. Not
       compatible with version 1.

    3  Construction-time hit cap is no longer used. Compatible with version 2.

    4  Use SDSL data structures where appropriate. Not compatible with version 3.

    5  An option to use 128-bit keys. Compatible with version 4.

    6  Store the position/pointer bit in the key instead of a separate bit vector. Store
       64 bits of payload for each position.
       Not compatible with version 5.

    7  Option to use closed syncmers instead of minimizers. Compatible with version 6.

    8  Payload is now 128 bits per position. Not compatible with earlier versions.
*/

template<class KeyType>
class MinimizerIndex
{
public:
  typedef KeyType key_type;
  typedef std::uint32_t offset_type;

  // Public constants.
  constexpr static size_t       INITIAL_CAPACITY = 1024;
  constexpr static double       MAX_LOAD_FACTOR  = 0.77;
  constexpr static code_type    NO_VALUE         = 0;
  constexpr static payload_type DEFAULT_PAYLOAD  = {0, 0};

  // Serialize the hash table in blocks of this many cells.
  constexpr static size_t BLOCK_SIZE = 4 * gbwt::MEGABYTE;

  const static std::string EXTENSION; // ".min"

  union value_type
  {
    hit_type value;
    std::vector<hit_type>* pointer;
  };

  typedef std::pair<key_type, value_type> cell_type;

  constexpr static hit_type empty_hit() { return { NO_VALUE, DEFAULT_PAYLOAD }; }
  constexpr static cell_type empty_cell() { return cell_type(key_type::no_key(), { empty_hit() }); }

  /*
    The sequence offset of a minimizer is the base that corresponds to the start of the
    minimizer. For minimizers in forward orientation, the offset is the first base
    covered by the minimizer. For reverse orientation, the offset is the last base
    covered by the minimizer.
  */
  struct minimizer_type
  {
    key_type    key;        // Encoded minimizer.
    size_t      hash;       // Hash of the minimizer.
    offset_type offset;     // Sequence offset.
    bool        is_reverse; // The minimizer is the reverse complement of the kmer.
    std::string kmer_seq = "";
    bool matched = false;
    key_type original_kmer_key;
    size_t original_kmer_hash;

    // Is the minimizer empty?
    bool empty() const { return (this->key == key_type::no_key()); }

    // Sort by (offset, !is_reverse). When the offsets are equal, a reverse complement
    // minimizer is earlier in the sequence than a forward minimizer.
    bool operator<(const minimizer_type& another) const
    {
      return ((this->offset < another.offset) ||
              (this->offset == another.offset && this->is_reverse > another.is_reverse));
    }

    bool operator==(const minimizer_type& another) const
    {
      return (this->key == another.key && this->offset == another.offset && this->is_reverse == another.is_reverse);
    }

    bool operator!=(const minimizer_type& another) const
    {
      return !(this->operator==(another));
    }
  };

//------------------------------------------------------------------------------

  explicit MinimizerIndex(bool use_syncmers = false) :
    header(KeyType::KMER_LENGTH, (use_syncmers ? KeyType::SMER_LENGTH : KeyType::WINDOW_LENGTH),
           INITIAL_CAPACITY, MAX_LOAD_FACTOR, KeyType::KEY_BITS),
    hash_table(this->header.capacity, empty_cell())
  {
    if(use_syncmers) { this->header.set(MinimizerHeader::FLAG_SYNCMERS); }
    this->header.sanitize(KeyType::KMER_MAX_LENGTH);
  }

  MinimizerIndex(size_t kmer_length, size_t window_or_smer_length, bool use_syncmers = false) :
    header(kmer_length, window_or_smer_length, INITIAL_CAPACITY, MAX_LOAD_FACTOR, KeyType::KEY_BITS),
    hash_table(this->header.capacity, empty_cell())
  {
    if(use_syncmers) { this->header.set(MinimizerHeader::FLAG_SYNCMERS); }
    this->header.sanitize(KeyType::KMER_MAX_LENGTH);
  }

  MinimizerIndex(const MinimizerIndex& source)
  {
    this->copy(source);
  }

  MinimizerIndex(MinimizerIndex&& source)
  {
    *this = std::move(source);
  }

  ~MinimizerIndex()
  {
    this->clear();
  }

  void swap(MinimizerIndex& another)
  {
    if(&another == this) { return; }
    std::swap(this->header, another.header);
    this->hash_table.swap(another.hash_table);
  }


  // Call `callback` for every non-empty hash table cell.
  // If callback returns false, then stop iterating.
  // Returns false if the iteration stopped early, true otherwise.
  bool for_each_kmer(const std::function<bool(const cell_type&)>& callback) const
  {
    for(const cell_type& cell : this->hash_table)
    {
      if(cell.first != key_type::no_key())
      {
          if(!callback(cell))
          {
              return false;
          }
      }
    }
    return true;
  }


std::string get_minimizer_seq(auto &r, auto &rymer_index)
{
    size_t offset = rymer_index.find_offset(r.value.key, r.value.hash);
    bool reversed = r.value.is_reverse;
    uint64_t payload = rymer_index.hash_table[offset].second.value.payload.first;
    if (payload == 0) {
        // Return a string of 'N's with length equal to rymer_index.k()
        return std::string(rymer_index.k(), 'N');
    }
    gbwtgraph::Key64 key(payload);
    std::string seq = key.decode(rymer_index.k());
    if (reversed) {
        seq = reverse_complement(seq);
    }
    return seq;
}

void dump_hash_table(){
    int k = this->k();
    int row = 0; // Initialize row counter
    // Iterate over each cell in the hash table
    for (const auto& cell : this->hash_table) {
        // Check if the cell is not empty (i.e., contains a key)
        if (cell.first != gbwtgraph::Key64::no_key()) {
            // Decode the sequence from the key, using precomputed k
            const auto &minimizer_seq = cell.first.decode(k);
            std::cerr << "  kmer key: " << cell.first.get_key() << "  KMER PAYLOAD: " << cell.second.value.payload.first << '\t' << \
           cell.second.value.payload.second << "  kmer seq: " << minimizer_seq << std::endl;
        }
        row++; // Increment row counter for each cell
    }
    return;
}


void dump_hash_table_rymer(){
int k=this->k();
    // Iterate over each cell in the hash table
    for (const auto& cell : this->hash_table) {
        // Check if the cell is not empty (i.e., contains a key)
        if (cell.first != gbwtgraph::Key64::no_key()) {
            // Decode the sequence from the key, using precomputed k
            const auto &rymer_seq = cell.first.decode_rymer(k);
            std::cerr << "RYMER KEY: " << cell.first.get_key() << "  RYMER PAYLOAD: " << cell.second.value.payload.first << '\t' << \
            cell.second.value.payload.second << "rymer seq: " << rymer_seq << "  kmer seq: " << gbwtgraph::Key64(cell.second.value.payload.first).decode(k) << std::endl;
        }
    }
    return;
}

  MinimizerIndex& operator=(const MinimizerIndex& source)
  {
    if(&source != this) { this->copy(source); }
    return *this;
  }

  MinimizerIndex& operator=(MinimizerIndex&& source)
  {
    if(&source != this)
    {
      this->header = std::move(source.header);
      this->hash_table = std::move(source.hash_table);
    }
    return *this;
  }

  // Serialize the index to the ostream. Returns the number of bytes written and
  // true if the serialization was successful.
  std::pair<size_t, bool> serialize(std::ostream& out) const
  {
    size_t bytes = 0;
    bool ok = true;

    bytes += io::serialize(out, this->header, ok);
    bytes += io::serialize_hash_table(out, this->hash_table, empty_hit(), ok);

    // Serialize the occurrence lists.
    for(size_t i = 0; i < this->capacity(); i++)
    {
      if(this->hash_table[i].first.is_pointer())
      {
        bytes += io::serialize_vector(out, *(this->hash_table[i].second.pointer), ok);
      }
    }

    if(!ok)
    {
      std::cerr << "MinimizerIndex::serialize(): Serialization failed" << std::endl;
    }

    return std::make_pair(bytes, ok);
  }

  // Load the index from the istream and return true if successful.
  bool deserialize(std::istream& in)
  {
    bool ok = true;

    // Load and check the header.
    ok &= io::load(in, this->header);
    try { this->header.check(); }
    catch(const std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
      return false;
    }
    if(this->header.key_bits() != KeyType::KEY_BITS)
    {
      std::cerr << "MinimizerIndex::deserialize(): Expected " << KeyType::KEY_BITS << "-bit keys, got " << this->header.key_bits() << "-bit keys" << std::endl;
      return false;
    }
    this->header.update_version(KeyType::KEY_BITS);

    // Load the hash table.
    if(ok) { ok &= io::load_vector(in, this->hash_table); }

    // Load the occurrence lists.
    if(ok)
    {
      for(size_t i = 0; i < this->capacity(); i++)
      {
        if(this->hash_table[i].first.is_pointer())
        {
          this->hash_table[i].second.pointer = new std::vector<hit_type>();
          ok &= io::load_vector(in, *(this->hash_table[i].second.pointer));
        }
      }
    }

    if(!ok)
    {
      std::cerr << "MinimizerIndex::deserialize(): Index loading failed" << std::endl;
    }

    return ok;
  }

  // For testing.
  bool operator==(const MinimizerIndex& another) const
  {
    if(this->header != another.header) { return false; }

    for(size_t i = 0; i < this->capacity(); i++)
    {
      cell_type a = this->hash_table[i], b = another.hash_table[i];
      if(a.first != b.first) { return false; }
      if(a.first.is_pointer() != b.first.is_pointer()) { return false; }
      if(a.first.is_pointer())
      {
        if(*(a.second.pointer) != *(b.second.pointer)) { return false; }
      }
      else
      {
        if(a.second.value != b.second.value) { return false; }
      }
    }

    return true;
  }



  bool operator!=(const MinimizerIndex& another) const { return !(this->operator==(another)); }

//------------------------------------------------------------------------------

  /*
    A circular buffer of size 2^i for all minimizer candidates. The candidates are sorted
    by both key and sequence offset. The candidate at offset j is removed when we reach
    offset j + w. Candidates at the tail are removed when we advance with a smaller hash.
  */
  struct CircularBuffer
  {
    std::vector<minimizer_type> buffer;
    size_t head, tail;
    size_t w;

    constexpr static size_t BUFFER_SIZE = 16;

    CircularBuffer(size_t capacity) :
      buffer(),
      head(0), tail(0), w(capacity)
    {
      size_t buffer_size = BUFFER_SIZE;
      while(buffer_size < this->w) { buffer_size *= 2; }
      this->buffer.resize(buffer_size);
    }

    bool empty() const { return (this->head >= this->tail); }
    size_t size() const { return this->tail - this->head; }

    size_t begin() const { return this->head; }
    size_t end() const { return this->tail; }
    minimizer_type& at(size_t i) { return this->buffer[i & (this->buffer.size() - 1)]; }
    minimizer_type& front() { return this->at(this->begin()); }
    minimizer_type& back() { return this->at(this->end() - 1); }

    // Advance to the next offset (pos) with a valid kmer.
    void advance(offset_type pos, key_type forward_key, key_type reverse_key)
    {
      if(!(this->empty()) && this->front().offset + this->w <= pos) { this->head++; }
      size_t forward_hash = forward_key.hash(), reverse_hash = reverse_key.hash();
      size_t hash = std::min(forward_hash, reverse_hash);
      while(!(this->empty()) && this->back().hash > hash) { this->tail--; }
      this->tail++;
      if(reverse_hash < forward_hash) { this->back() = { reverse_key, reverse_hash, pos, true }; }
      else                            { this->back() = { forward_key, forward_hash, pos, false }; }
    }

    // Advance to the next offset (pos) without a valid kmer.
    void advance(offset_type pos)
    {
      if(!(this->empty()) && this->front().offset + this->w <= pos) { this->head++; }
    }
  };


key_type kmer2rymer(key_type& kmerKey, int kmerLength) const {
    gbwtgraph::Key64 rymerKey; // Initializes with the default constructor.
    for (int i = 0; i < kmerLength; ++i) {
        // Extract 2 bits representing a nucleotide in the kmer key
        gbwtgraph::Key64::key_type bits = (kmerKey.key >> ((kmerLength - i - 1) * 2)) & 0x3;
        // Collapse 'G'->'A' and 'T'->'C', effectively keeping only the least significant bit
        gbwtgraph::Key64::key_type rymerBit = bits & 0x1;
        rymerKey.key = (rymerKey.key << 1) | rymerBit;
    }
    return rymerKey;
}

  /*
    Returns all minimizers in the string specified by the iterators. The return
    value is a vector of minimizers sorted by their offsets. If there are multiple
    occurrences of one or more minimizer keys with the same hash in a window,
    return all of them.

    Calls syncmers() if the index uses closed syncmers.
  */



std::vector<minimizer_type> minimizers(std::string::const_iterator begin, std::string::const_iterator end, bool rymer) const
{
    if(this->uses_syncmers()) { 
        //std::cout << "[DEBUG] Using syncmers." << std::endl;
        return this->syncmers(begin, end); 
    }

    std::vector<minimizer_type> result;
    std::vector<minimizer_type> temp_result;
    size_t window_length = this->window_bp(), total_length = end - begin;
    //std::cout << "[DEBUG] Window length: " << window_length << std::endl;
    if(total_length < window_length) {
        //std::cout << "[DEBUG] Sequence length is shorter than window length." << std::endl;
        return result;
    }

    CircularBuffer buffer(this->w());
    size_t valid_chars = 0, start_pos = 0;
    size_t next_read_offset = 0;
    key_type forward_key, reverse_key;
    std::string::const_iterator iter = begin;
    while(iter != end)
    {

        forward_key.forward(this->k(), *iter, valid_chars);
        reverse_key.reverse(this->k(), *iter);

        if(valid_chars >= this->k()) { buffer.advance(start_pos, forward_key, reverse_key);}
        else                         { buffer.advance(start_pos); }

        ++iter;
        if(static_cast<size_t>(iter - begin) >= this->k()) { start_pos++; }

        if(static_cast<size_t>(iter - begin) >= window_length && !buffer.empty())
        {
            // Assuming rymer is a boolean indicating which vector to use
           const auto& selected_vector = rymer ? temp_result : result;
            if(selected_vector.empty() || selected_vector.back().hash == buffer.front().hash || \
                                          selected_vector.back().offset < buffer.front().offset)
            {
                for(size_t i = buffer.begin(); i < buffer.end() && buffer.at(i).hash == buffer.front().hash; i++)
                {
                    if(buffer.at(i).offset >= next_read_offset)
                    {
                        if(rymer){
                        auto &m = buffer.at(i);
                        m.original_kmer_key = m.key;
                        temp_result.emplace_back(m);
                                 }
                        else{
                        result.emplace_back(buffer.at(i));
                            }
                        next_read_offset = buffer.at(i).offset + 1;
                    }
                }
            }

            }

        }

if(!rymer) {
    for(minimizer_type& minimizer : result) {
        if(minimizer.is_reverse) { minimizer.offset += this->k() - 1; }
    }
    std::sort(result.begin(), result.end());
} else {

    for(minimizer_type& m : temp_result) {
        m.key = kmer2rymer(m.key, this->k());
        m.hash = m.key.hash();
        if(m.is_reverse) { m.offset += this->k() - 1; }
    }

    std::sort(temp_result.begin(), temp_result.end());
}

// Use the appropriate vector (result or temp_result) based on the rymer condition
return rymer ? temp_result : result;
}

  /*
    Returns all minimizers in the string. The return value is a vector of
    minimizers sorted by their offsets.

    Calls syncmers() if the index uses closed syncmers.
  */
  std::vector<minimizer_type> minimizers(const std::string& str, bool rymer) const
  {
    return this->minimizers(str.begin(), str.end(), rymer);
  }


  /*
    Returns all minimizers in the string. The return value is a vector of
    minimizers, region starts, and region lengths sorted by their offsets.

    Calls syncmers() if the index uses closed syncmers but leaves the start
    and length fields empty.
  */
  std::vector<std::tuple<minimizer_type, size_t, size_t>> minimizer_regions(const std::string& str, bool rymer) const
  {
    return this->minimizer_regions(str.begin(), str.end(), rymer);
  }

//------------------------------------------------------------------------------

  /*
    Returns all closed syncmers in the string specified by the iterators. The return
    value is a vector of minimizers sorted by their offsets.

    Calls minimizers() if the index uses minimizers.
  */

  std::vector<minimizer_type> syncmers(std::string::const_iterator begin, std::string::const_iterator end) const
  {
    if(!(this->uses_syncmers())) { return this->minimizers(begin, end, false); }
    std::vector<minimizer_type> result;
    size_t total_length = end - begin;
    if(total_length < this->k()) { return result; }

    // Find the closed syncmers.
    CircularBuffer buffer(this->k() + 1 - this->s());
    size_t processed_chars = 0, dummy_valid_chars = 0, valid_chars = 0, smer_start = 0;
    key_type forward_smer, reverse_smer, forward_kmer, reverse_kmer;
    std::string::const_iterator iter = begin;
    while(iter != end)
    {
      forward_smer.forward(this->s(), *iter, valid_chars);
      reverse_smer.reverse(this->s(), *iter);
      forward_kmer.forward(this->k(), *iter, dummy_valid_chars);
      reverse_kmer.reverse(this->k(), *iter);
      if(valid_chars >= this->s()) { buffer.advance(smer_start, forward_smer, reverse_smer); }
      else                         { buffer.advance(smer_start); }
      ++iter; processed_chars++;
      if(processed_chars >= this->s()) { smer_start++; }
      // We have a full kmer with a closed syncmer.
      if(valid_chars >= this->k())
      {
        // Insert the kmer if the first or the last smer is among the smallest, i.e. if
        // 1) the first smer in the buffer is at the start of the kmer;
        // 2) the first smer in the buffer is at the end of the kmer; or
        // 3) the last smer in the buffer is at the end of the kmer and shares shares
        //    the hash with the first smer.
        if(buffer.front().offset == processed_chars - this->k() ||
          buffer.front().offset == smer_start - 1 ||
          (buffer.back().offset == smer_start - 1 && buffer.back().hash == buffer.front().hash))
        {
          size_t forward_hash = forward_kmer.hash(), reverse_hash = reverse_kmer.hash();
          offset_type pos = processed_chars - this->k();
          if(reverse_hash < forward_hash)
          {
            result.push_back({ reverse_kmer, reverse_hash, pos, true });
          }
          else
          {
            result.push_back({ forward_kmer, forward_hash, pos, false });
          }
        }
      }
    }

    // It was more convenient to use the first offset of the kmer, regardless of the orientation.
    // If the closed syncmer is a reverse complement, we must return the last offset instead.
    for(minimizer_type& minimizer : result)
    {
      if(minimizer.is_reverse) { minimizer.offset += this->k() - 1; }
    }
    std::sort(result.begin(), result.end());

    return result;
  }

std::vector<std::tuple<minimizer_type, size_t, size_t>> minimizer_regions(
    std::string::const_iterator begin,
    std::string::const_iterator end,
    bool rymer
) const
{
    std::vector<std::tuple<minimizer_type, size_t, size_t>> result;
    if(this->uses_syncmers())
    {
        std::vector<minimizer_type> res = this->syncmers(begin, end);
        result.reserve(res.size());
        for(const minimizer_type& m : res) { result.emplace_back(m, 0, 0); }
        return result;
    }

    size_t window_length = this->window_bp(), total_length = end - begin;
    //std::cerr << "total len: " << total_length << std::endl;
    //std::cerr << "window len: " << window_length << std::endl;
    if(total_length < window_length) { return result; }

    CircularBuffer buffer(this->w());
    size_t valid_chars = 0, start_pos = 0;
    size_t next_read_offset = 0;
    size_t finished_through = 0;
    key_type forward_key, reverse_key;
    std::string::const_iterator iter = begin;
    while(iter != end)
    {

        forward_key.forward(this->k(), *iter, valid_chars);
        reverse_key.reverse(this->k(), *iter);

        if(valid_chars >= this->k()) { buffer.advance(start_pos, forward_key, reverse_key); }
        else { buffer.advance(start_pos); }

        ++iter;
        if(static_cast<size_t>(iter - begin) >= this->k()) { start_pos++; }

        if(static_cast<size_t>(iter - begin) >= window_length)
        {
            size_t window_start = static_cast<size_t>(iter - begin) - window_length;
            size_t prev_past_end_pos = window_start + window_length - 1;

            while(finished_through < result.size() &&
                std::get<0>(result[finished_through]).offset < window_start)
            {
                std::get<2>(result[finished_through]) = prev_past_end_pos - std::get<1>(result[finished_through]);
                finished_through++;
            }

            if (!buffer.empty())
            {
                if (result.empty() ||
                    std::get<0>(result.back()).hash == buffer.front().hash ||
                    std::get<0>(result.back()).offset < buffer.front().offset)
                {
                    for(size_t i = buffer.begin(); i < buffer.end() && buffer.at(i).hash == buffer.front().hash; i++)
                    {
                        if(buffer.at(i).offset >= next_read_offset)
                        {
                                result.emplace_back(buffer.at(i), window_start, 0);
                                next_read_offset = buffer.at(i).offset + 1;
                        }
                    }

                    while(!result.empty() &&
                          finished_through < result.size() &&
                          std::get<0>(result.back()).hash != std::get<0>(result[finished_through]).hash)
                    {
                        std::get<2>(result[finished_through]) = prev_past_end_pos - std::get<1>(result[finished_through]);
                        finished_through++;
                    }
                }
            }
        }
    }

    while(finished_through < result.size())
    {
        std::get<2>(result[finished_through]) = total_length - std::get<1>(result[finished_through]);
        finished_through++;
    }

    for(auto& record : result)
    {
        if(std::get<0>(record).is_reverse) { std::get<0>(record).offset += this->k() - 1; }
    }
    std::sort(result.begin(), result.end());

    return result;
}

  /*
    Returns all closed syncmers in the string. The return value is a vector of minimizers sorted
    by their offsets.

    Calls minimizers() if the index uses minimizers.
  */
  std::vector<minimizer_type> syncmers(const std::string& str) const
  {
    return this->syncmers(str.begin(), str.end());
  }

//------------------------------------------------------------------------------

  /*
    Inserts the value and the payload into the index, using minimizer.key as the
    key and minimizer.hash as its hash. Does not insert empty minimizers or
    values equal to NO_VALUE (0). Does not update the payload if the value has
    already been inserted with the same key.
    This version of insert() is intended for storing arbitrary values instead of
    graph positions.
    Use minimizer() or minimizers() to get the minimizer.
  */

  void insert(const minimizer_type& minimizer, code_type value, payload_type payload = DEFAULT_PAYLOAD)
  {
    if(minimizer.empty() || value == NO_VALUE) { return; }

    size_t offset = this->find_offset(minimizer.key, minimizer.hash);
    if(this->hash_table[offset].first == key_type::no_key())
    {
      this->insert(minimizer.key, { value, payload }, offset);
    }
    else if(this->hash_table[offset].first == minimizer.key)
    {
      this->append({ value, payload }, offset);
    }
  }

  /*
    Inserts the position and the payload into the index, using minimizer.key as
    the key and minimizer.hash as its hash. Does not insert empty minimizers or
    positions. Does not update the payload if the position has already been
    inserted with the same key.
    The offset of the position will be truncated to fit in OFFSET_BITS bits.
    Use minimizer() or minimizers() to get the minimizer and
    Position::valid_offset() to check if the offset fits in the available space.
    The position should match the orientation of the minimizer: a path label
    starting from the position should have the minimizer as its prefix.
  */
  void insert(const minimizer_type& minimizer, const pos_t& pos, payload_type payload = DEFAULT_PAYLOAD)
  {
    if(is_empty(pos)) { return; }
    code_type code = Position::encode(pos);
    this->insert(minimizer, code, payload);
  }


  /*
    Returns the sorted set of occurrences of the minimizer with their payloads.
    Use minimizer() or minimizers() to get the minimizer.
    If the minimizer is in reverse orientation, use reverse_base_pos() to reverse
    the reported occurrences.
  */
  std::vector<std::pair<pos_t, payload_type>> find(const minimizer_type& minimizer) const
  {
    std::vector<std::pair<pos_t, payload_type>> result;
    if(minimizer.empty()) { return result; }

    //std::cerr << "IN FIND, KEY: " << minimizer.key << std::endl;
    //std::cerr << "IN FIND, RYMER SEQ: " << minimizer.key.decode_rymer(this->k()) << std::endl;
    //std::cerr << "IN FIND, HASH: " << minimizer.hash << std::endl;
    size_t offset = this->find_offset(minimizer.key, minimizer.key.hash());
    //std::cerr << "IN FIND, OFFSET: " << offset << std::endl;
    const cell_type& cell = this->hash_table[offset];
    //std::cerr << "CELL.FIRST: " << cell.first << std::endl;
    //std::cerr << "CELL.SECOND.FIRST: " << cell.second.first << std::endl;
    if(cell.first == minimizer.key)
    {
      if(cell.first.is_pointer())
      {
        result.reserve(cell.second.pointer->size());
        for(hit_type hit : *(cell.second.pointer)) {
            //std::cerr << "IN FIND, PAYLOAD: " << hit.payload.first << std::endl;
            result.emplace_back(Position::decode(hit.pos), hit.payload);
                                                   }
      }
      else {
         //std::cerr << "IN FIND, PAYLOAD: " << cell.second.value.payload.first << std::endl;
         result.emplace_back(Position::decode(cell.second.value.pos), cell.second.value.payload);
           }
    }

    //std::cerr << "END OF FINDING HASH TABLE, CAME UP EMPTY" << std::endl;
    return result;
  }

  /*
    Returns the occurrence count of the minimizer.
    Use minimizer() or minimizers() to get the minimizer.
  */
  size_t count(const minimizer_type& minimizer) const
  {
    if(minimizer.empty()) { return 0; }

    size_t offset = this->find_offset(minimizer.key, minimizer.hash);
    const cell_type& cell = this->hash_table[offset];
    if(cell.first == minimizer.key)
    {
      return (cell.first.is_pointer() ? cell.second.pointer->size() : 1);
    }

    return 0;
  }

  /*
    Returns the occurrence count of the minimizer and a pointer to the internal
    representation of the occurrences (which are in sorted order) and their payloads.
    The pointer may be invalidated if new positions are inserted into the index.
    Use minimizer() or minimizers() to get the minimizer and Position::decode() to
    decode the occurrences.
    If the minimizer is in reverse orientation, use reverse_base_pos() to reverse
    the reported occurrences.
  */

  std::pair<size_t, const hit_type*> count_and_find(const minimizer_type& minimizer) const
  {
    std::pair<size_t, const hit_type*> result(0, nullptr);
    if(minimizer.empty()) {
         return result;
                          }

    size_t offset = this->find_offset(minimizer.key, minimizer.hash);
    if(this->hash_table[offset].first == minimizer.key)
    {
      const cell_type& cell = this->hash_table[offset];
      if(cell.first.is_pointer())
      {
        result.first = cell.second.pointer->size();
        result.second = cell.second.pointer->data();
      }
      else
      {
        result.first = 1; result.second = &(cell.second.value);
      }
    }
    return result;
  }


  std::pair<size_t, hit_type*> count_and_find_non_const(const minimizer_type& minimizer) const
  {
    std::pair<size_t, hit_type*> result(0, nullptr);
    if(minimizer.empty()) {
         return result;
                          }

    size_t offset = this->find_offset(minimizer.key, minimizer.hash);
    if(this->hash_table[offset].first == minimizer.key)
    {
      const cell_type& cell = this->hash_table[offset];
      if(cell.first.is_pointer())
      {
        result.first = cell.second.pointer->size();
        result.second = cell.second.pointer->data();
      }
      else
      {
        result.first = 1; result.second = const_cast<hit_type*>(&cell.second.value);
      }
    }
    return result;
  }
//------------------------------------------------------------------------------

  // Length of the kmers in the index.
  size_t k() const { return this->header.k; }

  // Window length for the minimizers. Does not make sense when using closed syncmers.
  size_t w() const { return this->header.w; }

  // Length of the smers when using closed syncmers. Does not make sense when using minimizers.
  size_t s() const { return this->header.w; }

  // Does the index use closed syncmers instead of minimizers.
  bool uses_syncmers() const { return this->header.get_flag(MinimizerHeader::FLAG_SYNCMERS); }

  // Window length in bp. We are guaranteed to have at least one kmer from the window if
  // all characters within it are valid.
  size_t window_bp() const
  {
    return (this->uses_syncmers() ? 2 * this->k() - this->s() - 1 : this->k() + this->w() - 1);
  }

  // Number of keys in the index.
  size_t size() const { return this->header.keys; }

  // Is the index empty.
  bool empty() const { return (this->size() == 0); }

  // Number of values (minimizer occurrences) in the index.
  size_t values() const { return this->header.values; }

  // Size of the hash table.
  size_t capacity() const { return this->header.capacity; }

  // Actual capacity of the hash table. Exceeding it will initiate rehashing.
  size_t max_keys() const { return this->header.max_keys; }

  // Current load factor of the hash table.
  double load_factor() const { return static_cast<double>(this->size()) / static_cast<double>(this->capacity()); }

  // Number of minimizers with a single occurrence.
  size_t unique_keys() const { return this->header.unique; }

size_t find_first(key_type key, size_t hash) const
{
  //std::cerr << "Initial hash value: " << hash << "\n";
  size_t offset = hash & (this->capacity() - 1);
  for(size_t attempt = 0; attempt < this->capacity(); attempt++)
  {
    if(this->hash_table[offset].first == key_type::no_key()) {
      // Print to cerr before returning when the key is no_key
      //std::cerr << "Returning offset for no_key: " << offset << std::endl;
      return -1;
    }
    if(this->hash_table[offset].first == key) {
      return this->hash_table[offset].second.value.payload.first;
    }

    // Quadratic probing with triangular numbers.
    offset = (offset + attempt + 1) & (this->capacity() - 1);
    //std::cerr << "New offset after probing: " << offset << "\n";
  }

  // This should not happen.
  std::cerr << "MinimizerIndex::find_offset(): Cannot find the offset for key " << key << std::endl;
  return -1;
}

size_t find_second(key_type key, size_t hash) const
{
  //std::cerr << "Initial hash value: " << hash << "\n";
  size_t offset = hash & (this->capacity() - 1);
  for(size_t attempt = 0; attempt < this->capacity(); attempt++)
  {
    if(this->hash_table[offset].first == key_type::no_key()) {
      // Print to cerr before returning when the key is no_key
      //std::cerr << "Returning offset for no_key: " << offset << std::endl;
      return -1;
    }
    if(this->hash_table[offset].first == key) {
      return this->hash_table[offset].second.value.payload.second;
    }

    // Quadratic probing with triangular numbers.
    offset = (offset + attempt + 1) & (this->capacity() - 1);
    //std::cerr << "New offset after probing: " << offset << "\n";
  }

  // This should not happen.
  std::cerr << "MinimizerIndex::find_offset(): Cannot find the offset for key " << key << std::endl;
  return -1;
}

//std::vector<cell_type> hash_table;
//------------------------------------------------------------------------------

private:
  MinimizerHeader        header;
  std::vector<cell_type> hash_table;
//------------------------------------------------------------------------------

private:
  void copy(const MinimizerIndex& source)
  {
    this->clear();
    this->header = source.header;
    this->hash_table = source.hash_table;
  }

  // Delete all pointers in the hash table.
  void clear()
  {
    for(size_t i = 0; i < this->hash_table.size(); i++)
    {
      cell_type& cell = this->hash_table[i];
      if(cell.first.is_pointer())
      {
        delete cell.second.pointer;
        cell.second.value = empty_hit();
        cell.first.clear_pointer();
      }
    }
  }


size_t find_offset(key_type key, size_t hash) const
{
  //std::cerr << "Initial hash value: " << hash << "\n";
  size_t offset = hash & (this->capacity() - 1);
  for(size_t attempt = 0; attempt < this->capacity(); attempt++)
  {
    if(this->hash_table[offset].first == key_type::no_key()) {
      // Print to cerr before returning when the key is no_key
      //std::cerr << "Returning offset for no_key: " << offset << std::endl;
      return offset;
    }
    if(this->hash_table[offset].first == key) {
      return offset; 
    }

    // Quadratic probing with triangular numbers.
    offset = (offset + attempt + 1) & (this->capacity() - 1);
    //std::cerr << "New offset after probing: " << offset << "\n";
  }

  // This should not happen.
  std::cerr << "MinimizerIndex::find_offset(): Cannot find the offset for key " << key << std::endl;
  return 0;
}


  // Insert (key, hit) to hash_table[offset], which is assumed to be empty.
  // Rehashing may be necessary.
  void insert(key_type key, hit_type hit, size_t offset)
  {
    this->hash_table[offset].first = key;
    this->hash_table[offset].second.value = hit;
    this->header.keys++;
    this->header.values++;
    this->header.unique++;

    if(this->size() > this->max_keys()) { this->rehash(); }
  }

  // Add pos to the list of occurrences of key at hash_table[offset].
  void append(hit_type hit, size_t offset)
  {
    if(this->contains(offset, hit)) { return; }

    cell_type& cell = this->hash_table[offset];
    if(cell.first.is_pointer())
    {
      std::vector<hit_type>* occs = cell.second.pointer;
      occs->push_back(hit);
      size_t offset = occs->size() - 1;
      while(offset > 0 && occs->at(offset - 1) > occs->at(offset))
      {
        std::swap(occs->at(offset - 1), occs->at(offset));
        offset--;
      }
    }
    else
    {
      std::vector<hit_type>* occs = new std::vector<hit_type>(2);
      occs->at(0) = cell.second.value;
      occs->at(1) = hit;
      if(occs->at(0) > occs->at(1)) { std::swap(occs->at(0), occs->at(1)); }
      cell.second.pointer = occs;
      cell.first.set_pointer();
      this->header.unique--;
    }
    this->header.values++;
  }

  // Does the list of occurrences at hash_table[offset] contain the hit?
  bool contains(size_t offset, hit_type hit) const
  {
    const cell_type& cell = this->hash_table[offset];
    if(cell.first.is_pointer())
    {
      const std::vector<hit_type>* occs = cell.second.pointer;
      return std::binary_search(occs->begin(), occs->end(), hit);
    }
    else
    {
      return (cell.second.value == hit);
    }
  }

  // Double the size of the hash table.
  void rehash()
  {
    // Reinitialize with a larger hash table.
    std::vector<cell_type> old_hash_table(2 * this->capacity(), empty_cell());
    this->hash_table.swap(old_hash_table);
    this->header.capacity = this->hash_table.size();
    this->header.max_keys = this->capacity() * MAX_LOAD_FACTOR;

    // Move the keys to the new hash table.
    for(size_t i = 0; i < old_hash_table.size(); i++)
    {
      const cell_type& source = old_hash_table[i];
      if(source.first == key_type::no_key()) { continue; }

      size_t offset = this->find_offset(source.first, source.first.hash());
      this->hash_table[offset] = source;
    }
  }


};

template<class KeyType>
std::ostream&
operator<<(std::ostream& out, const typename MinimizerIndex<KeyType>::minimizer_type& minimizer)
{
  out << "(" << minimizer.key << ", " << (minimizer.is_reverse ? "-" : "+") << minimizer.offset << ")";
  return out;
}

//------------------------------------------------------------------------------

/*
  Decode the subset of minimizer hits and their payloads in the given subgraph induced
  by node identifiers.
  This version should only be used when the number of hits is small.
  If the minimizer is in reverse orientation, use reverse_base_pos() to reverse
  the reported occurrences.
*/
void hits_in_subgraph(size_t hit_count, const hit_type* hits, const std::unordered_set<nid_t>& subgraph,
                      const std::function<void(pos_t, payload_type)>& report_hit);

/*
  Decode the subset of minimizer hits and their payloads in the given subgraph induced
  by node identifiers. The set of node ids must be in sorted order.
  This version uses exponential search on the larger list, so it should be efficient
  regardless of the size of the subgraph and the number of hits.
  If the minimizer is in reverse orientation, use reverse_base_pos() to reverse
  the reported occurrences.
*/
void hits_in_subgraph(size_t hit_count, const hit_type* hits, const std::vector<nid_t>& subgraph,
                      const std::function<void(pos_t, payload_type)>& report_hit);

//------------------------------------------------------------------------------

// Choose the default index type.
typedef MinimizerIndex<Key64> DefaultMinimizerIndex;
typedef MinimizerIndex<Key64> DefaultRymerIndex;
//typedef MinimizerIndex<Key128> DefaultMinimizerIndex;

//------------------------------------------------------------------------------

// Numerical template class constants.

template<class KeyType> constexpr size_t MinimizerIndex<KeyType>::INITIAL_CAPACITY;
template<class KeyType> constexpr double MinimizerIndex<KeyType>::MAX_LOAD_FACTOR;
template<class KeyType> constexpr code_type MinimizerIndex<KeyType>::NO_VALUE;
template<class KeyType> constexpr payload_type MinimizerIndex<KeyType>::DEFAULT_PAYLOAD;

// Other template class variables.

template<class KeyType> const std::string MinimizerIndex<KeyType>::EXTENSION = ".min";

//------------------------------------------------------------------------------

} // namespace gbwtgraph

#endif // GBWTGRAPH_MINIMIZER_H

