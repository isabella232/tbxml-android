// INCLUDE FILES

#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <android/log.h>
#include "tbxml.h"

// TYPEDEFS

typedef int BOOL;

#define YES 1
#define NO  0

// FUNCTION PROTOTYPES

void            decodeBytes           (TBXMLDocument *);
TBXMLElement   *nextAvailableElement  (TBXMLDocument *);
TBXMLAttribute *nextAvailableAttribute(TBXMLDocument *);

// __android_log_print(ANDROID_LOG_INFO,"TBXML","ATTRIBUTE: %s::%s",name,value);

/** JNIParse
 *
 */

jlong Java_za_co_twyst_tbxml_TBXML_jniParse(JNIEnv* env,jobject object,jbyteArray xml) {
     TBXMLDocument *document = (TBXMLDocument *) calloc(1,sizeof(TBXMLDocument));
     int            N        = (*env)->GetArrayLength(env,xml);

     document->length                 = N;
     document->xml                    = calloc(1,N+1);
     document->rootXMLElement         = NULL;
     document->currentElement         = 0;
     document->currentElementBuffer   = NULL;
     document->currentAttribute       = 0;
     document->currentAttributeBuffer = NULL;

     (*env)->GetByteArrayRegion(env,xml,0,N,document->xml);

     decodeBytes(document);

     return (jlong) (uintptr_t) document;
}

/** JNIFree
 *
 */
void Java_za_co_twyst_tbxml_TBXML_jniFree(JNIEnv* env,jobject object,jlong document) {
    TBXMLDocument *doc = (TBXMLDocument *) (uintptr_t) document;

    if (doc) {
    	if (doc->xml) {
    	   free(doc->xml);
    	}

    	if (doc->currentElementBuffer) {
    	   free(doc->currentElementBuffer);
    	}

    	if (doc->currentAttributeBuffer) {
    	   free(doc->currentAttributeBuffer);
    	}

    	free(doc);
    }
}

/** JNIRootElement
 *
 */
jlong Java_za_co_twyst_tbxml_TBXML_jniRootElement(JNIEnv* env,jobject object,jlong document) {
      TBXMLDocument *doc = (TBXMLDocument *) (uintptr_t) document;

      if (doc) {
    	  return (jlong) (uintptr_t) doc->rootXMLElement;
      }

      return (jlong) (uintptr_t) NULL;
}

/** JNIFirstChild
 *
 */
jlong Java_za_co_twyst_tbxml_TBXML_jniFirstChild(JNIEnv* env,jobject object,jlong document,jlong element) {
      TBXMLDocument *doc  = (TBXMLDocument *) (uintptr_t) document;
      TBXMLElement  *node = (TBXMLElement  *) (uintptr_t) element;

      if (doc) {
    	  if (node) {
        	  return (jlong) (uintptr_t) node->firstChild;
    	  }
      }

      return (jlong) (uintptr_t) NULL;
}

/** JNIChildElementNamed
 *
 */
jlong Java_za_co_twyst_tbxml_TBXML_jniChildElementNamed(JNIEnv* env,jobject object,jlong document,jlong element,jstring tag) {
      TBXMLDocument *doc  = (TBXMLDocument *) (uintptr_t) document;
      TBXMLElement  *node = (TBXMLElement  *) (uintptr_t) element;
      const char    *name = (*env)->GetStringUTFChars(env,tag,0);
      TBXMLElement  *child = NULL;

      if (doc) {
    	  if (node) {
    		  TBXMLElement *_node = node->firstChild;

    		  while(_node) {
    			  if (strcmp(_node->name,name) == 0) {
    				  child = _node;
    				  break;
    			  }

    			  _node = _node->nextSibling;
    		  }
    	  }
      }

      (*env)->ReleaseStringUTFChars(env,tag,name);

      return (jlong) (uintptr_t) child;
}

/** JNINextSibling
 *
 */
jlong Java_za_co_twyst_tbxml_TBXML_jniNextSibling(JNIEnv* env,jobject object,jlong document,jlong element) {
      TBXMLDocument *doc  = (TBXMLDocument *) (uintptr_t) document;
      TBXMLElement  *node = (TBXMLElement  *) (uintptr_t) element;

      if (doc) {
    	  if (node) {
        	  return (jlong) (uintptr_t) node->nextSibling;
    	  }
      }

      return (jlong) (uintptr_t) NULL;
}

