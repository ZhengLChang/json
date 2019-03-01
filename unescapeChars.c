#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int HexValue[256];
static void SetupConstants();
typedef enum {
  EscapeRest,
  EscapeFirst,
  EscapeSecond
} EscapeState;

typedef enum {
  UnescapeSuccess,
  UnescapeMemory
} UnescapeResultType;
typedef enum {
  ParseSuccess,
  ParseMemory,
  ParseIO
} ParseResultType;
typedef struct FormEntryStruct {
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
  struct FormEntryStruct *next;
} FormEntry;
static FormEntry *FormEntryFirst;
UnescapeResultType UnescapeChars(char **sp, char *cp, int len);
static ParseResultType ParseFormInput(char *data, int length);
static void FreeResources();
int cgigetinteger_simple(char *name, const int def);
const char *cgigetstring_simple(char *name, const char *def);
int main(void){ 
  char *sp = NULL;
  char cp[] = "PnPEnable=1&DHCPEnable=1&dhcpCustomOptionFor66=&provisioningServer=&userName=%26&password=&commonAESKey=&MACAESKey=&checkNewConfig=1&repeatedly=0&interval=1440&weekly=0&startHour=3&startMinute=0&stopHour=3&stopMinute=0&dayOfWeekly=1%2C1%2C0%2C0%2C0%2C0%2C1&sunday=1&monday=1&tuesday=0&wednesday=0&thursday=0&friday=0&saturday=1&user_set_auto";
  SetupConstants();
  ParseFormInput(cp, strlen(cp));
  if(FormEntryFirst != NULL){
    struct FormEntryStruct *p = NULL;
    for(p = FormEntryFirst; p ; p = p->next){
      printf("%s = %s\n", p->attr, p->value);
    }
  }
  printf("DHCPEnable = %d\n", cgigetinteger_simple("DHCPEnable", 0));
  printf("userName = %s\n", cgigetstring_simple("userName", ""));
  printf("dayOfWeekly = %s\n", cgigetstring_simple("dayOfWeekly", ""));
  FreeResources();
//  UnescapeChars(&sp, cp, strlen(cp));
//  printf("%s\n", sp);
  return 0;
}

UnescapeResultType UnescapeChars(char **sp, char *cp, int len) {
  char *s;
  EscapeState escapeState = EscapeRest;
  int escapedValue = 0;
  int srcPos = 0;
  int dstPos = 0;
  s = (char *) malloc(len + 1);
  if (!s) {
    return UnescapeMemory;
  }
  while (srcPos < len) {
    int ch = cp[srcPos];
    switch (escapeState) {
      case EscapeRest:
        if (ch == '%') {
          escapeState = EscapeFirst;
        } else if (ch == '+') {
          s[dstPos++] = ' ';
        } else {
          s[dstPos++] = ch; 
        }
        break;
      case EscapeFirst:
        escapedValue = HexValue[ch] << 4;  
        escapeState = EscapeSecond;
        break;
      case EscapeSecond:
        escapedValue += HexValue[ch];
        s[dstPos++] = escapedValue;
        escapeState = EscapeRest;
        break;
    }
    srcPos++;
  }
  s[dstPos] = '\0';
  *sp = s;
  return UnescapeSuccess;
}
static void SetupConstants() {
  int i;
  for (i=0; (i < 256); i++) {
    HexValue[i] = 0;
  }
  HexValue['0'] = 0; 
  HexValue['1'] = 1; 
  HexValue['2'] = 2; 
  HexValue['3'] = 3; 
  HexValue['4'] = 4; 
  HexValue['5'] = 5; 
  HexValue['6'] = 6; 
  HexValue['7'] = 7; 
  HexValue['8'] = 8; 
  HexValue['9'] = 9;
  HexValue['A'] = 10;
  HexValue['B'] = 11;
  HexValue['C'] = 12;
  HexValue['D'] = 13;
  HexValue['E'] = 14;
  HexValue['F'] = 15;
  HexValue['a'] = 10;
  HexValue['b'] = 11;
  HexValue['c'] = 12;
  HexValue['d'] = 13;
  HexValue['e'] = 14;
  HexValue['f'] = 15;
  return;
}

static ParseResultType ParseFormInput(char *data, int length) {
	/* Scan for pairs, unescaping and storing them as they are found. */
	int pos = 0;
	FormEntry *n;
	FormEntry *l = 0;
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
		if (UnescapeChars(&attr, data+start, len)
			!= UnescapeSuccess) {
			return ParseMemory;
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
		if (UnescapeChars(&value, data+start, len)
			!= UnescapeSuccess) {
			free(attr);
			return ParseMemory;
		}	
		/* OK, we have a new pair, add it to the list. */
		n = (FormEntry *) malloc(sizeof(FormEntry));
		if (!n) {
			free(attr);
			free(value);
			return ParseMemory;
		}
		n->attr = attr;
		n->value = value;
		n->valueLength = strlen(n->value);
		n->fileName = (char *) malloc(1);
		if (!n->fileName) {
			free(attr);
			free(value);
			free(n);
			return ParseMemory;
		}	
		n->fileName[0] = '\0';
		n->contentType = (char *) malloc(1);
		if (!n->contentType) {
			free(attr);
			free(value);
			free(n->fileName);
			free(n);
			return ParseMemory;
		}	
		n->contentType[0] = '\0';
		n->tfileName = (char *) malloc(1);
		if (!n->tfileName) {
			free(attr);
			free(value);
			free(n->fileName);
			free(n->contentType);
			free(n);
			return ParseMemory;
		}	
		n->tfileName[0] = '\0';
		n->next = 0;
		if (!l) {
			FormEntryFirst = n;
		} else {
			l->next = n;
		}
		l = n;
		if (!foundAmp) {
			break;
		}			
	}
	return ParseSuccess;
}

static void FreeResources() {
    FormEntry *c = FormEntryFirst;
    FormEntry *n;
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
    FormEntryFirst = NULL;
    return;
}
int cgigetinteger_simple(char *name, const int def){
	FormEntry *next = NULL;
	for(next = FormEntryFirst; next; next = next->next){
		if(strcmp(next->attr, name) == 0){
			return atoi(next->value);
		}
	}
    return def;
}
const char *cgigetstring_simple(char *name, const char *def){
	FormEntry *next = NULL;
	for(next = FormEntryFirst; next; next = next->next){
		if(strcmp(next->attr, name) == 0){
			return next->value;
		}
	}
    return def;
}
