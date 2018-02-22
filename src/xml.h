#ifndef XML_H
#define XML_H

#include "lib/xmlbare/parser.h"

void xmlBrowse( nodecPtr root, void (*callback)(nodecPtr, int, void *), void *);
char *getAttributeValue(nodecPtr node, char *attributeName);
float getAttributeValueAsFloat(nodecPtr node, char *attributeName);

void _addBrothersRec(nodecPtr **children, nodecPtr node, int nbChildren);

nodecPtr* getChildren(nodecPtr node);

void _addAttributesBrotherRec(attcPtr **attributes, attcPtr attr, int nbAttributes);

attcPtr* getAttributes(nodecPtr node);

void fprintfAttribute(FILE *f, attcPtr attr, int level);

void fprintfNodeRec(FILE *f, nodecPtr node, int level);

void fprintfNode(FILE *f, struct nodec *node);

void testXmlBare(char* text);

nodecPtr xmlParse(char* text);

void xmlBrowseCallbackMzdbParamTree(nodecPtr node, int level, void *userParam);


#endif /* XML_H */