/** JNINextSiblingNamed
 *
 */
jlong Java_za_co_twyst_tbxml_TBXML_jniNextSiblingNamed(JNIEnv* env,jobject object,jlong document,jlong element,jstring tag) {
      TBXMLDocument *doc   = (TBXMLDocument *) (uintptr_t) document;
      TBXMLElement  *node  = (TBXMLElement  *) (uintptr_t) element;
      const char    *name  = (*env)->GetStringUTFChars(env,tag,0);
      TBXMLElement  *child = NULL;

      if (doc) {
    	  if (node) {
    		  TBXMLElement *_node = node->nextSibling;

    		  while(_node) {
    			  if (strcmp(_node->name,name) == 0) {
    				  child = _node;
    				  break;
    			  }

    			  _node = _node->nextSibling;
    		  }
    	  }
      }

      (*env)->ReleaseStringUTFChars(env,tag,name);

      return (jlong) (uintptr_t) child;
}

/** JNIElementName
 *
 */
jstring Java_za_co_twyst_tbxml_TBXML_jniElementName(JNIEnv* env,jobject object,jlong document,jlong element) {
      TBXMLDocument *doc  = (TBXMLDocument *) (uintptr_t) document;
      TBXMLElement  *node = (TBXMLElement  *) (uintptr_t) element;

      if (doc) {
    	  if (node) {
        	  return (*env)->NewStringUTF(env,node->name);
    	  }
      }

      return (jstring) NULL;
}

/** JNIValueOfAttributeNamed
 *
 */
jstring Java_za_co_twyst_tbxml_TBXML_jniValueOfAttributeNamed(JNIEnv* env,jobject object,jlong document,jlong element,jstring attribute) {
      const TBXMLDocument *doc   = (TBXMLDocument *) (uintptr_t) document;
      const TBXMLElement  *node  = (TBXMLElement  *) (uintptr_t) element;
      const char          *name  = (*env)->GetStringUTFChars(env,attribute,0);
            char          *value = NULL;

      if (doc) {
    	  if (node) {
    		  TBXMLAttribute *attr = node->firstAttribute;

    		  while (attr) {
    			  if (strcmp(name,attr->name) == 0) {
    				  value = attr->value;
    				  break;
    			  }

    			  attr = attr->next;
    		  }
    	  }
      }

      (*env)->ReleaseStringUTFChars(env,attribute,name);

	  return value ? (*env)->NewStringUTF(env,value) : (*env)->NewStringUTF(env,"");
}

/** JNITextForElement
 *
 */
jstring Java_za_co_twyst_tbxml_TBXML_jniTextForElement(JNIEnv* env,jobject object,jlong document,jlong element) {
      TBXMLDocument *doc  = (TBXMLDocument *) (uintptr_t) document;
      TBXMLElement  *node = (TBXMLElement  *) (uintptr_t) element;

      if (doc) {
    	  if (node) {
    		  if (node->text) {
    			  return (*env)->NewStringUTF(env,node->text);
    		  }
    	  }
      }

	  return (*env)->NewStringUTF(env,"");
}


/** JNIListElementsForQuery
 *
 */
