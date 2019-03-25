#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <json.h>

int main(void){
  char jsonStr[] = "{\"123\":1000000,\"zheng\":\"FanFan\",\"number\":1,\"arr\":[1,2,3,0,6],\"data\":{\"mac\":110}}";
  JsonNode *json = NULL;

  json = json_decode(jsonStr);
  char *pStr = NULL;
  JsonNode *node = NULL;
  if(json != NULL){
    json_foreach(node, json){
      if((pStr = json_stringify(node, NULL)) != NULL){
        fprintf(stderr, "%s\n", pStr);
        free(pStr);
        pStr = NULL;
      }
    }
      /*
    JsonNode *dataNode = json_find_member(json, "data");
    JsonNode *macNode = json_find_member(dataNode, "mac");
    if(dataNode != NULL){
      if((pStr = json_stringify(macNode, NULL)) != NULL){
        fprintf(stderr, "%s\n", pStr);
        free(pStr);
        pStr = NULL;
      }
    }
  */
  }
  return 0;
}
