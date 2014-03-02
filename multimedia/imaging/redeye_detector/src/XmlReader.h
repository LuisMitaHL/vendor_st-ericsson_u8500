/*******************************************************************************
 *                                                                                         
 * STMicroelectronics - Company Confidential                                                
 *                                                                                         
 *******************************************************************************
 *                                                                                         
 * Project name		: RedEyes
 * File name			: Xmlreader.h
 * Goal					: Red-Eyes Removal
 * Date					: 10 November 2009           
 * Version				: Release 2.0
 * Author				: Daniele Ravì
 * Company				: AST - STMicroelectronics - Catania                                             
 *                                                                                                     
 *******************************************************************************/

#include "RedEyes.h"

void findXmlTag(char* xmlString,char* tagStringB,char* tagStringE,size_t* indBg,size_t* indEn,char* strRet);
size_t find(char *p1,char *p2,size_t ind);
void substr(char *p1,char* p2, size_t bg,size_t ln);

