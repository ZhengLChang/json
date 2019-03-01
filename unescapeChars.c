#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int cgiHexValue[256];
static void cgiSetupConstants();
typedef enum {
  cgiEscapeRest,
  cgiEscapeFirst,
  cgiEscapeSecond
} cgiEscapeState;

typedef enum {
  cgiUnescapeSuccess,
  cgiUnescapeMemory
} cgiUnescapeResultType;
typedef enum {
  cgiParseSuccess,
  cgiParseMemory,
  cgiParseIO
} cgiParseResultType;
typedef struct cgiFormEntryStruct {
  char *attr;
  /* value is populated for regular form fields only.
   * *    For file uploads, it points to an empty string, and file
   *        upload data should be read from the file tfileName. */ 
  char *value;
  /* When fileName is not an empty string, tfileName is not null,
   * *  and 'value' points to an empty string. */
  /* Valid for both files and regular fields; does not include
   * *    terminating null of regular fields. */
  int valueLength;
  char *fileName; 
  char *contentType;
  /* Temporary file name for working storage of file uploads. */
  char *tfileName;
  struct cgiFormEntryStruct *next;
} cgiFormEntry;
static cgiFormEntry *cgiFormEntryFirst;
cgiUnescapeResultType cgiUnescapeChars(char **sp, char *cp, int len);
static cgiParseResultType cgiParseFormInput(char *data, int length);
static void cgiFreeResources();
int main(void){ 
  char *sp = NULL;
  char cp[] = "PnPEnable=1&DHCPEnable=1&dhcpCustomOptionFor66=&provisioningServer=&userName=%26&password=&commonAESKey=&MACAESKey=&checkNewConfig=1&repeatedly=0&interval=1440&weekly=0&startHour=3&startMinute=0&stopHour=3&stopMinute=0&dayOfWeekly=1\%2C1\%2C0\%2C0\%2C0\%2C0\%2C1&sunday=1&monday=1&tuesday=0&wednesday=0&thursday=0&friday=0&saturday=1&user_set_auto";
  cgiSetupConstants();
  cgiParseFormInput(cp, strlen(cp));
  if(cgiFormEntryFirst != NULL){
    struct cgiFormEntryStruct *p = NULL;
    for(p = cgiFormEntryFirst; p ; p = p->next){
      printf("%s = %s\n", p->attr, p->value);
    }
  }

  cgiFreeResources();
//  cgiUnescapeChars(&sp, cp, strlen(cp));
//  printf("%s\n", sp);
  return 0;
}

cgiUnescapeResultType cgiUnescapeChars(char **sp, char *cp, int len) {
  char *s;
  cgiEscapeState escapeState = cgiEscapeRest;
  int escapedValue = 0;
  int srcPos = 0;
  int dstPos = 0;
  s = (char *) malloc(len + 1);
  if (!s) {
    return cgiUnescapeMemory;
  }
  while (srcPos < len) {
    int ch = cp[srcPos];
    switch (escapeState) {
      case cgiEscapeRest:
        if (ch == '%') {
          escapeState = cgiEscapeFirst;
        } else if (ch == '+') {
          s[dstPos++] = ' ';
        } else {
          s[dstPos++] = ch; 
        }
        break;
      case cgiEscapeFirst:
        escapedValue = cgiHexValue[ch] << 4;  
        escapeState = cgiEscapeSecond;
        break;
      case cgiEscapeSecond:
        escapedValue += cgiHexValue[ch];
        s[dstPos++] = escapedValue;
        escapeState = cgiEscapeRest;
        break;
    }
    srcPos++;
  }
  s[dstPos] = '\0';
  *sp = s;
  return cgiUnescapeSuccess;
}
static void cgiSetupConstants() {
  int i;
  for (i=0; (i < 256); i++) {
    cgiHexValue[i] = 0;
  }
  cgiHexValue['0'] = 0; 
  cgiHexValue['1'] = 1; 
  cgiHexValue['2'] = 2; 
  cgiHexValue['3'] = 3; 
  cgiHexValue['4'] = 4; 
  cgiHexValue['5'] = 5; 
  cgiHexValue['6'] = 6; 
  cgiHexValue['7'] = 7; 
  cgiHexValue['8'] = 8; 
  cgiHexValue['9'] = 9;
  cgiHexValue['A'] = 10;
  cgiHexValue['B'] = 11;
  cgiHexValue['C'] = 12;
  cgiHexValue['D'] = 13;
  cgiHexValue['E'] = 14;
  cgiHexValue['F'] = 15;
  cgiHexValue['a'] = 10;
  cgiHexValue['b'] = 11;
  cgiHexValue['c'] = 12;
  cgiHexValue['d'] = 13;
  cgiHexValue['e'] = 14;
  cgiHexValue['f'] = 15;
  return;
}

static cgiParseResultType cgiParseFormInput(char *data, int length) {
	/* Scan for pairs, unescaping and storing them as they are found. */
	int pos = 0;
	cgiFormEntry *n;
	cgiFormEntry *l = 0;
	while (pos != length) {
		int foundEq = 0;
		int foundAmp = 0;
		int start = pos;
		int len = 0;
		char *attr;
		char *value;
		while (pos != length) {
			if (data[pos] == '=') {
				foundEq = 1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		if (!foundEq) {
			break;
		}
		if (cgiUnescapeChars(&attr, data+start, len)
			!= cgiUnescapeSuccess) {
			return cgiParseMemory;
		}	
		start = pos;
		len = 0;
		while (pos != length) {
			if (data[pos] == '&') {
				foundAmp = 1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		/* The last pair probably won't be followed by a &, but
			that's fine, so check for that after accepting it */
		if (cgiUnescapeChars(&value, data+start, len)
			!= cgiUnescapeSuccess) {
			free(attr);
			return cgiParseMemory;
		}	
		/* OK, we have a new pair, add it to the list. */
		n = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));	
		if (!n) {
			free(attr);
			free(value);
			return cgiParseMemory;
		}
		n->attr = attr;
		n->value = value;
		n->valueLength = strlen(n->value);
		n->fileName = (char *) malloc(1);
		if (!n->fileName) {
			free(attr);
			free(value);
			free(n);
			return cgiParseMemory;
		}	
		n->fileName[0] = '\0';
		n->contentType = (char *) malloc(1);
		if (!n->contentType) {
			free(attr);
			free(value);
			free(n->fileName);
			free(n);
			return cgiParseMemory;
		}	
		n->contentType[0] = '\0';
		n->tfileName = (char *) malloc(1);
		if (!n->tfileName) {
			free(attr);
			free(value);
			free(n->fileName);
			free(n->contentType);
			free(n);
			return cgiParseMemory;
		}	
		n->tfileName[0] = '\0';
		n->next = 0;
		if (!l) {
			cgiFormEntryFirst = n;
		} else {
			l->next = n;
		}
		l = n;
		if (!foundAmp) {
			break;
		}			
	}
	return cgiParseSuccess;
}

static void cgiFreeResources() {
    cgiFormEntry *c = cgiFormEntryFirst;
    cgiFormEntry *n;
    while (c) {
      n = c->next;
      free(c->attr);
      free(c->value);
      free(c->fileName);
      free(c->contentType);
      if (strlen(c->tfileName)) {
        unlink(c->tfileName);
      }
      free(c->tfileName);
      free(c);
      c = n;
    }
    cgiFormEntryFirst = NULL;
    return;
}
