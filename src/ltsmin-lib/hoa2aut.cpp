

// hoa parser libraries
#include <cpphoafparser/parser/hoa_parser.hh>
#include <cpphoafparser/consumer/hoa_consumer_ltsmin.hh>

#include <iostream>
#include <fstream>
#include <string>

using namespace cpphoafparser;

extern "C" {
#include <hre/config.h>
#include <ltsmin-lib/hoa2aut.h>
}


ltsmin_buchi_t *
ltsmin_create_hoa(const char* hoa_file, ltsmin_parse_env_t env, lts_type_t ltstype)
{
    std::ifstream hoa_input(hoa_file, std::ifstream::in);
    HOAConsumerLTSmin *cons = new HOAConsumerLTSmin(env, ltstype);
    HOAConsumer::ptr consumer(cons);

    try {
        HOAParser::parse(hoa_input, consumer);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        Abort("Could not read %s", hoa_file);
    }

    return cons->get_ltsmin_buchi();
}


