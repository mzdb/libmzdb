#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "asprintf.h"
#include "log.h"
#include "models.h"
#include "xml.h"
#include "util.h"

void xml_browse_rec(nodec_ptr_t node, int level, void (* callback) (nodec_ptr_t node, int level, void *), void* user_param)
{
  if(node&&callback) callback(node, level, user_param);
  if(node->next) xml_browse_rec(node->next, level, callback, user_param);
  if(node->numchildren) xml_browse_rec(node->firstchild, level+1, callback, user_param );
}

void xml_browse(nodec_ptr_t root, void (* callback) (nodec_ptr_t node, int level, void *), void* user_param)
{
  if(root) xml_browse_rec(root, 0, callback, user_param);
}

int get_attribute_value_rec(attc_ptr_t attribute, char* attribute_name, char attribute_value[2048])
{
  if(!attribute) return 0;
  if(!strncmp(attribute_name, attribute->name, attribute->namelen))
  {
    strncpy(attribute_value, attribute->name, attribute->namelen);
    attribute_value[attribute->vallen]= 0;
    return 1;
  }
  return get_attribute_value_rec(attribute->next, attribute_name, attribute_value);
}

char* get_attribute_value(nodec_ptr_t node, char* attribute_name)
{
  static char attribute_value[2048];
  if (!node || !node->firstatt ) return NULL;
  if (get_attribute_value_rec(node->firstatt, attribute_name, attribute_value)) return attribute_value;
  return NULL;
}

int get_attribute_value_as_float(nodec_ptr_t node, char* attribute_name, float* result)
{
  char* attr = get_attribute_value(node, attribute_name);
  if(!attr) return -1;
  *result = atof(attr);
  return 0;
}

int get_attribute_value_as_int(nodec_ptr_t node, char* attribute_name, int* result)
{
  char* attr = get_attribute_value(node, attribute_name);
  if(!attr) return -1;
  *result = atoi(attr);
  return 0;
}

void _add_brothers_rec(nodec_ptr_t** children, nodec_ptr_t node, int children_count)
{
  if(node->next)
  {
    (*children)[children_count] = node->next;
    _add_brothers_rec(children, node->next, children_count+1);
  }
}

nodec_ptr_t* get_children(nodec_ptr_t node)
{
  if(!node || !node->numchildren) return NULL;
  nodec_ptr_t* children = (nodec_ptr_t*) malloc(sizeof(nodec_ptr_t)*node->numchildren);
  children[0] = node->firstchild;
  _add_brothers_rec(&children, node->firstchild, 1);
  return children;
}

void _add_attributes_brother_rec(attc_ptr_t** attributes, attc_ptr_t attr, int attribute_count)
{
  if(attr->next)
  {
    (*attributes)[attribute_count] = attr->next;
    _add_attributes_brother_rec(attributes, attr->next, attribute_count+1);
  }
}

attc_ptr_t* get_attribute(nodec_ptr_t node)
{
  if(!node || !node->numatt) return NULL;
  attc_ptr_t* attrs = (attc_ptr_t*) malloc(sizeof(attc_ptr_t)*node->numatt);
  attrs[0] = node->firstatt;
  _add_attributes_brother_rec(&attrs, node->firstatt, 1);
  return attrs;
}

void fprintf_attribute(FILE* f, attc_ptr_t attr, int level)
{
  fprintf(f, "%.*s************Attribute*******\n", level * 2, "                                                                    ");
  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "name : '%.*s'\n", attr->namelen, attr->name);
  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "value : '%.*s'\n", attr->vallen, attr->value);
}


void fprintf_node_rec(FILE *f, nodec_ptr_t node, int level)
{
  nodec_ptr_t *children;
  attc_ptr_t *attributes;
  if (node)
  {
    fprintf(f, "%.*s------------------------------------------------------------------\n", level * 2, "                                                                    ");
    fprintf(f, "%.*s", level * 2, "                                                                    ");
    fprintf(f, "name : '%.*s'\n", node->namelen, node->name);
    fprintf(f, "%.*s", level * 2, "                                                                    ");
    fprintf(f, "comment : '%.*s'\n", node->comlen, node->comment);
    fprintf(f, "%.*s", level * 2, "                                                                    ");
//    fprintf(f, "curchild : %X\n", node->curchild);

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

//    fprintf(f, "%.*s", level * 2, "                                                                    ");
//    fprintf(f, "next : %X\n", node->next);


    attributes = get_attribute(node);
    for (int n = 0; n < node->numatt; ++n) fprintf_attribute(f, attributes[n], level);
    free(attributes);

    children = get_children(node);
    for (int n = 0; n < node->numchildren; ++n) fprintf_node_rec(f, children[n], level+1);
    free(children);
  }
}