jlongArray Java_za_co_twyst_tbxml_TBXML_jniListElementsForQuery(JNIEnv* env,jobject object,jlong document,jlong element,jstring query) {
      TBXMLDocument *doc     = (TBXMLDocument *) (uintptr_t) document;
      TBXMLElement  *node    = (TBXMLElement  *) (uintptr_t) element;
      const char    *str     = (*env)->GetStringUTFChars(env,query,0);
      jlongArray     result  = (*env)->NewLongArray(env,3);
      TBXMLElement  *current = node;

      if (doc) {
    	  if (node) {
    		   char *token = strtok(str,".");

    		   while (token) {
    			  __android_log_print(ANDROID_LOG_INFO,"TBXML","QUERY %s",token);

    			  if (strcmp(token,"*") == 0) {
    				  current = current->firstChild;
    			  } else {
                      // ... 'childElementNamed'

    				  TBXMLElement *_node = current->firstChild;
    				  TBXMLElement *child = NULL;

    	    		  while(_node) {
    	    			  if (strcmp(_node->name,token) == 0) {
    	    				  child = _node;
    	    				  break;
    	    			  }

    	    			  _node = _node->nextSibling;
    	    		  }

    	    		  current = child;
    			  }

                  if (!current) {
    		         break;
                  }

    		      token = strtok (NULL,".");
    		   }

    		 // ... enumerate matching nodes

    	     TBXMLElement *root  = current;
    		 int           count = 0;

    		 while(current) {
    			 count++;
    			 current = current->nextSibling;
    		 }

   			  __android_log_print(ANDROID_LOG_INFO,"TBXML","QUERY:MATCH  %d",count);
    	  }
      }

      int   i;
      jlong fill[3];

      fill[0] = 123;
      fill[1] = 345;
      fill[2] = 567;

      (*env)->ReleaseStringUTFChars(env,query,str);
      (*env)->SetLongArrayRegion   (env,result,0,3,fill);

	  return result;
}

//for (NSInteger i=0; i < components.count; ++i) {
//    NSString *iTagName = [components objectAtIndex:i];
//
//    if ([iTagName isEqualToString:@"*"]) {
//        currTBXMLElement = currTBXMLElement->firstChild;
//
//        // different behavior depending on if this is the end of the query or midstream
//        if (i < (components.count - 1)) {
//            // midstream
//            do {
//                NSString *restOfQuery = [[components subarrayWithRange:NSMakeRange(i + 1, components.count - i - 1)] componentsJoinedByString:@"."];
//                [TBXML iterateElementsForQuery:restOfQuery fromElement:currTBXMLElement withBlock:iterateBlock];
//            } while ((currTBXMLElement = currTBXMLElement->nextSibling));
//
//        }
//    } else {
//        currTBXMLElement = [TBXML childElementNamed:iTagName parentElement:currTBXMLElement];
//    }
//
//    if (!currTBXMLElement) {
//        break;
//    }
//}
//
//if (currTBXMLElement) {
//    // enumerate
//    NSString *childTagName = [components lastObject];
//
//    if ([childTagName isEqualToString:@"*"]) {
//        childTagName = nil;
//    }
//
//    do {
//        iterateBlock(currTBXMLElement);
//    } while (childTagName ? (currTBXMLElement = [TBXML nextSiblingNamed:childTagName searchFromElement:currTBXMLElement]) : (currTBXMLElement = currTBXMLElement->nextSibling));


// *** INTERNAL ***

