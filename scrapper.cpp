#include<iostream>
#include<string>
#include<vector>
#include<curl/curl.h>
#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"
using namespace std;

struct BlogPost
{
    string date;
    string title;
    string url;
};

string get_request(string url) {
    CURL *curl = curl_easy_init();
    string result;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t {
            string* response = static_cast<string*>(userdata);
            response->append(ptr, size * nmemb);
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << endl;
        }
        curl_easy_cleanup(curl);
    }
    return result;
}

xmlXPathObjectPtr get_nodes(xmlDocPtr doc, const xmlChar* xpath)
{
    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    if(!context)
    {
        cerr << "Failed to create XPath context" << endl;
        return nullptr;
    }
    xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
    xmlXPathFreeContext(context);
    return result;
}

int main() {
    curl_global_init(CURL_GLOBAL_ALL);
    //cout << "[DEBUG] curl initialized" << endl;
    string docStr = get_request("https://opensource.googleblog.com/search/label/gsoc");
    //cout << "[DEBUG] Fetched " << doc.size() << " bytes\n\n";
    htmlDocPtr htmldoc = htmlReadMemory(docStr.c_str(), docStr.size(),nullptr, nullptr, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING );
    curl_global_cleanup();
    if(!htmldoc)
    {
        cerr << "Failed to parse HTML" << endl;
        return 1;
    }

    xmlXPathObjectPtr dateNodes = get_nodes(htmldoc, BAD_CAST "//h4[@class='date-header']/span");
    xmlXPathObjectPtr postNodes = get_nodes(htmldoc, BAD_CAST "//h3[@class='post-title entry-title']/a");
    if(!dateNodes || !postNodes)
    {
        cerr << "XPath query fail" << endl;
        xmlFreeDoc(htmldoc);
        return 1;
    }
    vector<BlogPost> posts;

    int datecount = dateNodes->nodesetval->nodeNr;
    int postcount = postNodes->nodesetval->nodeNr;
    for(int i = 0; i < postcount; i++)
    {
        xmlNodePtr postNode = postNodes->nodesetval->nodeTab[i];
        string title = (const char*)xmlNodeGetContent(postNode);
        string url = (const char*)xmlGetProp(postNode, BAD_CAST "href");
        string date = "";
        if(i < datecount)
        {
            xmlNodePtr dateNode = dateNodes->nodesetval->nodeTab[i];
            date = (const char*)xmlNodeGetContent(dateNode);
        }

        posts.push_back({date, title, url});
    }
    cout << "[\n";
    for (size_t i = 0; i < posts.size(); ++i) {
        const auto& post = posts[i];
        cout << "  {\n";
        cout << "    \"date\": \"" << post.date << "\",\n";
        cout << "    \"title\": \"" << post.title << "\",\n";
        cout << "    \"url\": \"" << post.url << "\"\n";
        cout << "  }" << (i + 1 < posts.size() ? "," : "") << "\n";
    }
    cout << "]\n";

    xmlXPathFreeObject(dateNodes);
    xmlXPathFreeObject(postNodes);
    xmlFreeDoc(htmldoc);
    curl_global_cleanup();
    return 0;
}