void fprintf_node (FILE* f, nodec_ptr_t node)
{
  fprintf_node_rec(f, node, 0);
}

void print_xml_bare(char* text)
{
  struct parserc parser;
  int err = parserc_parse(&parser, text); //err = 0 if no error
  if(!err) fprintf_node(stdout, parser.rootnode);
}


int xml_parse(char* text, nodec_ptr_t * rootnode)
{
  struct parserc parser;
  int err = parserc_parse(&parser, text);
  if(err)
  {
    flogf(LOG_ERROR, "something wrong happened (error code=%d) while parsing XML chunk '%s", err, text);
    return -1;
  }
  *rootnode = parser.rootnode;
  return 0;
}

void xml_browse_callback_mzdb_param_tree(nodec_ptr_t node, int level, void* user_param)
{
  libmzdb_mzdb_param_tree_t* tree = (libmzdb_mzdb_param_tree_t*) user_param;
  if (strncmp(node->name, "userParam", node->namelen)) return;

  char* value = get_attribute_value(node, "name");
  if (!value) return;

  if      (!strcmp(value, "ms1_bb_mz_width"))   tree->ms1_bb_mz_width     = atof(get_attribute_value(node, "value"));
  else if (!strcmp(value, "msn_bb_mz_width"))   tree->msn_bb_mz_width     = atof(get_attribute_value(node, "value"));
  else if (!strcmp(value, "ms1_bb_time_width")) tree->ms1_bb_time_width   = atof(get_attribute_value(node, "value"));
  else if (!strcmp(value, "msn_bb_time_width")) tree->msn_bb_time_width   = atof(get_attribute_value(node, "value"));
  else if (!strcmp(value, "is_lossless"))       tree->is_loss_less       = (atob(get_attribute_value(node, "value"))) ? 1 : 0; //see utils.h
  else if (!strcmp(value, "origin_file_format"))tree->origin_file_format = get_attribute_value(node, "value");
}

























































///** recursive xml traversal with callback on all nodes */
//static void xmlBrowseRec( nodecPtr node, int level, void (*callback)(nodecPtr node, int level, void *), void *userParam) {
////    printf("browse node %.*s\n", node->namelen, node->name);   fflush(stdout);

//    if ( node && callback ) callback( node, level, userParam);
//    if ( node->next ) {
//        xmlBrowseRec( node->next, level, callback, userParam);
//    }
//    if ( node->numchildren) {
//        xmlBrowseRec(node->firstchild,level+1, callback, userParam);
//    }
//}


///** xml trversal with callback on all nodes */
//void xmlBrowse( nodecPtr root, void (*callback)(nodecPtr, int, void *), void *userParam) {
//    if ( !root ) return;
//    xmlBrowseRec( root, 0, callback, userParam);
//}

///** get the attribute value matching attributeName */
//static int getAttributeValueRec(attcPtr attribute, char *attributeName, char attributeValue[2048]) {
//    if ( !attribute) return 0;

//    if ( !strncmp(attributeName, attribute->name, attribute->namelen) ) {
//        strncpy(attributeValue,attribute->value, attribute->vallen);
//        attributeValue[attribute->vallen] = 0;
//        return 1;
//    }
//    return getAttributeValueRec(attribute->next, attributeName, attributeValue);
//}

///** get the attribute value matching attributeName. Static char returned: Do not need to free memory */
//char *getAttributeValue(nodecPtr node, char *attributeName) {
//    static char attributeValue[2048];
//    if ( !node ) return NULL;
//    if ( !node->firstatt )  return NULL;
//    if ( getAttributeValueRec(node->firstatt, attributeName, attributeValue) ) return attributeValue;
//    return NULL;
//}

