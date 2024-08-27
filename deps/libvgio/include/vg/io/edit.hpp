#ifndef VG_IO_EDIT_HPP_INCLUDED
#define VG_IO_EDIT_HPP_INCLUDED

#include <vg/vg.pb.h>
#include <utility>

namespace vg {

namespace io {

using namespace std;

bool edit_is_match(const Edit& e);
bool edit_is_sub(const Edit& e);
bool edit_is_insertion(const Edit& e);
bool edit_is_deletion(const Edit& e);
bool edit_is_empty(const Edit& e);
pair<Edit, Edit> cut_edit_at_to(const Edit& e, size_t to_off);
pair<Edit, Edit> cut_edit_at_from(const Edit& e, size_t from_off);
// Reverse an edit and reverse complement any embedded sequence
Edit reverse_complement_edit(const Edit& e);
bool operator==(const Edit& e1, const Edit& e2);

}
}

#endif
