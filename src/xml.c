#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include "xml.h"
#include "models.h"


/* fill the children array */
void _addBrothersRec(nodecPtr **children, nodecPtr node, int nbChildren)
{
  if (!node->next)
    {
      return;
    }
  (*children)[nbChildren] = node->next;
  _addBrothersRec(children, node->next, nbChildren + 1);
}

/* get an array containing all children (and allocate array memory to be freed)*/
nodecPtr* getChildren(nodecPtr node)
{
  nodecPtr *children;

  if (!node || !node->numchildren)
    return NULL;
  children = (nodecPtr*)malloc(sizeof(nodecPtr) * node->numchildren);   // allocate children
  children[0] = node->firstchild;
  _addBrothersRec(&children, node->firstchild, 1);
  return children;
}

/* fill the attributes array */
void _addAttributesBrotherRec(attcPtr **attributes, attcPtr attr, int nbAttributes)
{
  if (!attr->next)
    {
      return;
    }
  (*attributes)[nbAttributes] = attr->next;
  _addAttributesBrotherRec(attributes, attr->next, nbAttributes + 1);
}

/* get an array containing all attributes (and allocate array memory to be freed)*/
attcPtr* getAttributes(nodecPtr node)
{
  attcPtr *attributes;

  if (!node || !node->numatt)
    return NULL;
  attributes = (attcPtr*)malloc(sizeof(attcPtr) * node->numatt);   // allocate attributes
  attributes[0] = node->firstatt;
  _addAttributesBrotherRec(&attributes, node->firstatt, 1);
  return attributes;
}

void fprintfAttribute(FILE *f, attcPtr attr, int level)
{
  fprintf(f, "%.*s************Attribute*******\n", level * 2, "                                                                    ");
  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "name : '%.*s'\n", attr->namelen, attr->name);
  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "value : '%.*s'\n", attr->vallen, attr->value);
}

void fprintfNodeRec(FILE *f, nodecPtr node, int level)
{
  int n = 0;
  nodecPtr *children;
  attcPtr *attributes;

  if (!node)
    return;

  fprintf(f, "%.*s------------------------------------------------------------------\n", level * 2, "                                                                    ");
  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "name : '%.*s'\n", node->namelen, node->name);
  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "comment : '%.*s'\n", node->comlen, node->comment);
  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "curchild : %X\n", node->curchild);

  //    fprintf(f,"%.*s", level*2, "                                                                    ");
  //    fprintf(f,"firstchild : %X   lastchild : %X\n", node->firstchild, node->lastchild);
  //    fprintf(f,"diff : %ld   struct : %d\n", ((long)( node->lastchild))- ((long)(node->firstchild)), sizeof(struct nodec));

  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "nb attrs : %d\n", node->numatt);

  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "nb vals : %d\n", node->numvals);

  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "value : '%.*s'\n", node->vallen, node->value);

  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "nb children : %d\n", node->numchildren);

  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "nb comments : %d\n", node->numcoms);

  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "next : %X\n", node->next);


  attributes = getAttributes(node);
  for (n = 0; n < node->numatt; n++)
    {
      fprintfAttribute(f, attributes[n], level);
    }
  free(attributes);

  children = getChildren(node);
  for (n = 0; n < node->numchildren; n++)
    {
      fprintfNodeRec(f, children[n], level + 1);
    }
  free(children);
}

void fprintfNode(FILE *f, struct nodec *node)
{
  fprintfNodeRec(f, node, 0);
}

//char* text= "<CATALOG><CD><TITLE>Empire Burlesque</TITLE><ARTIST>Bob Dylan</ARTIST><COUNTRY>USA</COUNTRY>";