///** get the attribute value matching attributeName as float */
////TODO: gestion d'erreurs ...
//float getAttributeValueAsFloat(nodecPtr node, char *attributeName) {
//    char *attributeValue = getAttributeValue(node, attributeName);
//    if (attributeValue) return atof( attributeValue );
//}

//int getAttributeAsInt(nodecPtr node, char *attributeName){
//    char *attributeValue = getAttributeValue(node, attributeName);
//    if(attributeValue) return atoi( attributeValue );
//}

///* fill the children array */
//void _addBrothersRec(nodecPtr **children, nodecPtr node, int nbChildren)
//{
//  if (!node->next)
//    {
//      return;
//    }
//  (*children)[nbChildren] = node->next;
//  _addBrothersRec(children, node->next, nbChildren + 1);
//}

///* get an array containing all children (and allocate array memory to be freed)*/
//nodecPtr* getChildren(nodecPtr node)
//{
//  nodecPtr *children;

//  if (!node || !node->numchildren)
//    return NULL;
//  children = (nodecPtr*)malloc(sizeof(nodecPtr) * node->numchildren);   // allocate children
//  children[0] = node->firstchild;
//  _addBrothersRec(&children, node->firstchild, 1);
//  return children;
//}

///* fill the attributes array */
//void _addAttributesBrotherRec(attcPtr **attributes, attcPtr attr, int nbAttributes)
//{
//  if (!attr->next)
//    {
//      return;
//    }
//  (*attributes)[nbAttributes] = attr->next;
//  _addAttributesBrotherRec(attributes, attr->next, nbAttributes + 1);
//}

///* get an array containing all attributes (and allocate array memory to be freed)*/
//attcPtr* getAttributes(nodecPtr node)
//{
//  attcPtr *attributes;

//  if (!node || !node->numatt)
//    return NULL;
//  attributes = (attcPtr*)malloc(sizeof(attcPtr) * node->numatt);   // allocate attributes
//  attributes[0] = node->firstatt;
//  _addAttributesBrotherRec(&attributes, node->firstatt, 1);
//  return attributes;
//}






//void fprintfAttribute(FILE *f, attcPtr attr, int level)
//{
//  fprintf(f, "%.*s************Attribute*******\n", level * 2, "                                                                    ");
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "name : '%.*s'\n", attr->namelen, attr->name);
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "value : '%.*s'\n", attr->vallen, attr->value);
//}

//void fprintfNodeRec(FILE *f, nodecPtr node, int level)
//{
//  int n = 0;
//  nodecPtr *children;
//  attcPtr *attributes;

//  if (!node)
//    return;

//  fprintf(f, "%.*s------------------------------------------------------------------\n", level * 2, "                                                                    ");
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "name : '%.*s'\n", node->namelen, node->name);
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "comment : '%.*s'\n", node->comlen, node->comment);
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "curchild : %X\n", node->curchild);

//  //    fprintf(f,"%.*s", level*2, "                                                                    ");
//  //    fprintf(f,"firstchild : %X   lastchild : %X\n", node->firstchild, node->lastchild);
//  //    fprintf(f,"diff : %ld   struct : %d\n", ((long)( node->lastchild))- ((long)(node->firstchild)), sizeof(struct nodec));

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "nb attrs : %d\n", node->numatt);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "nb vals : %d\n", node->numvals);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "value : '%.*s'\n", node->vallen, node->value);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "nb children : %d\n", node->numchildren);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "nb comments : %d\n", node->numcoms);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "next : %X\n", node->next);


//  attributes = getAttributes(node);
//  for (n = 0; n < node->numatt; n++)
//    {
//      fprintfAttribute(f, attributes[n], level);
//    }
//  free(attributes);

//  children = getChildren(node);
//  for (n = 0; n < node->numchildren; n++)
//    {
//      fprintfNodeRec(f, children[n], level + 1);
//    }
//  free(children);
//}



//void fprintfNode(FILE *f, struct nodec *node)
//{
//  fprintfNodeRec(f, node, 0);
//}

////char* text= "<CATALOG><CD><TITLE>Empire Burlesque</TITLE><ARTIST>Bob Dylan</ARTIST><COUNTRY>USA</COUNTRY>";

/////TODO: change name (display ?)
//void testXmlBare(char* text)
//{
//  struct parserc *parser = (struct parserc *)calloc(1, sizeof(struct parserc));
//  int err;

