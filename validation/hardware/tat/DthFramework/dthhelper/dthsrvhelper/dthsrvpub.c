/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   9P interfaces for DTH server helper
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wchar.h>

#include "dthsrvhelper.h"
#include "dthsrvpub.h"

/* CONSTANTS */

#define MAX_MODULE_NAME             256
#define MAX_ELEMENT_PATH            256
#define MAX_FILEPATH                256

/* internal globals */

static int g_dthPublisherVerbose;	/* minimal verbose */
static int g_dthPublisherEnabled;	/* publisher not enabled by default */
static char g_module[MAX_MODULE_NAME];
static char g_group[MAX_ELEMENT_PATH];
static char g_action[MAX_ELEMENT_PATH];
static int g_groupDepth;
static char g_elementPath[MAX_ELEMENT_PATH];
static char g_filename[MAX_FILEPATH];
xmlChar *g_xmlConvStr[] = {NULL, NULL};

static xmlDocPtr g_xmlDocPtr;
static xmlNodePtr g_thisNode;

#define XMLSTR0(s) (_xmlConvStr(s, 0))
#define XMLSTR1(s) (_xmlConvStr(s, 1))

/* Implementation of PRIVATE functions */

static void _dthGetGroupName(const char *path, char *buffer, const int size)
{
	char copy_path[MAX_ELEMENT_PATH];
	int cnt_group;
	char *token = NULL;
	int group_len = 0;

	strncpy(copy_path, path, MAX_ELEMENT_PATH);
	copy_path[MAX_ELEMENT_PATH - 1] = 0;

	memset(buffer, 0, size);

	if (g_groupDepth > 0) {
		cnt_group = 0;
		/* first is a separator : /DTH/xxxx */
		token = strtok(copy_path, "/");

		while ((cnt_group < g_groupDepth) && (token != NULL)) {
			token = strtok(NULL, "/");
			cnt_group++;
		}

		/*fprintf(stdout, "element max depth = %i\n", cnt_group); */
		if (cnt_group >= g_groupDepth) {
			group_len = token - copy_path;
			/*fprintf(stdout, "group_len = %i\n", group_len); */
			if ((group_len > 0) && (group_len < MAX_ELEMENT_PATH)) {
				group_len--;

				strncpy(buffer, path, group_len);
				buffer[group_len] = 0;
				/*fprintf(stdout, "buffer=%s\n", buffer); */
			} else {
				fprintf(stderr,
					"warning: element '%s', \
					group length out of range!\n",
					path);
			}
		}
	}
}

static xmlXPathObjectPtr _xmlXPathNodeset(xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		fprintf(stderr, "Error in xmlXPathNewContext\n");
		return NULL;
	}

	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) {
		fprintf(stderr, "Error in xmlXPathEvalExpression\n");
		return NULL;
	}

	if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
		xmlXPathFreeObject(result);
		/*fprintf(stderr, "No result\n"); */
		return NULL;
	}
	return result;
}

static void _xmlRemoveNode(xmlNodePtr node)
{
	if (g_dthPublisherVerbose)
		fprintf(stdout, "Delete node %s\n", node->name);

	xmlUnlinkNode(node);
	xmlFreeNode(node);
}

static xmlChar *_xmlConvStr(const char *ascii, int pos)
{
	/* do not free the returned pointer: each time
	 * the function is called, it frees the precedent pointer.
	 * but last pointers are to be freed at least.
	 */
	if (NULL != g_xmlConvStr[pos])
		xmlFree(g_xmlConvStr[pos]);

	g_xmlConvStr[pos] = xmlCharStrdup(ascii);

	return g_xmlConvStr[pos];
}

