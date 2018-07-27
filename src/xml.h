#ifndef XML_H
#define XML_H

#include "lib/xmlbare/parser.h"

typedef struct nodec * nodec_ptr_t;
typedef struct attc * attc_ptr_t;

/**
 * @brief xml_browse_rec recursive xml traversal with callback on all nodes
 * @param node the node
 * @param level the current level
 * @param user_param the user param
 */
void xml_browse_rec(nodec_ptr_t node, int level, void (* callback) (nodec_ptr_t node, int level, void *), void* user_param);

/**
 * @brief xml_browse xml traversal with callback on all nodes
 * @param root the tree root
 * @param user_param the user param
 */
void xml_browse(nodec_ptr_t root, void (* callback) (nodec_ptr_t node, int level, void *), void* user_param);

/**
 * @brief get_attribute_value_rec get the attribute value matching attributeName
 * @param attribute the attribute
 * @param attribute_name the attribute name
 * @param attribute_value the attribute value
 * @return the attribute value
 */
int get_attribute_value_rec(attc_ptr_t attribute, char* attribute_name, char attribute_value[2048]);

/**
 * @brief get_attribute_value get the attribute value matching attributeName.
 * @param node the node
 * @param attribute_name the attribute name
 * @return Static char returned: Do not need to free memory
 */
char* get_attribute_value(nodec_ptr_t node, char* attribute_name);

/**
 * @brief get_attribute_value_as_float get the attribute value matching attributeName as float
 * @param node the node
 * @param attribute_name the attribute name
 * @param result the float result
 * @return 0 if no error
 */
int get_attribute_value_as_float(nodec_ptr_t node, char* attribute_name, float* result);

/**
 * @brief get_attribute_value_as_int  get the attribute value matching attributeName as int
 * @param node the node
 * @param attribute_name the attribute name
 * @param result the integer result
 * @return 0 if no error
 */
int get_attribute_value_as_int(nodec_ptr_t node, char* attribute_name, int* result);

/**
 * @brief _add_brothers_rec fill the children array
 * @param children the children array
 * @param node the node
 * @param children_count the number of child
 */
void _add_brothers_rec(nodec_ptr_t** children, nodec_ptr_t node, int children_count);

/**
 * @brief get_children  get an array containing all children
 * @param node the node
 * @return an array of child (allocate array memory to be freed)
 */
nodec_ptr_t* get_children(nodec_ptr_t node);

/**
 * @brief _add_attributes_brother_rec fill the attributes array
 * @param attributes the attributes array
 * @param attr the attribute
 * @param attribute_count the number of attribute
 */
void _add_attributes_brother_rec(attc_ptr_t** attributes, attc_ptr_t attr, int attribute_count);

/**
 * @brief get_attribute get an array containing all attributes
 * @param node the node
 * @return the attribute array (allocate array memory to be freed)
 */
attc_ptr_t* get_attribute(nodec_ptr_t node);

/**
 * @brief fprintf_attribute print an attribute
 * @param f the file
 * @param attr the attribute
 * @param level the level
 */
void fprintf_attribute(FILE* f, attc_ptr_t attr, int level);

/**
 * @brief fprintf_node_rec print each node
 * @param f the file
 * @param node the root
 * @param level the level
 */
void fprintf_node_rec(FILE *f, nodec_ptr_t node, int level);

/**
 * @brief fprintf_node print each node
 * @param f the file
 * @param node the node
 */
void fprintf_node (FILE* f, nodec_ptr_t node);

/**
 * @brief print_xml_bare print the xml bare
 * @param text the text
 */
void print_xml_bare(char* text);

/**
TODO: change it
* @brief xml_parse parse a xml
* @param text the text
* @return the xml root
*/
int xml_parse(char* text, nodec_ptr_t * rootnode);

/**
 * @brief xml_browse_callback_mazdb_param_tree browse a xml param tree
 * @param node the node
 * @param user_param the user param
 */
void xml_browse_callback_mzdb_param_tree(nodec_ptr_t node, int level, void* user_param);

#endif /* XML_H */