///TODO: change name (display ?)
void testXmlBare(char* text)
{
  struct parserc *parser = (struct parserc *)calloc(1, sizeof(struct parserc));
  int err;

  err = parserc_parse(parser, text);

  fprintfNode(stdout, parser->rootnode);

  /* Below printfs are only temp vars
     printf( "attname: %s\n", parser->attname );
     printf( "attname len: %d\n", parser->attname_len );
     printf( "attval: %s\n", parser->attval );
     printf( "attval len: %d\n", parser->attval_len );
     printf( "att_has val: %d\n", parser->att_has_val );
     //printf( "curatt: %d\n", parser->curatt );
     //printf( "curnode: %d\n", parser->curnode );
     printf( "err: %d\n", parser->err );
     printf( "last state: %d\n", parser->last_state );
     //printf( "rootnode: %d\n", parser->rootnode );
     printf( "rootpos: %s\n", parser->rootpos );
     printf( "tagname: %s\n", parser->tagname );
     printf( "tagname len: %d\n", parser->tagname_len );
     //printf( "last state: %s\n", parser->rootnode-> );
   */

  free(parser);
}

nodecPtr xmlParse(char* text)
{
  struct parserc *parser = (struct parserc *)calloc(1, sizeof(struct parserc));
  nodecPtr root = NULL;
  int err;

  err = parserc_parse(parser, text);
  //TODO: Check error code
  if (parser == NULL)
    {
      printf("error encountered on parser : %d\n", err);
      return NULL;
    }


  root = parser->rootnode;
  free(parser);
  return root;
}

void xmlBrowseCallbackMzdbParamTree(nodecPtr node, int level, void *userParam)
{
  float value;
  char *result;
  char * attributeValue;

  MzdbParamTree *mzdbParamTree = (MzdbParamTree*)userParam;

  if (!strncmp(node->name, "userParam", node->namelen))
    {
      attributeValue = getAttributeValue(node, "name");    // ms1_bb_mz_width
      if (attributeValue && !strcmp(attributeValue, "ms1_bb_mz_width"))
        {
          value = getAttributeValueAsFloat(node, "value");
          //printf("Attribute Value ms1_bb_mz_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
          mzdbParamTree->ms1BbMzWidth = getAttributeValueAsFloat(node, "value");
        }
      else
        {
          if (attributeValue && !strcmp(attributeValue, "msn_bb_mz_width"))
            {
              value = getAttributeValueAsFloat(node, "value");
              //printf("Attribute Value msn_bb_mz_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
              mzdbParamTree->msnBbMzWidth = getAttributeValueAsFloat(node, "value");
            }
          else
            {
              if (attributeValue && !strcmp(attributeValue, "ms1_bb_time_width"))
                {
                  value = getAttributeValueAsFloat(node, "value");
                  //printf("Attribute Value ms1_bb_time_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
                  mzdbParamTree->ms1BbTimeWidth = getAttributeValueAsFloat(node, "value");
                }
              else
                {
                  if (attributeValue && !strcmp(attributeValue, "msn_bb_time_width"))
                    {
                      value = getAttributeValueAsFloat(node, "value");
                      // printf("Attribute Value msn_bb_time_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
                      mzdbParamTree->msnBbTimeWidth = getAttributeValueAsFloat(node, "value");
                    }
                  else
                    {
                      if (attributeValue && !strcmp(attributeValue, "is_lossless"))
                        {
                          //printf("Attribute Value isLossLess = %s\n", getAttributeValue(node,"value"));
                          result = getAttributeValue(node, "value");
                          if (strcmp(result, "true") == 0)
                            {
                              mzdbParamTree->isLossless = 1;
                            }
                          else
                            {
                              mzdbParamTree->isLossless = 0;
                            }
                        }
                      else
                        {
                          if (attributeValue && !strcmp(attributeValue, "origin_file_format"))
                            {
                              //printf("Attribute Value originFileFormat = %s\n", getAttributeValue(node,"value"));
                              mzdbParamTree->originFileFormat = getAttributeValue(node, "value");
                            }
                        }
                    }
                }
            }
        }
    }
}