//  err = parserc_parse(parser, text);

//  fprintfNode(stdout, parser->rootnode);

//  /* Below printfs are only temp vars
//     printf( "attname: %s\n", parser->attname );
//     printf( "attname len: %d\n", parser->attname_len );
//     printf( "attval: %s\n", parser->attval );
//     printf( "attval len: %d\n", parser->attval_len );
//     printf( "att_has val: %d\n", parser->att_has_val );
//     //printf( "curatt: %d\n", parser->curatt );
//     //printf( "curnode: %d\n", parser->curnode );
//     printf( "err: %d\n", parser->err );
//     printf( "last state: %d\n", parser->last_state );
//     //printf( "rootnode: %d\n", parser->rootnode );
//     printf( "rootpos: %s\n", parser->rootpos );
//     printf( "tagname: %s\n", parser->tagname );
//     printf( "tagname len: %d\n", parser->tagname_len );
//     //printf( "last state: %s\n", parser->rootnode-> );
//   */

//  free(parser);
//}



//nodecPtr xmlParse(char* text)
//{
//  struct parserc *parser = (struct parserc *)calloc(1, sizeof(struct parserc));
//  nodecPtr root = NULL;
//  int errCode;

//  errCode = parserc_parse(parser, text);
//  //TODO: Check error code
//  if (parser == NULL)
//    {
//      _logf(LOG_ERROR,"something wrong happened (error code=%d) while parsing XML chunk '%s'", errCode, text);

//      return NULL;
//    }


//  root = parser->rootnode;
//  free(parser);
//  return root;
//}

//void xmlBrowseCallbackMzdbParamTree(nodecPtr node, int level, void *userParam)
//{
//  float value;
//  char *result;
//  char * attributeValue;

//  MzdbParamTree *mzdbParamTree = (MzdbParamTree*)userParam;

//  if (!strncmp(node->name, "userParam", node->namelen))
//    {
//      attributeValue = getAttributeValue(node, "name");    // ms1_bb_mz_width
//      if (attributeValue && !strcmp(attributeValue, "ms1_bb_mz_width"))
//        {
//          value = getAttributeValueAsFloat(node, "value");
//          //printf("Attribute Value ms1_bb_mz_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
//          mzdbParamTree->ms1BbMzWidth = getAttributeValueAsFloat(node, "value");
//        }
//      else
//        {
//          if (attributeValue && !strcmp(attributeValue, "msn_bb_mz_width"))
//            {
//              value = getAttributeValueAsFloat(node, "value");
//              //printf("Attribute Value msn_bb_mz_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
//              mzdbParamTree->msnBbMzWidth = getAttributeValueAsFloat(node, "value");
//            }
//          else
//            {
//              if (attributeValue && !strcmp(attributeValue, "ms1_bb_time_width"))
//                {
//                  value = getAttributeValueAsFloat(node, "value");
//                  //printf("Attribute Value ms1_bb_time_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
//                  mzdbParamTree->ms1BbTimeWidth = getAttributeValueAsFloat(node, "value");
//                }
//              else
//                {
//                  if (attributeValue && !strcmp(attributeValue, "msn_bb_time_width"))
//                    {
//                      value = getAttributeValueAsFloat(node, "value");
//                      // printf("Attribute Value msn_bb_time_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
//                      mzdbParamTree->msnBbTimeWidth = getAttributeValueAsFloat(node, "value");
//                    }
//                  else
//                    {
//                      if (attributeValue && !strcmp(attributeValue, "is_lossless"))
//                        {
//                          //printf("Attribute Value isLossLess = %s\n", getAttributeValue(node,"value"));
//                          result = getAttributeValue(node, "value");
//                          if (strcmp(result, "true") == 0)
//                            {
//                              mzdbParamTree->isLossless = 1;
//                            }
//                          else
//                            {
//                              mzdbParamTree->isLossless = 0;
//                            }
//                        }
//                      else
//                        {
//                          if (attributeValue && !strcmp(attributeValue, "origin_file_format"))
//                            {
//                              //printf("Attribute Value originFileFormat = %s\n", getAttributeValue(node,"value"));
//                              mzdbParamTree->originFileFormat = getAttributeValue(node, "value");
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//}











