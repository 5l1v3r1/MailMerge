#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <string.h>

void retrieveDataById(xmlNode *a_node, xmlChar * studentID);
void createOutputFile(xmlChar * studentID, xmlChar * data);
char * concat(char * s1, char * s2);

xmlDocPtr docVar = NULL;
xmlDocPtr docTemplate = NULL;
char * OUTPUT_FILE_PATH = NULL;
char * startXpathQuery = "//"; /* Selects nodes in the document from the current node that match the selection no matter where they are */

xmlDocPtr getDoc (const char * docname) {
    xmlDocPtr doc;
    doc = xmlReadFile(docname, NULL, XML_PARSE_NOBLANKS);
    if (doc == NULL ) {
        fprintf(stderr,"Document not parsed successfully. \n");
        return NULL;
    }
    return doc;
}

xmlXPathObjectPtr getNodeSet (xmlDocPtr doc,
                              xmlChar * xpath){
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    context = xmlXPathNewContext(doc);
    if (context == NULL) {
        printf("Error in xmlXPathNewContext\n");
        return NULL;
    }
    result = xmlXPathEvalExpression(xpath, context);
    xmlXPathFreeContext(context);
    if (result == NULL) {
        printf("Error in xmlXPathEvalExpression\n");
        return NULL;
    }
    if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
        xmlXPathFreeObject(result);
        //printf("No result\n");
        return NULL;
    }
    return result;
}

xmlNodeSetPtr evalXPath(xmlDocPtr doc, xmlChar * xpath){

    xmlXPathObjectPtr result = getNodeSet(doc, xpath);

    if (result) {
        xmlNodeSetPtr nodeset = result->nodesetval;
        return nodeset;
    }
    return NULL;
}

void fillTemplate(const char * TEMPLATE_FILENAME,
                  const char * VARIABLE_FILENAME,
                  char * OUTPUT_FILEPATH){

    docVar = getDoc(VARIABLE_FILENAME);
    docTemplate = getDoc(TEMPLATE_FILENAME);
    OUTPUT_FILE_PATH = OUTPUT_FILEPATH;

    xmlNode * root = xmlDocGetRootElement(docVar);
    while (root){ // iteration in order to find ID attribute!
        if (root->properties && (root->properties->name,"id")){
            startXpathQuery = concat(startXpathQuery, (char * )root->name); // determine name of node which contains id attribute
            // in example variable file is a student, but of course it might be person, employee etc.
            break;
        }
        root = root->children;
    }

    xmlNodeSetPtr nodeset = evalXPath(docVar, (xmlChar *) "//@id"); // Selects all attributes that are named id

    int i = 0;
    while (nodeset->nodeNr){ // number of nodes which contains id attribute
        xmlChar * studentID = xmlNodeListGetString(docVar, nodeset->nodeTab[i]->xmlChildrenNode, 1); //get ID
        retrieveDataById(xmlDocGetRootElement(docTemplate), studentID);
        nodeset->nodeNr--;
        i++;
    }

    xmlFreeDoc(docVar);
    xmlFreeDoc(docTemplate);
    xmlCleanupParser();

}
void retrieveDataById(xmlNode *a_node, xmlChar *studentID)
{
    xmlNode *cur_node = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {

        if (cur_node->type == XML_ELEMENT_NODE) {
            if (xmlStrcmp(cur_node->name, BAD_CAST "id") == 0)
                createOutputFile(studentID, studentID);
            else {

                char * xPathQuery = concat(startXpathQuery, "[@id = ");
                xPathQuery = concat(xPathQuery, (char *) studentID);
                xPathQuery = concat(xPathQuery, "]/");
                xPathQuery = concat(xPathQuery, (char *) cur_node->name);

                // in example variable file , xPathQuery correspond to ==>﻿//student[@id = 2008900]/name,surname etc.

                xmlNodeSetPtr nodeset = evalXPath(docVar, (xmlChar *) xPathQuery);
                xmlChar * keyword;
                if (nodeset) {
                    keyword = xmlNodeListGetString(docVar, nodeset->nodeTab[0]->xmlChildrenNode, 1);
                    createOutputFile(studentID, keyword);
                }
                free(xPathQuery);
                free(nodeset);
            }
        }
        else if (cur_node -> type == XML_TEXT_NODE)
            createOutputFile(studentID, cur_node->content);

        retrieveDataById(cur_node->children, studentID);
    }
}

void createOutputFile(xmlChar * studentID, xmlChar * data){

    char * filePath = OUTPUT_FILE_PATH;
    filePath = concat(filePath, "/"); // path of file to not have extra slash in the file path so i created local variable.
    char * fileName = concat(filePath,(char * ) studentID);
    fileName = concat(fileName,".txt");

    filePath = concat("mkdir -p ",OUTPUT_FILE_PATH);

    system(filePath); // system - execute a shell command

    FILE * txtFile = fopen(fileName, "a");
    if (txtFile){
        fputs((char *)data, txtFile);
        fclose(txtFile);
    }
    else
        fprintf(stderr, "Cannot create txt file! \n");

}

char* concat(char * s1, char * s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
