#include <pugixml.hpp>
#include <sqlite3x-master/sqlite3x.hpp>
#include <iostream>
#include <string>
#include <vector>

#include <patentparse.h>

using namespace std;
using namespace sqlite3x; // because it uses prefixes


/**
 * Parse each of the XML files and insert them into the database
 */
int main(int argc, const char * argv[]) {

    // Arguments
    if (argc < 3) {
        cerr << "Usage: patent <db> <document> [document ..]" << endl;
        return 2;
    }
    
    // Database setup
    sqlite3_connection conn(argv[1]);
    conn.setbusytimeout(60000);
    conn.executenonquery("PRAGMA synchronous=0;");
    sqlite3_command insert_patent(conn, "INSERT OR REPLACE INTO patents (id, abstract, description, tags, title) VALUES (?, ?, ?, ?, ?);");
    sqlite3_command insert_log(conn, "INSERT INTO log (id, filename, success, message) VALUES (?, ?, ?, ?);");
    

    // Load each patent file
    for (int i=2; i<argc; i++) {
        PatentParse p(argv[i]);
        insert_log.bind(1, p.id);
        insert_log.bind(2, argv[i]);

        if(p.empty()) {
            cout << "Parse failed on " << argv[i] << endl;
            insert_log.bind(3, 0);
            insert_log.bind(4, p.error_log);
        } else {
            insert_patent.bind(1, p.id);
            insert_patent.bind(2, p.abstract);
            insert_patent.bind(3, p.description);
            insert_patent.bind(4, p.tags);
            insert_patent.bind(5, p.title);
            insert_patent.executenonquery();
            insert_log.bind(3, 1);
            insert_log.bind(4, "");
            cout << "Parse succeeded on " << argv[i] << endl;
        }
        insert_log.executenonquery();
    }
}
