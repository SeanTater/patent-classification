#include <pugixml.hpp>
#include <sqlite3x-master/sqlite3x.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <vector>
#include <boost/program_options.hpp>

#include <patentparse.hpp>

using namespace std;
using namespace sqlite3x; // because it uses prefixes
namespace po = boost::program_options;


/**
 * Parse each of the XML files and insert them into the database
 */
int main(int argc, const char * argv[]) {

    // Option Management

    po::options_description visible("Options");
    visible.add_options()
            ("help", "produce help message")
            ("database,d", po::value<string>()->default_value("patent.db")->value_name("filename"), "Patent sqlite database");

    po::options_description actual;
    actual.add(visible).add_options()
            ("command", po::value< string >(), "Run mode")
            ("input-xml", po::value< vector<string> >(), "Input XML files, from CLEF IP");


    po::positional_options_description p;
    p.add("command", 1);
    p.add("input-xml", -1);


    po::variables_map varmap;
    po::store(po::command_line_parser(argc, argv).
            options(actual).positional(p).run(), varmap);
    po::notify(varmap);


    if (varmap.count("help")) {
        cerr << "Usage: patent store <document> [document ..]" << endl
             << visible << endl;
        return 1;
    }

    if (not (varmap.count("command") && varmap["command"].as<string>() == "store") ) {
        cerr << "Invalid command. Available commands: 'store'" << endl
             << "Usage: patent store <document> [document ..]" << endl
             << visible << endl;
        return 1;
    }

    if (not (varmap.count("input-xml"))) {
        cerr << "Missing input files." << endl
             << "Usage: patent store <document> [document ..]" << endl
             << visible << endl;
        return 1;
    }

    
    // Database setup
    sqlite3_connection conn(varmap["database"].as<string>().c_str());
    conn.setbusytimeout(60000);
    conn.executenonquery("PRAGMA synchronous=0;");
    sqlite3_command insert_patent(conn, "INSERT OR REPLACE INTO patents (id, abstract, description, tags, title) VALUES (?, ?, ?, ?, ?);");
    sqlite3_command insert_log(conn, "INSERT INTO log (id, filename, success, message) VALUES (?, ?, ?, ?);");
    

    // Load each patent file
    for (string filename : varmap["input-xml"].as< vector<string> >()) {
        PatentParse p(filename);
        insert_log.bind(1, p.id);
        insert_log.bind(2, filename);

        if(p.empty()) {
            insert_log.bind(3, 0);
            insert_log.bind(4, p.error_log);
            cout << "Parse failed on " << filename << endl;
        } else {
            insert_patent.bind(1, p.id);
            insert_patent.bind(2, p.abstract);
            insert_patent.bind(3, p.description);
            insert_patent.bind(4, p.tags);
            insert_patent.bind(5, p.title);
            insert_patent.executenonquery();
            insert_log.bind(3, 1);
            insert_log.bind(4, "");
            cout << "Parse succeeded on " << filename << endl;
        }
        insert_log.executenonquery();
    }
}