static void _xmlMoveTo(const xmlNodePtr node)
{
	if (g_dthPublisherVerbose) {
		if (xmlStrEqual(node->name, XMLSTR0("group")) != 0)
			fprintf(stdout, "cursor: group\n");
		else if (xmlStrEqual(node->name, XMLSTR0("action")) != 0)
			fprintf(stdout, "cursor: action\n");
		else if (xmlStrEqual(node->name, XMLSTR0("argument")) != 0)
			fprintf(stdout, "cursor: argument\n");
		else if (xmlStrEqual(node->name, XMLSTR0("module")) != 0)
			fprintf(stdout, "cursor: module\n");
		else if (xmlStrEqual(node->name, XMLSTR0("dthapi")) != 0)
			fprintf(stdout, "cursor: (root)\n");
		else
			fprintf(stdout, "cursor: (unknown!)\n");
	}

	g_thisNode = node;
}

static void _xmlEndAction(void)
{
	if (*g_action) {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "close action %s.\n", g_action);

		_xmlMoveTo(g_thisNode->parent);
		memset(g_action, 0, MAX_ELEMENT_PATH);
	}
}

static void _xmlEndGroup(void)
{
	_xmlEndAction();

	if (*g_group) {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "close group %s.\n", g_group);

		_xmlMoveTo(g_thisNode->parent);
		memset(g_group, 0, MAX_ELEMENT_PATH);
	}
}

static void _xmlEndModule(void)
{
	/* close group if opened */
	_xmlEndGroup();

	if (*g_module) {
		/* close previous module before opening next */
		if (g_dthPublisherVerbose)
			fprintf(stdout, "close module %s.\n", g_module);

		_xmlMoveTo(g_thisNode->parent);
		memset(g_module, 0, MAX_MODULE_NAME);
	}
}

static void _dthGetTypeString(const int type, char *str, const int size)
{
	char *ptr = "";
	int vl_size = 0;

	if ((type >= DTH_BITFIELD_PART_MIN_SIZE)
	    && (type <= DTH_BITFIELD_PART_MAX_SIZE)) {
		snprintf(str, size, "BIFIELD%i",
			 type - DTH_BITFIELD_PART_MIN_SIZE);
	} else {
		switch (type) {
		case DTH_TYPE_U8:
			ptr = "U8";
			break;
		case DTH_TYPE_S8:
			ptr = "S8";
			break;
		case DTH_TYPE_U16:
			ptr = "U16";
			break;
		case DTH_TYPE_S16:
			ptr = "S16";
			break;
		case DTH_TYPE_U32:
			ptr = "U32";
			break;
		case DTH_TYPE_S32:
			ptr = "S32";
			break;
		case DTH_TYPE_U64:
			ptr = "U64";
			break;
		case DTH_TYPE_S64:
			ptr = "S64";
			break;
		case DTH_TYPE_FLOAT:
			ptr = "FLOAT";
			break;
		case DTH_TYPE_STRING:
			ptr = "STRING";
			break;
		default:
			ptr = "UNKNOWN";
		}

		vl_size = strlen(ptr);
		if (vl_size >= size)
			vl_size = size - 1;

		strncpy(str, ptr, vl_size);
		str[vl_size] = 0;
	}
}

static void _dthPublishEnum(const char *enumstr)
{
	int start = 0;
	int pos = 0;
	char item[256];
	int index = 0, len;
	int len_enum = strlen(enumstr);

	xmlNodePtr choice =
	    xmlNewChild(g_thisNode, NULL, XMLSTR0("enum"), NULL);
	xmlNodePtr opt = NULL;

	while (pos < len_enum) {
		while (enumstr[pos] && enumstr[pos] != '\n')
			pos++;

		if (pos < len_enum) {
			len = pos - start;
			strncpy(item, enumstr + start, len);
			item[len] = 0;

			opt =
			    xmlNewTextChild(choice, NULL, XMLSTR0("enum_item"),
					    XMLSTR1(item));
			sprintf(item, "%i", index);
			xmlSetProp(opt, XMLSTR0("value"), XMLSTR1(item));
			index++;
		} else if (pos > start) {
			opt =
			    xmlNewTextChild(choice, NULL, XMLSTR0("enum_item"),
					    XMLSTR1(enumstr + start));
			sprintf(item, "%i", index);
			xmlSetProp(opt, XMLSTR0("value"), XMLSTR1(item));

			index++;
		}
		start = pos + 1;
		pos++;
	}
}

