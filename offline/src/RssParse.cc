#include "RssParse.h"
#include <boost/regex.hpp>
#include <iostream>
#include <sstream>
using std::cout;
using std::endl;
using std::ostringstream;

namespace wd {

RssReader::RssReader(vector<string>& files) 
: _files(files) { cout << "RssReader()" << endl; }

RssReader::~RssReader() { cout << " ~RssReader()" << endl; }

void RssReader::loadFiles() {
    for (auto& path : _files) {
        loadXML(path);
    }
}

void RssReader::loadXML(const string& xmlPath) {
    XMLDocument doc;
    doc.LoadFile(xmlPath.c_str());
    if (doc.ErrorID()) {
        cout << "tinyxml load file error!" << endl;
        return;
    }
    parseRss(doc);
}

void RssReader::parseRss(XMLDocument& doc) {
    XMLElement* root = doc.RootElement();
    XMLElement* channel = root->FirstChildElement("channel");
    XMLElement* item = channel->FirstChildElement("item");

    boost::regex reg("<[^>]*>");
    do {
        RssItem tmp;
        XMLElement* pNode = item->FirstChildElement("title");
        tmp.title = pNode->GetText();
        pNode = item->FirstChildElement("link");
        tmp.link = pNode->GetText();
        pNode = item->FirstChildElement("description");
        const char* descrip = pNode->GetText();
        pNode = item->FirstChildElement("content:encoded");
        const char* content = pNode->GetText();
        if (content) {
            tmp.content = content;
        } else {
            tmp.content = descrip;
        }
        tmp.content = boost::regex_replace(
            tmp.content, reg, "", boost::match_default | boost::format_all);
        _rssItems.push_back(tmp);
    } while (item = item->NextSiblingElement());
}

void RssReader::makePages(vector<string>& pages) {
    int i = 0;
    for (auto& item : _rssItems) {
        ostringstream oss;
        oss << "<doc>" << '\n'
            << '\t' << "<docid>" << ++i << "</docid>" << '\n'
            << '\t' << "<title>" << item.title << "</title>" << '\n'
            << '\t' << "<link>" << item.link << "</link>" << '\n'
            << '\t' << "<content>" << item.content << "</content>" << '\n'
            << "</doc>" << '\n';
        pages.push_back(oss.str());
    }
}

}  // namespace wd