#include "vcfheader.hpp"

using namespace std;
namespace vcfh{

Header::Header(){
    my_header_fields.set_fields = {"CHROM", "POS", "ID", "REF", "ALT", "QUAL", "FILTER", "INFO"};
    my_reference = "";
    my_source = "";
    my_contig = "";
    my_date = "";
    my_version = "";
}

Header::~Header(){

}

void Header::set_reference(string ref_file){
    my_reference = ref_file;
}

void Header::set_date(){
    string d = make_date();
    my_date = d;
}

string Header::make_date(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer [20];
    strftime(buffer, 20, "%G%m%d", ltm);
    return string(buffer);
}

void Header::set_source(string s){
    my_source = s;
}

void Header::set_contig(string c){
    my_contig = c;
}

void Header::set_version(string v){
    my_version = v;
}

void Header::Header::add_format(vcfh_format f){
    my_formats.push_back(f);
}

void Header::add_info(vcfh_info i){
    my_infos.push_back(i);
}

void Header::add_field(string f){
    my_header_fields.opt_fields.push_back(f);
}

void Header::add_filter(vcfh_filter f){
    my_filters.push_back(f);
}

string Header::get_fields() const{
    string ret;
    ret = "#";
    int i;
    for (i=0; i < my_header_fields.set_fields.size(); i++){
        ret += my_header_fields.set_fields[i] + "\t";
    }
    for (i=0; i < my_header_fields.opt_fields.size(); i++){
        ret += my_header_fields.opt_fields[i] + "\t";
    }
    return ret;
}

string Header::to_string() const{
    // print format (version), date, source and reference.
    string ret;
    ret = string("##fileformat=") + my_version + "\n";
    ret += "##fileDate=" + my_date + "\n";
    ret += "##source=" + my_source + "\n";
    ret += "##reference=" + my_reference + "\n";
    ret += "##contig=" + my_contig + "\n";

    //print info fields
    int i;
    for (i = 0; i < my_infos.size(); i++){
        ret += string("##INFO=<") + "ID=" + my_infos[i].id + ",Number=" + my_infos[i].number + ",Type=" + my_infos[i].type + ",Description=" + my_infos[i].description + ">\n";
    }

    // print filter fields
    for (i = 0; i < my_filters.size(); i++){
        ret += string("##FILTER=<") + "ID=" + my_filters[i].id + ",Number=" + my_filters[i].number + ",Type=" + my_filters[i].type + ",Description=" + my_filters[i].description + ">\n";
    }
    
    //print format fields
    for (i = 0; i < my_formats.size(); i++){
        ret += string("##FORMAT=<") + "ID=" + my_formats[i].id + ",Number=" + my_formats[i].number + ",Type=" + my_formats[i].type + ",Description=" + my_formats[i].description + ">\n";
    }

    ret+=get_fields();
    return ret;
}

std::ostream& operator<<(std::ostream& os, const vcfh::Header& h){
    os << h.to_string(); 
    return os;
}
}