char *replace_str(char *str, char *orig, char *rep)
{
	static char buffer[4096];
	char *s, *p, *b;

	if ((NULL != orig) && strlen(orig)) {
		s = str;
		b = buffer;
		p = strstr(s, orig);
		if (NULL != p) {
			do {
				strncpy(b, s, p - s);
				b += p - s;

				strncpy(b, rep, strlen(rep));
				b += strlen(rep);
				*b = 0;

				s = p + strlen(orig);

				p = strstr(s, orig);
			} while (p);

			strncpy(b, s, (str + strlen(str)) - s);
			b += (str + strlen(str)) - s;
			*b = 0;

			strncpy(str, buffer, strlen(buffer));
			str[strlen(buffer)] = 0;
		}
	}

	return str;
}

static void _dthPublishInfo(const char *info)
{
	int len_info = strlen(info);
	char *xml_info = (char *)malloc(len_info * 2);

	if ((NULL != xml_info) && strlen(xml_info)) {
		strncpy(xml_info, info, len_info);
		xml_info[len_info] = 0;

		/* pseudo line feed is converted to another
		 * printable sequence */
		replace_str(xml_info, "\\n", "\\\\n");
		/* C line feed is converted to another
		 * printable sequence */
		replace_str(xml_info, "\n", "\\n");

		if (g_dthPublisherVerbose)
			fprintf(stdout, "add node info: %s.\n", xml_info);

		/* create a sub node whose value is the info string */
		xmlNewTextChild(g_thisNode, NULL, XMLSTR0("info"),
				XMLSTR1(xml_info));
	}

	free(xml_info);
}

/* PUBLIC INTERFACE */

void dth_publish_verbose(const int verbose)
{
	fprintf(stdout, "DTH publisher verbose set to %i\n", verbose);
	g_dthPublisherVerbose = verbose;
}

int dth_publish_is_enabled(void)
{
	return g_dthPublisherEnabled;
}

int dth_publish_init(const char *filename, const char mode)
{
	int err = 0;

	if (g_dthPublisherVerbose)
		fprintf(stdout, "DTH publisher destination: %s\n", filename);

	/* globals initialization */
	memset(g_module, 0, MAX_MODULE_NAME);
	memset(g_group, 0, MAX_ELEMENT_PATH);
	g_groupDepth = 0;
	memset(g_action, 0, MAX_ELEMENT_PATH);
	memset(g_elementPath, 0, MAX_ELEMENT_PATH);

	/* Any XML file to be produced should be indented so
	 * it is easier to read in plain text */
	xmlIndentTreeOutput = 1;

	/* close any opened document before proceeding */
	dth_publish_validate(1);

	if (strlen(filename) >= sizeof(g_filename))	{
		err = ENAMETOOLONG;
		goto error;
	}

	strcpy(g_filename, filename);

	/* in append mode, try to load the file */
	if ('a' == mode) {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "Open file %s if existing...\n",
				filename);

		g_xmlDocPtr = xmlParseFile(filename);
	}

	if (g_xmlDocPtr == NULL) {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "Creating XML document...\n");

		/* file not existing. create it */
		g_xmlDocPtr = xmlNewDoc(XMLSTR0("1.0"));

		if (g_xmlDocPtr != NULL) {
			if (g_dthPublisherVerbose)
				fprintf(stdout, "Xml doc was created.\n");

			g_xmlDocPtr->children =
			    xmlNewDocNode(g_xmlDocPtr, NULL, XMLSTR0("dthapi"),
					  NULL);
			_xmlMoveTo(xmlDocGetRootElement(g_xmlDocPtr));
		} else {
			fprintf(stdout, "Fail to create XML doc!\n");
			err = EBADF;	/* error bad file descriptor */
		}
	} else {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "File %s exists.\n", filename);

		/* file opened. set focus on root node (should be dthapi) */
		_xmlMoveTo(xmlDocGetRootElement(g_xmlDocPtr));
	}

	if (0 == err) {
		fprintf(stdout, "DTH publisher initialized.\n");
		g_dthPublisherEnabled = 1;
	}
