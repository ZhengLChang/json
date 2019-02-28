#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <json.h>

int main(void){
  JsonNode *json = json_mkobject();
  JsonNode *value = json_mkstring("234");
  JsonNode *arr = NULL;

  
  /*String*/
  json_append_member(json, "123", value);
  /*String*/
  json_append_member(json, "zheng", json_mkstring("FanFan"));
  /*null*/
  json_append_member(json, "number", json_mknumber(1));

  /*array*/
  arr = json_mkarray();
  json_append_element(arr, json_mknumber(1));
  json_append_element(arr, json_mknumber(2));
  json_append_element(arr, json_mknumber(3));
  json_append_element(arr, json_mknumber(0));
  json_append_element(arr, json_mknumber(6));

  json_append_member(json, "arr", arr);

  printf("%s\n", json_encode(json));
  json_delete(json);
  return 0;
}
