#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <ostream>
#include <iostream>

namespace vcfh{
    struct vcfh_info{
        std::string id;
        std::string number;
        std::string type;
        std::string description;
    };

    struct vcfh_format{
        std::string id;
        std::string number;
        std::string type;
        std::string description;
    };

    struct vcfh_filter{
        std::string id;
        std::string number;
        std::string type;
        std::string description;
    };

    struct vcfh_fields{
        std::vector<std::string> set_fields;
        std::vector<std::string> opt_fields;
    };


    class Header{

        public:
            Header();
            ~Header();
            void set_reference(std::string ref_file);
            void set_date();
            void set_source(std::string s);
            void set_contig(std::string contig);
            void add_info(vcfh_info info);
            void set_version(std::string version);
            void add_filter(vcfh_filter filter);
            void add_format(vcfh_format format);
            void add_field(std::string field);
            std::string to_string() const;

        private:    
            std::string get_fields() const;
            std::string make_date();
            vcfh_fields my_header_fields;
            std::string my_reference;
            std::string my_date;
            std::string my_source;
            std::string my_version;
            std::string my_contig;
            std::vector<vcfh_info> my_infos;
            std::vector<vcfh_format> my_formats;
            std::vector<vcfh_filter> my_filters;
            std::string compose_filters();
            std::string compose_infos();
            std::string compse_formats();
            std::string compose_fields();
    };

    std::ostream& operator<<(std::ostream& os, const Header& h);
};