void decodeBytes(TBXMLDocument *document)
     { // ... initialise

       jbyte        *bytes            = document->xml;
       jsize         N                = document->length;
	   char         *elementStart     = bytes;
	   TBXMLElement *parentXMLElement = NULL;

		// ... find next element start

		while ((elementStart = strstr(elementStart,"<")))
		      { // ... comment ?

			    if (strncmp(elementStart,"<!--",4) == 0)
			       { elementStart = strstr(elementStart,"-->") + 3;
			         continue;
			       }

				// .... CDATA ?

				int isCDATA = strncmp(elementStart,"<![CDATA[",9);

				if (isCDATA == 0)
				   { char *CDATAEnd   = strstr(elementStart,"]]>");
				     char *elementEnd = CDATAEnd;

				     elementEnd = strstr(elementEnd,"<");

				     while (strncmp(elementEnd,"<![CDATA[",9) == 0)
				           { elementEnd = strstr(elementEnd,"]]>");
				             elementEnd = strstr(elementEnd,"<");
				           }

					long CDATALength = CDATAEnd-elementStart;
					long textLength  = elementEnd-elementStart;

					memcpy(elementStart,elementStart+9,CDATAEnd-elementStart-9);
					memcpy(CDATAEnd-9,CDATAEnd+3,textLength-CDATALength-3);
					memset(elementStart+textLength-12,' ',12);

					elementStart = CDATAEnd-9;
					continue;
				}

				// ... find element end (skipping any CDATA sections within attributes)

				char *elementEnd = elementStart+1;

				while ((elementEnd = strpbrk(elementEnd, "<>")))
				      { if (strncmp(elementEnd,"<![CDATA[",9) == 0)
				           { elementEnd = strstr(elementEnd,"]]>")+3;
				           }
				      	   else
				      	   { break;
				      	   }
				      }

				if (!elementEnd)
				   { break;
				   }

				// ... delimit element

				if (elementEnd)
				   { *elementEnd = 0;
				   }

				*elementStart = 0;

				// ... extract element name

				char *elementNameStart = elementStart+1;

				// __android_log_print(ANDROID_LOG_INFO,"TBXML","ELEMENT: %s",elementNameStart);

				// ... skip <? and <! tags

				if (*elementNameStart == '?' || (*elementNameStart == '!' && isCDATA != 0)) {
					elementStart = elementEnd+1;
					continue;
				}

				// ... ignore attributes/text if this is a closing element

				if (*elementNameStart == '/')
				   { elementStart = elementEnd+1;

				     if (parentXMLElement)
				        { if (parentXMLElement->text)
				             { while (isspace(*parentXMLElement->text))
								     { parentXMLElement->text++;
								     }

				               char *end = parentXMLElement->text + strlen(parentXMLElement->text)-1;

				               while (end > parentXMLElement->text && isspace(*end))
				            	     { *end--=0;
				            	     }
				             }

						  parentXMLElement = parentXMLElement->parentElement;

						  if (parentXMLElement && parentXMLElement->firstChild)
							 { parentXMLElement->text = 0;
							 }
				        }

				     continue;
				   }

				// is this element opening and closing

				BOOL selfClosingElement = NO;

				if (*(elementEnd-1) == '/')
				   { selfClosingElement = YES;
				   }

				TBXMLElement *xmlElement = nextAvailableElement(document);

				xmlElement->name = elementNameStart;

				// ... add to parent element

				if (parentXMLElement) {
				   if (parentXMLElement->currentChild) {
						parentXMLElement->currentChild->nextSibling = xmlElement;
						xmlElement->previousSibling                 = parentXMLElement->currentChild;
						parentXMLElement->currentChild              = xmlElement;
				   } else {
					  parentXMLElement->currentChild = xmlElement;
					  parentXMLElement->firstChild   = xmlElement;
				   }

				   xmlElement->parentElement = parentXMLElement;
				}

				// in the following xml the ">" is replaced with \0 by elementEnd.
				// element may contain no atributes and would return nil while looking for element name end
				// <tile>
				// find end of element name
				char *elementNameEnd = strpbrk(xmlElement->name," /\n");

				// if end was found check for attributes
				if (elementNameEnd) {
					*elementNameEnd = 0;

					char           *chr              = elementNameEnd;
					char           *name             = NULL;
					char           *value            = NULL;
					char           *CDATAStart       = NULL;
					char           *CDATAEnd         = NULL;
					TBXMLAttribute *lastXMLAttribute = NULL;
					TBXMLAttribute *xmlAttribute     = NULL;
					BOOL            singleQuote      = NO;
					int             mode             = TBXML_ATTRIBUTE_NAME_START;

					// loop through all characters within element
					while (chr++ < elementEnd) {
						switch (mode) {
							// look for start of attribute name
							case TBXML_ATTRIBUTE_NAME_START:
                                 if (isspace(*chr))
                                	continue;

                                 name = chr;
                                 mode = TBXML_ATTRIBUTE_NAME_END;
                                 break;

                            // look for end of attribute name
							case TBXML_ATTRIBUTE_NAME_END:
							     if (isspace(*chr) || *chr == '=')
							        { *chr  = 0;
									   mode = TBXML_ATTRIBUTE_VALUE_START;
							        }
							     break;

							// look for start of attribute value
							case TBXML_ATTRIBUTE_VALUE_START:
							     if (isspace(*chr))
							    	continue;

							     if (*chr == '"' || *chr == '\'')
							        { value = chr+1;
									  mode  = TBXML_ATTRIBUTE_VALUE_END;

									  if (*chr == '\'')
										 singleQuote = YES;
									     else
										 singleQuote = NO;
							        }
							     break;

							// look for end of attribute value
							case TBXML_ATTRIBUTE_VALUE_END:
							     if (*chr == '<' && strncmp(chr, "<![CDATA[", 9) == 0) {
							    	 mode = TBXML_ATTRIBUTE_CDATA_END;
								 } else if ((*chr == '"' && singleQuote == NO) || (*chr == '\'' && singleQuote == YES)) {
									*chr = 0;

									while ((CDATAStart = strstr(value, "<![CDATA["))) {
										memcpy(CDATAStart, CDATAStart+9, strlen(CDATAStart)-8);
										CDATAEnd = strstr(CDATAStart,"]]>");
										memcpy(CDATAEnd, CDATAEnd+3, strlen(CDATAEnd)-2);
									}


									// create new attribute
									xmlAttribute = nextAvailableAttribute(document);

									// if this is the first attribute found, set pointer to this attribute on element
									if (!xmlElement->firstAttribute) {
										xmlElement->firstAttribute = xmlAttribute;
									}

									// if previous attribute found, link this attribute to previous one
									if (lastXMLAttribute) {
										lastXMLAttribute->next = xmlAttribute;
									}

									// set last attribute to this attribute
									lastXMLAttribute = xmlAttribute;

									// set attribute name & value
									xmlAttribute->name  = name;
									xmlAttribute->value = value;

//									__android_log_print(ANDROID_LOG_INFO,"TBXML","ATTRIBUTE: %s::%s",name,value);

									// clear name and value pointers
									name  = NULL;
									value = NULL;

									// start looking for next attribute
									mode = TBXML_ATTRIBUTE_NAME_START;
								}
								break;
								// look for end of cdata
							case TBXML_ATTRIBUTE_CDATA_END:
								if (*chr == ']') {
									if (strncmp(chr, "]]>", 3) == 0) {
										mode = TBXML_ATTRIBUTE_VALUE_END;
									}
								}
								break;
							default:
								break;
						}
					}
				}

				// if tag is not self closing, set parent to current element

				if (!selfClosingElement) {
					// set text on element to element end+1
					if (*(elementEnd+1) != '>') {
						xmlElement->text = elementEnd+1;
					}

					parentXMLElement = xmlElement;
				}

				// start looking for next element after end of current element
				elementStart = elementEnd+1;
		      }
     }