error:
	return err;
}

int dth_publish_element(const struct dth_element *elem)
{
	char local_group[MAX_ELEMENT_PATH];
	char xmlTag[125];

	if (g_xmlDocPtr == NULL)
		return EBADF;

	if (g_dthPublisherVerbose)
		fprintf(stdout, "DTH publishing %s...\n", elem->path);

	/* open a group if enabled and this element is owned by another group */
	_dthGetGroupName(elem->path, local_group, MAX_ELEMENT_PATH);

	int lenGroup = strlen(local_group);
	if (lenGroup > 0) {
		if (strcmp(g_group, local_group) != 0) {
			/* change group */
			_xmlEndGroup();

			if (g_dthPublisherVerbose)
				fprintf(stdout, "open group %s.\n",
					local_group);

			strncpy(g_group, local_group, lenGroup);
			_xmlMoveTo(xmlNewChild
				   (g_thisNode, NULL, XMLSTR0("group"), NULL));
			xmlSetProp(g_thisNode, XMLSTR0("label"),
				   XMLSTR1(local_group));
		}
	} else if (*g_group) {
		/* change group */
		_xmlEndGroup();
	}

	/* close action if changing */
	int len_action = strlen(g_action);
	if ((0 != *g_action)
	    && (strncmp(g_action, elem->path, len_action) != 0))
		_xmlEndAction();

	if (elem->exec != NULL) {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "add and select an action\n");

		_xmlEndAction();
		strcpy(g_action, elem->path);

		_xmlMoveTo(xmlNewChild
			   (g_thisNode, NULL, XMLSTR0("action"), NULL));
	} else {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "add and select an argument\n");

		_xmlMoveTo(xmlNewChild
			   (g_thisNode, NULL, XMLSTR0("argument"), NULL));

	}

	char elt_type_str[50];
	_dthGetTypeString(elem->type, elt_type_str, 50);

	if (g_dthPublisherVerbose)
		fprintf(stdout, "add attribute path\n");

	xmlSetProp(g_thisNode, XMLSTR0("path"), XMLSTR1(elem->path));

	if (g_dthPublisherVerbose)
		fprintf(stdout, "add attribute type\n");

	xmlSetProp(g_thisNode, XMLSTR0("type"), XMLSTR1(elt_type_str));

	sprintf(xmlTag, "%d", elem->user_data);
	xmlSetProp(g_thisNode, XMLSTR0("user_data"), XMLSTR1(xmlTag));

	if (elem->get != NULL) {
		sprintf(xmlTag, "get_%p", elem->get);
		xmlSetProp(g_thisNode, XMLSTR0("get"), XMLSTR1(xmlTag));
	}

	if (elem->set != NULL) {
		sprintf(xmlTag, "set_%p", elem->set);
		xmlSetProp(g_thisNode, XMLSTR0("set"), XMLSTR1(xmlTag));
	}

	if (elem->exec != NULL) {
		sprintf(xmlTag, "exec_%p", elem->exec);
		xmlSetProp(g_thisNode, XMLSTR0("exec"), XMLSTR1(xmlTag));
	}

	if (*g_elementPath) {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "add attribute source\n");

		xmlSetProp(g_thisNode, XMLSTR0("source"),
			   XMLSTR1(g_elementPath));
	}

	if (elem->enumString) {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "add node enum\n");

		_dthPublishEnum(elem->enumString);
	}

	if (elem->min < elem->max) {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "add node range\n");

		_xmlMoveTo(xmlNewChild
			   (g_thisNode, NULL, XMLSTR0("range"), NULL));
		sprintf(xmlTag, "%lld", elem->min);
		xmlSetProp(g_thisNode, XMLSTR0("min"), XMLSTR1(xmlTag));
		sprintf(xmlTag, "%lld", elem->max);
		xmlSetProp(g_thisNode, XMLSTR0("max"), XMLSTR1(xmlTag));
		_xmlMoveTo(g_thisNode->parent);
	}

	if ((elem->rows != 0) || (elem->cols != 0)) {
		if (g_dthPublisherVerbose)
			fprintf(stdout, "add node array\n");

		_xmlMoveTo(xmlNewChild
			   (g_thisNode, NULL, XMLSTR0("array"), NULL));
		sprintf(xmlTag, "%d", elem->rows);
		xmlSetProp(g_thisNode, XMLSTR0("rows"), XMLSTR1(xmlTag));
		sprintf(xmlTag, "%d", elem->cols);
		xmlSetProp(g_thisNode, XMLSTR0("cols"), XMLSTR1(xmlTag));
		_xmlMoveTo(g_thisNode->parent);
	}

	if (elem->info != NULL)
		_dthPublishInfo(elem->info);

	if (elem->exec == NULL)
		_xmlMoveTo(g_thisNode->parent);

	if (g_dthPublisherVerbose)
		fprintf(stdout, "Published: %s\n", elem->path);

	return 0;
}

