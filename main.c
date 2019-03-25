#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <json.h>

int main(void){
  JsonNode *json = json_mkobject();
  JsonNode *value = json_mkstring("234");
  JsonNode *arr = NULL;
  JsonNode *dataJson = json_mkobject();
  char *p_str = NULL;

  
  /*String*/
  json_append_member(json, "123", value);
#if 1
  json_append_member(json, "123", json_mkstring(NULL));
  json_append_member(json, "123", json_mknumber(1000000));
#endif
  /*String*/
  json_append_member(json, "zheng", json_mkstring("FanFan"));
  /*null*/
  json_append_member(json, "number", json_mknumber(1));

  /*array*/
#if 1
  arr = json_mkarray();
  json_append_element(arr, json_mknumber(1));
  json_append_element(arr, json_mknumber(2));
  json_append_element(arr, json_mknumber(3));
  json_append_element(arr, json_mknumber(0));
  json_append_element(arr, json_mknumber(6));

  json_append_member(json, "arr", arr);
#endif
  /*include object*/
  json_append_member(dataJson, "mac", json_mknumber(110));
  json_append_member(json, "data", dataJson);

  printf("%s", (p_str = json_encode(json)));
  if(p_str != NULL){
    free(p_str);
    p_str = NULL;
  }
  json_delete(json);
  return 0;
}