TBXMLElement *nextAvailableElement(TBXMLDocument *document) {
	document->currentElement++;

	if (!document->currentElementBuffer) {
		document->currentElementBuffer           = calloc(1,sizeof(TBXMLElementBuffer));
		document->currentElementBuffer->elements = (TBXMLElement*)calloc(1,sizeof(TBXMLElement) * MAX_ELEMENTS);
		document->currentElement                 = 0;
		document->rootXMLElement                 = &document->currentElementBuffer->elements[document->currentElement];
	} else if (document->currentElement >= MAX_ELEMENTS) {
		document->currentElementBuffer->next           = calloc(1,sizeof(TBXMLElementBuffer));
		document->currentElementBuffer->next->previous = document->currentElementBuffer;
		document->currentElementBuffer                 = document->currentElementBuffer->next;
		document->currentElementBuffer->elements       = (TBXMLElement*)calloc(1,sizeof(TBXMLElement)*MAX_ELEMENTS);
		document->currentElement = 0;
	}

	return &document->currentElementBuffer->elements[document->currentElement];
}

TBXMLAttribute *nextAvailableAttribute(TBXMLDocument *document) {
	document->currentAttribute++;

	if (!document->currentAttributeBuffer) {
		document->currentAttributeBuffer             = calloc(1,sizeof(TBXMLAttributeBuffer));
		document->currentAttributeBuffer->attributes = (TBXMLAttribute*) calloc(MAX_ATTRIBUTES,sizeof(TBXMLAttribute));
		document->currentAttribute                   = 0;
	} else if (document->currentAttribute >= MAX_ATTRIBUTES) {
		document->currentAttributeBuffer->next           = calloc(1,sizeof(TBXMLAttributeBuffer));
		document->currentAttributeBuffer->next->previous = document->currentAttributeBuffer;
		document->currentAttributeBuffer                 = document->currentAttributeBuffer->next;
		document->currentAttributeBuffer->attributes     = (TBXMLAttribute*) calloc(MAX_ATTRIBUTES,sizeof(TBXMLAttribute));
		document->currentAttribute = 0;
	}

	return &document->currentAttributeBuffer->attributes[document->currentAttribute];
}