void dth_publish_validate(const int doValidate)
{
	if (NULL != g_xmlDocPtr) {
		if (doValidate) {
			fprintf(stdout, "Saving API to file %s\n", g_filename);
			xmlSaveFormatFile(g_filename, g_xmlDocPtr, 1);
		} else {
			xmlFreeDoc(g_xmlDocPtr);
			g_xmlDocPtr = NULL;

			memset(g_filename, 0, MAX_FILEPATH);
		}
	}
}

void dth_publish_term(void)
{
	fprintf(stdout, "DTH publisher termination.\n");

	if (g_xmlDocPtr) {
		xmlFreeDoc(g_xmlDocPtr);
		g_xmlDocPtr = NULL;
	}

	memset(g_filename, 0, MAX_FILEPATH);

	xmlCleanupParser();

	g_dthPublisherEnabled = 0;
}

void dth_publish_set_module(const char *module)
{
	char xpath[255];
	int len, i;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;

	/* close previous module before opening next */
	_xmlEndModule();

	len = strlen(module);
	if (len >= MAX_MODULE_NAME)
		len = MAX_MODULE_NAME - 1;

	strncpy(g_module, module, len);
	g_module[len] = 0;

	fprintf(stdout, "open module %s.\n", module);

	if (NULL != g_xmlDocPtr) {
		/* see if the node for the module already exists */
		sprintf(xpath, "/dthapi/module[@name=\"%s\"]", g_module);
		result = _xmlXPathNodeset(g_xmlDocPtr, XMLSTR0(xpath));

		/* if yes then remove children */
		if (result) {
			nodeset = result->nodesetval;
			if (g_dthPublisherVerbose)
				fprintf(stdout,
					"found %d nodes matching module %s.\n",
					nodeset->nodeNr, g_module);

			for (i = 0; i < nodeset->nodeNr; i++)
				_xmlRemoveNode(nodeset->nodeTab[i]);

			xmlXPathFreeObject(result);

			/* create the node and select it */
			_xmlMoveTo(xmlNewChild
				   (g_thisNode, NULL, XMLSTR0("module"), NULL));
			xmlSetProp(g_thisNode, XMLSTR0("name"),
				   XMLSTR1(g_module));
		} else {
			/* if not create the node then select it */
			_xmlMoveTo(xmlNewChild
				   (g_thisNode, NULL, XMLSTR0("module"), NULL));
			xmlSetProp(g_thisNode, XMLSTR0("name"),
				   XMLSTR1(g_module));
		}
	}
}

void dth_publish_set_ref(const char *filename)
{
	int len = strlen(filename);
	if (len >= MAX_ELEMENT_PATH)
		len = MAX_ELEMENT_PATH - 1;

	strncpy(g_elementPath, filename, len);
	g_elementPath[len] = 0;
}

inline void dth_publish_set_group_depth(const int depth)
{
	g_groupDepth = depth;
}

