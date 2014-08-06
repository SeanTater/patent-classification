#include "xmlpatentparser.hpp"

// Just useful for debugging:
//  cout << "name is " << root.name() << " and is " << (root.empty() ? "empty" : "not empty") << endl;

/*
 * Metadata
 */

XMLPatentParser::XMLPatentParser(shared_ptr<pugi::xml_document> doc, pugi::xml_node root, PatentDialect dialect) {
    this->doc = doc;
    this->root = root;
    this->dialect = dialect;

    if (dialect == PatentDialect::CLEF)
        pat.setId(root.attribute("ucid").value());
    else if (dialect == PatentDialect::GOOGLE)
        pat.setId(root.attribute("file").value());

    pat.setTitle(extractEnglish("invention-title", -1));
    pat.setAbstract(extractEnglish("abstract", -1));
    pat.setDescription(extractEnglish("description"));
    extractIPC();
    extractUSPC();
    extractECLA();
    pat.setClaims(extractClaims());

    pat.validate();
}

vector<Patent> XMLPatentParser::parseXML(string filename) {
    // Read an XML, split it into different patents
    // Make sure the doc outlives the patents
    auto doc = make_shared<pugi::xml_document>();
    doc->load_file(filename.c_str());

    // Start parsing: note Google and CLEF have different root tags
    vector<Patent> res;
    for (pugi::xml_node root : doc->children()) {
        if (string("patent-document") == root.name()) {
            res.emplace_back(XMLPatentParser(doc, root, PatentDialect::CLEF).getPatent());
        } else if (string("us-patent-grant") == root.name()) {
            res.emplace_back(XMLPatentParser(doc, root, PatentDialect::GOOGLE).getPatent());
        } else {
            //cerr << "Warning: encountered unknown patent type " << root.name() << endl;
        }
    }
    return res;
}

/*
 * Bibliographic data
 */

string XMLPatentParser::snippet(pugi::xml_node node, unsigned int target_length, const string separator) {
    string content = Patent::sanitize(node.value());
    if (not content.empty()) {
        // Add a space if it's not empty to keep lines apart
        content += separator;
    }
    if (content.length() >= target_length)
        return content;
    else
        for (pugi::xml_node child : node) {
            content += snippet(child, target_length - content.length(), separator);
            if (content.length() >= target_length) break;
        }
    return content;
}

string XMLPatentParser::extractEnglish(string target_tag_name, unsigned int length) {
    pugi::xml_node target = root.find_node([&](pugi::xml_node node){
        auto lang = node.attribute("lang");
        return node.name() == target_tag_name and (lang.empty() or string("EN") == lang.value());
    });
    return snippet(target, length);
}


string XMLPatentParser::extractClaims() {
    auto claims_parent = root.first_element_by_path("claims");
    string claims = "";
    for (auto claim_node : claims_parent.children("claim")) {
        string claim_part = "";
        for (auto claim_text_node : claim_node.children("claim-text")) {
            claim_part += claim_text_node.child_value() + string("\x1e");
        }
        if (not claim_part.empty()) {
            claims += claim_part + "\x1d";
        }
    }
    return claims;
}


/*
 * Classifications
 */

void XMLPatentParser::extractIPC() {
    // CLEF and Google have different XML tag names ans split the parts of the IPCR
    if (dialect == PatentDialect::CLEF) {
        auto tags_parent = root.first_element_by_path("bibliographic-data/technical-data/classifications-ipcr");
        for (auto tag_node : tags_parent.children()) {
            string tag = string(tag_node.child_value()).substr(0, 3);
            pat.appendClass(pat.ipc, tag);
        }
    } else if (dialect == PatentDialect::GOOGLE) {
        auto tags_parent = root.first_element_by_path("us-bibliographic-data-grant/classifications-ipcr");

        for (auto tag_node : tags_parent.children()) {
            string tag = tag_node.child("section").child_value();
            tag += tag_node.child("class").child_value();
            pat.appendClass(pat.ipc, tag);
        }
    }
}

void XMLPatentParser::extractUSPC()
{
    // CLEF doesn't have it.
    if (dialect == PatentDialect::GOOGLE) {
        auto tags_parent = root.first_element_by_path("us-bibliographic-data-grant/classifications-national");

        for (auto tag_node : tags_parent.children()) {
            if (tag_node.child("country").child_value() == string("US")) {
                pat.appendClass(pat.uspc, tag_node.child("main-classification").child_value());
            }
        }
    }
}

void XMLPatentParser::extractECLA()
{
    if (dialect == PatentDialect::CLEF) {
        auto tags_parent = root.first_element_by_path("bibliographic-data/technical-data/classification-ecla");
        for (auto tag_node : tags_parent.children("classification-symbol")) {
            pat.appendClass(pat.ecla, tag_node.child_value());
        }
    }
}

const Patent XMLPatentParser::getPatent() const {
    return pat;
}
